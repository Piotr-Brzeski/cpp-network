//
//  exception.h
//  cpp-network
//
//  Created by Piotr Brzeski on 2023-03-03.
//  Copyright © 2023 Brzeski.net. All rights reserved.
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
	// TODO: Protect this calll, as strerror may not be thread-safe
	error_string = std::strerror(error_code);
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
