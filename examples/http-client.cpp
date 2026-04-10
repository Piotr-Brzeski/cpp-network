//
//  http-client.cpp
//  http-client
//
//  Created by Piotr Brzeski on 2023-03-01.
//  Copyright © 2023-2026 Brzeski.net. All rights reserved.
//

#include <cpp-network/socket.h>
#include <cpp-log/log.h>
#include <iostream>

int main() {
	try {
		auto log = logger::start(logger::cout());
		auto address = network::ipv4_address(127, 0, 0, 1);
		logger::log(address.str());
		auto socket = network::tcp_socket();
		socket.connect(address, 80);
		auto request = network::buffer("GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");
		socket.send(request);
		while(auto response = socket.recv()) {
			std::cout << response->size() << '\n' << response->c_str() << std::endl;
		}
		
		return 0;
	}
	catch(std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Unknown Exception" << std::endl;
	}
	return 1;
}
