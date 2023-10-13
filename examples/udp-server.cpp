//
//  udp-server.cpp
//  udp-server
//
//  Created by Piotr Brzeski on 2022-12-06.
//  Copyright © 2022-2023 Brzeski.net. All rights reserved.
//

#include <cpp-network/server.h>
#include <cpp-log/log.h>
#include <iostream>

int main(int argc, const char * argv[]) {
	try {
		auto log = logger::start(logger::cout());
		auto server = network::udp_server();
		server.callback = [](network::message message){
			std::cout << "Recv from " << message.source_str() << " [ ";
			for(auto i : message.content) {
				std::cout << static_cast<unsigned int>(i) << " ";
			}
			std::cout << "]" << std::endl;
		};
		server.set_periodic_task([](){ logger::log("Periodic task"); }, std::chrono::seconds(10));
		int port = 1234;
		server.start(port);
		std::cout << "Server started on UDP port " << port << std::endl;
		std::cin >> port;
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
