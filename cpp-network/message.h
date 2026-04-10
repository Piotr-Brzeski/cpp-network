//
//  message.h
//  cpp-network
//
//  Created by Piotr Brzeski on 2023-03-03.
//  Copyright © 2023-2026 Brzeski.net. All rights reserved.
//

#pragma once

#include "address.h"

namespace network {

using buffer = std::string;

struct message {
	explicit message(std::size_t size = 0) {
		content.resize(size);
	}
	
	std::string source_str() const {
		return source.str();
	}
	
	ipv4_address source;
	buffer       content;
};

}
