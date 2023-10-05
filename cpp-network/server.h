//
//  server.h
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022-2023 Brzeski.net. All rights reserved.
//

#pragma once

#include "socket.h"
#include "select.h"
#include "exception.h"
#include <thread>
#include <atomic>
#include <stdexcept>
#include <functional>

namespace network {

class server {
public:
	using exception = error::exception;
	
	server();
	~server();
	
	server(server const&) = delete;
	server(server&&) = delete;
	server& operator=(server const&) = delete;
	server& operator=(server&&) = delete;
	
	void stop();
	void wait();
	
protected:
	bool is_running();
	void wait(std::unique_lock<std::mutex>& lock);
	enum class state { ready, running, stopping, stopped };
	
	select                  m_select;
	std::thread             m_server_thread;
	state                   m_state = state::ready;
	std::mutex              m_mutex;
	std::condition_variable m_condition;
	
	
};

// MARK: udp_server
class udp_server: public server {
public:
	void start(int port);
	
	std::size_t                  packet_size = 16;
	std::function<void(message)> callback;
	
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
