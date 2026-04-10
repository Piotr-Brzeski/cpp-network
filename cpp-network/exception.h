//
//  exception.h
//  cpp-network
//
//  Created by Piotr Brzeski on 2023-03-03.
//  Copyright © 2023-2026 Brzeski.net. All rights reserved.
//

#pragma once

#include <string>
#include <cstring>
#include <cerrno>
#include <stdexcept>

namespace error {

inline std::string const& errno_str() {
	thread_local auto error_string = std::string();
	int error_code = errno;
	char buf[256];
	strerror_r(error_code, buf, sizeof(buf));
	error_string = buf;
	return error_string;
}

class exception : public std::runtime_error {
public:
	explicit exception(char const* message)
		: std::runtime_error(message + (": " + errno_str()))
	{
	}
};

}
