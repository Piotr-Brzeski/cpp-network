//
//  socket.h
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#pragma once

#include <string>
#include <cstdint>
#include <stdexcept>

namespace network {

using packet = std::basic_string<std::uint8_t>;

class socket {
public:
	using exception = std::runtime_error;
	
	~socket();
	
	socket(socket&& s);
	socket& operator=(socket&& s);
	
	socket(socket const&) = delete;
	socket& operator=(socket const& s) = delete;

	void bind(int port);
	packet recv(std::size_t max_size);
	
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
