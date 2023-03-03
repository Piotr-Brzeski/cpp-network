//
//  socket.h
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022-2023 Brzeski.net. All rights reserved.
//

#pragma once

#include "message.h"
#include "exception.h"

namespace network {

class socket {
public:
	using exception = error::exception;
	
	~socket();
	
	socket(socket&& s);
	socket& operator=(socket&& s);
	
	socket(socket const&) = delete;
	socket& operator=(socket const& s) = delete;

	void bind(int port);
	buffer recv(std::size_t max_size);
	message recvfrom(std::size_t max_size);

	int descriptor() const {
		return m_descriptor;
	}
	
protected:
	socket(int descriptor);
	static int create(int type);
	
private:
	int m_descriptor;
};

// MARK: - UDP socket
class udp_socket: public socket {
public:
	udp_socket();
	
	void broadcast(int port, buffer const& data);
	void send(ipv4_address address, int port, buffer const& data);
	
private:
	bool m_can_broadcast = false;
};

// MARK: - TCP socket
class tcp_socket: public socket {
public:
	tcp_socket();
	
	void listen();
	tcp_socket accept();

private:
	tcp_socket(int descriptor);
};

}
