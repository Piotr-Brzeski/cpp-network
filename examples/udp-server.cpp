//
//  udp-server.cpp
//  udp-server
//
//  Created by Piotr Brzeski on 2022-12-06.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#include <cpp-network/server.h>
#include <iostream>

int main(int argc, const char * argv[]) {
	try {
		auto server = network::udp_server();
		server.callback = [](network::packet data){
			std::cout << "Recv [ ";
			for(auto i : data) {
				std::cout << static_cast<unsigned int>(i) << " ";
			}
			std::cout << "]" << std::endl;
		};
		int port = 1234;
		server.start(port);
		std::cout << "Server started on UDP port " << port << std::endl;
		std::cin >> port;
		return 0;
	}
	catch(...) {
	}
	return 1;
}
