//
//  message.h
//  cpp-network
//
//  Created by Piotr Brzeski on 2023-03-03.
//  Copyright © 2023 Brzeski.net. All rights reserved.
//

#pragma once

#include <string>
#include <cstdint>

namespace network {

using buffer = std::basic_string<std::uint8_t>;
using ipv4_address = std::uint32_t;
constexpr ipv4_address invalid_ipv4_address = 0;

inline std::string ipv4_str(ipv4_address address) {
	return
		std::to_string(address & 0xFF) + '.' +
		std::to_string((address >> 8) & 0xFF) + '.' +
		std::to_string((address >> 16) & 0xFF) + '.' +
		std::to_string((address >> 24) & 0xFF);
}

struct message {
	explicit message(std::size_t size = 0) {
		content.resize(size);
	}
	
	std::string source_str() const {
		return ipv4_str(source);
	}
	
	ipv4_address source = 0;
	buffer       content;
};

}
