//
//  socket.cpp
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022-2023 Brzeski.net. All rights reserved.
//

#include "socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace network;

namespace {

class socket_address {
public:
	explicit socket_address(int port = 0, ipv4_address address = INADDR_ANY) {
		m_address.sin_family = AF_INET;
		m_address.sin_addr.s_addr = address;
		m_address.sin_port = htons(port);
	}
	
	ipv4_address get() const {
		return m_address.sin_addr.s_addr;
	}
	
	::socklen_t size() {
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

buffer socket::recv(std::size_t max_size) {
	auto data = buffer(max_size, 0);
	auto size = ::recv(m_descriptor, data.data(), max_size, MSG_NOSIGNAL);
	if(size < 0) {
		throw exception("socket::recv failed.");
	}
	data.resize(size);
	return data;
}

message socket::recvfrom(std::size_t max_size) {
	auto msg = message(max_size);
	auto address = socket_address();
	auto address_size = address.size();
	auto size = ::recvfrom(m_descriptor, msg.content.data(), max_size, MSG_NOSIGNAL, address.pointer(), &address_size);
	assert(address_size == address.size());
	if(size < 0) {
		throw exception("socket::recvfrom failed.");
	}
	msg.content.resize(size);
	msg.source = address.get();
	return msg;
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
	send(INADDR_BROADCAST, port, data);
}

void udp_socket::send(ipv4_address address_value, int port, buffer const& data) {
	auto address = socket_address(port, address_value);
	auto send = ::sendto(descriptor(), data.data(), data.size(), 0, address.pointer(), address.size());
	if(send != data.size()) {
		throw exception("udp_socket::send failed");
	}
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

