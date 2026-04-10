//
//  address.h
//  cpp-network
//
//  Created by Piotr Brzeski on 2023-10-24.
//  Copyright © 2023 Brzeski.net. All rights reserved.
//

#pragma once

#include <string>
#include <cstdint>

namespace network {

class ipv4_address {
public:
	constexpr ipv4_address() = default;
	constexpr ipv4_address(std::uint32_t value)
		: m_address(value)
	{
	}
	constexpr ipv4_address(std::uint8_t octet1, std::uint8_t octet2, std::uint8_t octet3, std::uint8_t octet4)
		: m_address(octet1 + (octet2 << 8) + (octet3 << 16) + (octet4 << 24))
	{
	}
	
	std::uint32_t value() const {
		return m_address;
	}
	
	std::string str() const {
		return
			std::to_string(m_address & 0xFF) + '.' +
			std::to_string((m_address >> 8) & 0xFF) + '.' +
			std::to_string((m_address >> 16) & 0xFF) + '.' +
			std::to_string((m_address >> 24) & 0xFF);
	}
private:
	std::uint32_t m_address = 0;
};

}
