//
//  server.cpp
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022-2023 Brzeski.net. All rights reserved.
//

#include "server.h"
#include <cpp-log/log.h>
#include <cassert>

using namespace network;

server::server() {
}

server::~server() {
	stop();
}

void server::stop() {
	auto lock = std::unique_lock(m_mutex);
	switch(m_state) {
		case state::ready:
			return;
		case state::stopping:
			throw exception("server::stop failed - stop is already in progress.");
		case state::running:
			m_state = state::stopping;
			m_select.wake();
			break;
		case state::stopped:
			break;
	}
	wait(lock);
}

void server::wait() {
	auto lock = std::unique_lock(m_mutex);
	wait(lock);
}

void server::wait(std::unique_lock<std::mutex>& lock) {
	while(m_state != state::stopped && m_state != state::ready) {
		m_condition.wait(lock);
	}
	if(m_state == state::stopped) {
		if(m_server_thread.joinable()) {
			m_server_thread.join();
		}
		m_state = state::ready;
	}
}

bool server::is_running() {
	auto lock = std::lock_guard(m_mutex);
	return m_state == state::running;
}

// MARK: udp_server
void udp_server::start(int port) {
	auto lock = std::lock_guard(m_mutex);
	if(m_state != state::ready) {
		throw exception("server::start failed - server is already running.");
	}
	m_server_thread = std::thread([this, port](){
		try {
			m_socket.bind(port);
			while(is_running()) {
				auto set = m_select.wait(m_socket, m_period);
				if(set.contains(m_socket)) {
					auto msg = m_socket.recvfrom(packet_size);
					if(callback) {
						callback(std::move(msg));
					}
				}
				else if(m_periodic_task) {
					m_periodic_task();
				}
			}
		}
		catch(std::exception &e) {
			log::log(std::string("server finished due to exception: ") + e.what());
		}
		catch(...) {
			log::log("server finished due to unknown exception");
		}
		auto lock = std::lock_guard(m_mutex);
		assert(m_state == state::running);
		m_state = state::stopped;
		m_condition.notify_all();
	});
	m_state = state::running;
}
