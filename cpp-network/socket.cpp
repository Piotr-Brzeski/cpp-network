//
//  socket.cpp
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#include "socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace network;

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
	::sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if(::bind(m_descriptor, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
		throw exception("socket::bind failed.");
	}
}

packet socket::recv(std::size_t max_size) {
	auto buffer = packet(max_size, 0);
	auto size = ::recv(m_descriptor, buffer.data(), max_size, MSG_NOSIGNAL);
	if(size < 0) {
		throw exception("socket::recv failed.");
	}
	buffer.resize(size);
	return buffer;
}

// MARK: - UDP socket
udp_socket::udp_socket()
	: socket(create(SOCK_DGRAM))
{
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

