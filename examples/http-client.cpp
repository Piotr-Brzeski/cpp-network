//
//  http-client.cpp
//  http-client
//
//  Created by Piotr Brzeski on 2023-03-01.
//  Copyright © 2023 Brzeski.net. All rights reserved.
//

#include <cpp-network/socket.h>
#include <cpp-log/log.h>
#include <iostream>

int main(int argc, const char * argv[]) {
	try {
		auto log = logger::start(logger::cout());
//		auto address = network::ipv4_address(127, 0, 0, 1);
		auto address = network::ipv4_address(91, 121, 86, 137);
		logger::log(address.str());
		auto socket = network::tcp_socket();
		socket.connect(address, 80);
//		auto request = network::buffer(reinterpret_cast<const std::uint8_t*>("GET / HTTP/1.1\r\n\r\n"));
		auto request = network::buffer(reinterpret_cast<const std::uint8_t*>("GET http://91.121.86.137/\r\n\r\n"));
		socket.send(request);
		auto response = socket.recv();
		std::cout << response.size() << '\n' << response.c_str() << std::endl;
		response = socket.recv();
		std::cout << response.size() << '\n' << response.c_str() << std::endl;
		response = socket.recv();
		std::cout << response.size() << '\n' << response.c_str() << std::endl;
		response = socket.recv();
		std::cout << response.size() << '\n' << response.c_str() << std::endl;
		
//		auto message = network::buffer();
//		message.append(5, 'a');
//		while(true) {
//			int i = 0; std::cin >> i;
//			socket.broadcast(1234, message);
//		}
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
