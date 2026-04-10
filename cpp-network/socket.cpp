//
//  socket.cpp
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022-2026 Brzeski.net. All rights reserved.
//

#include "socket.h"
#include <array>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace network;

namespace {

class socket_address {
public:
	explicit socket_address(int port = 0, ipv4_address address = ipv4_address(INADDR_ANY)) {
		m_address.sin_family = AF_INET;
		m_address.sin_addr.s_addr = address.value();
		m_address.sin_port = htons(port);
	}
	
	ipv4_address get() const {
		return ipv4_address(m_address.sin_addr.s_addr);
	}
	
	::socklen_t size() const {
		return sizeof(m_address);
	}
	sockaddr* pointer() {
		return reinterpret_cast<sockaddr*>(&m_address);
	}
	sockaddr const * pointer() const {
		return reinterpret_cast<sockaddr const*>(&m_address);
	}
	
private:
	::sockaddr_in m_address = {};
};

}

socket::socket(int descriptor)
	: m_descriptor(descriptor)
{
}

int socket::create(int type)
{
	auto descriptor = ::socket(AF_INET, type, 0);
	if(descriptor < 0) {
		throw exception("socket creation failed.");
	}
	// Enable address reuse to avoid bind error after socket close
	int on = 1;
	::setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	return descriptor;
}

socket::~socket() {
	if(m_descriptor >= 0) {
		::close(m_descriptor);
	}
}

socket::socket(socket&& s)
	: m_descriptor(s.descriptor())
{
	s.m_descriptor = -1;
}

network::socket& socket::operator=(socket&& s) {
	if(this != &s) {
		if(m_descriptor >= 0) {
			::close(m_descriptor);
		}
		m_descriptor = s.descriptor();
		s.m_descriptor = -1;
	}
	return *this;
}

void socket::bind(int port) {
	auto address = socket_address(port);
	if(::bind(m_descriptor, address.pointer(), address.size()) != 0) {
		throw exception("socket::bind failed.");
	}
}

// MARK: - UDP socket
udp_socket::udp_socket()
	: socket(create(SOCK_DGRAM))
{
}

void udp_socket::broadcast(int port, const buffer &data) {
	if(!m_can_broadcast) {
		int enabled = 1;
		if(::setsockopt(descriptor(), SOL_SOCKET, SO_BROADCAST, &enabled, sizeof(enabled)) != 0) {
			throw exception("udp_socket::broadcast failed: can not enable broadcast");
		}
		m_can_broadcast = true;
	}
	send(ipv4_address(INADDR_BROADCAST), port, data);
}

void udp_socket::send(ipv4_address address_value, int port, buffer const& data) {
	auto address = socket_address(port, address_value);
	auto send = ::sendto(descriptor(), data.data(), data.size(), 0, address.pointer(), address.size());
	if(send < 0 || static_cast<std::size_t>(send) != data.size()) {
		throw exception("udp_socket::send failed");
	}
}

buffer udp_socket::recv(std::size_t max_size) {
	auto data = buffer(max_size, 0);
	auto size = ::recv(descriptor(), data.data(), max_size, MSG_NOSIGNAL);
	if(size < 0) {
		throw exception("udp_socket::recv failed.");
	}
	data.resize(size);
	return data;
}

message udp_socket::recvfrom(std::size_t max_size) {
	auto msg = message(max_size);
	auto address = socket_address();
	auto address_size = address.size();
	auto size = ::recvfrom(descriptor(), msg.content.data(), max_size, MSG_NOSIGNAL, address.pointer(), &address_size);
	if(address_size != address.size()) {
		throw exception("udp_socket::recvfrom failed: unexpected address size.");
	}
	if(size < 0) {
		throw exception("udp_socket::recvfrom failed.");
	}
	msg.content.resize(size);
	msg.source = address.get();
	return msg;
}

// MARK: - TCP socket
tcp_socket::tcp_socket()
	: socket(create(SOCK_STREAM))
{
}

tcp_socket::tcp_socket(int descriptor)
	: socket(descriptor)
{
}

void tcp_socket::listen() {
	constexpr int max_queue_size = 8;
	if(::listen(descriptor(), max_queue_size) != 0) {
		throw exception("socket::listen failed.");
	}
}

tcp_socket tcp_socket::accept() {
	auto new_desciptor = ::accept(descriptor(), nullptr, nullptr);
	if(new_desciptor < 0) {
		throw exception("socket::accept failed.");
	}
	return tcp_socket(new_desciptor);
}

void tcp_socket::connect(ipv4_address address_value, int port) {
	auto address = socket_address(port, address_value);
	if (::connect(descriptor(), address.pointer(), address.size()) != 0) {
		throw exception("socket::connect failed.");
	}
}

void tcp_socket::send(buffer const& data) {
	std::size_t offset = 0;
	while(offset < data.size()) {
		auto sent = ::send(descriptor(), data.data() + offset, data.size() - offset, MSG_NOSIGNAL);
		if(sent < 0) {
			throw exception("socket::send failed.");
		}
		offset += static_cast<std::size_t>(sent);
	}
}

std::optional<buffer> tcp_socket::recv(bool read_all) {
	thread_local auto local_buffer = std::array<char, 1024>();
	auto size = ::recv(descriptor(), local_buffer.data(), local_buffer.size(), MSG_NOSIGNAL);
	if(size < 0) {
		throw exception("socket::recv failed.");
	}
	if(size == 0) {
		return std::nullopt;
	}
	auto data = buffer(local_buffer.data(), size);
	if(read_all) {
		while(size > 0) {
			size = ::recv(descriptor(), local_buffer.data(), local_buffer.size(), MSG_NOSIGNAL);
			if(size < 0) {
				throw exception("socket::recv failed.");
			}
			data.append(local_buffer.data(), size);
		}
	}
	else {
		while(size == static_cast<ssize_t>(local_buffer.size())) {
			size = ::recv(descriptor(), local_buffer.data(), local_buffer.size(), MSG_NOSIGNAL | MSG_DONTWAIT);
			if(size < 0) {
				if(errno == EAGAIN || errno == EWOULDBLOCK) {
					break;
				}
				throw exception("socket::recv failed.");
			}
			if(size == 0) {
				break;
			}
			data.append(local_buffer.data(), size);
		}
	}
	return data;
}
