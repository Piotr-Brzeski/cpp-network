//
//  udp-client.cpp
//  udp-client
//
//  Created by Piotr Brzeski on 2023-03-01.
//  Copyright © 2023 Brzeski.net. All rights reserved.
//

#include <cpp-network/server.h>
#include <iostream>

int main(int argc, const char * argv[]) {
	try {
		auto socket = network::udp_socket();
		auto message = network::buffer();
		message.append(5, 'a');
		while(true) {
			int i = 0; std::cin >> i;
			socket.broadcast(1234, message);
		}
	}
	catch(std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Unknown Exception" << std::endl;
	}
	return 1;
}
