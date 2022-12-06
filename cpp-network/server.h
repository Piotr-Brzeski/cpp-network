//
//  server.h
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#pragma once

#include "socket.h"
#include "select.h"
#include <thread>
#include <atomic>
#include <stdexcept>
#include <functional>

namespace network {

class server {
public:
	using exception = std::runtime_error;
	
	server();
	~server();
	
	server(server const&) = delete;
	server(server&&) = delete;
	server& operator=(server const&) = delete;
	server& operator=(server&&) = delete;
	
	void start(int port);
	void stop();
	
protected:
	select           m_select;
	std::thread      m_server_thread;
	std::thread      m_worker_thread;
	std::atomic<int> m_state;
};

// MARK: udp_server
class udp_server: public server {
public:
	void start(int port);
	
	std::size_t                 packet_size = 16;
	std::function<void(packet)> callback;
	
private:
	udp_socket m_socket;
};

// MARK: tcp_server
/*
class tcp_server: public server {
public:
	void start(int port);
	
	std::function<void(tcp_socket)> socket_callback;
	std::function<void(packet)>     packet_callback;
	
private:
	tcp_socket m_socket;
};
*/

}
