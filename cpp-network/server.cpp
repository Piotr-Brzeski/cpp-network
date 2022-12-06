//
//  server.cpp
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#include "server.h"

using namespace network;

server::server() {
	m_state = 0;
}

server::~server() {
	stop();
}

void server::stop() {
	int state = 1;
	if(m_state.compare_exchange_strong(state, 0)) {
		m_select.wake();
		m_server_thread.join();
	}
}

// MARK: udp_server
void udp_server::start(int port) {
	int state = 0;
	if(!m_state.compare_exchange_strong(state, 1)) {
		throw exception("server::start failed - server is alresy running.");
	}
	m_server_thread = std::thread([this, port](){
		try {
			m_socket.bind(port);
			while(m_state == 1) {
				auto set = m_select.wait(m_socket);
				if(set.contains(m_socket)) {
					auto data = m_socket.recv(packet_size);
					if(callback) {
						callback(std::move(data));
					}
				}
			}
		}
		catch(...) {
			// TODO: Handle exceptions
		}
	});
}
