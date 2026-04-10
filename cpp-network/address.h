//
//  address.h
//  cpp-network
//
//  Created by Piotr Brzeski on 2023-10-24.
//  Copyright © 2023-2026 Brzeski.net. All rights reserved.
//

#pragma once

#include <string>
#include <cstdint>
#include <arpa/inet.h>

namespace network {

class ipv4_address {
public:
	constexpr ipv4_address() = default;
	constexpr ipv4_address(std::uint32_t value)
		: m_address(value)
	{
	}
	ipv4_address(std::uint8_t octet1, std::uint8_t octet2, std::uint8_t octet3, std::uint8_t octet4)
		: m_address(htonl((static_cast<std::uint32_t>(octet1) << 24) |
		                  (static_cast<std::uint32_t>(octet2) << 16) |
		                  (static_cast<std::uint32_t>(octet3) << 8)  |
		                   static_cast<std::uint32_t>(octet4)))
	{
	}
	
	std::uint32_t value() const {
		return m_address;
	}
	
	std::string str() const {
		auto host = ntohl(m_address);
		return
			std::to_string((host >> 24) & 0xFF) + '.' +
			std::to_string((host >> 16) & 0xFF) + '.' +
			std::to_string((host >> 8) & 0xFF) + '.' +
			std::to_string(host & 0xFF);
	}
private:
	std::uint32_t m_address = 0;
};

}
