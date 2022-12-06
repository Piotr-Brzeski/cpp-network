//
//  descriptor_set.cpp
//  Network
//
//  Created by Piotr Brzeski on 2022-12-06.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#include "descriptor_set.h"
#include <cstring>

using namespace network;

descriptor_set::descriptor_set()
	: max_fd(0)
{
	FD_ZERO(&fd_set);
}

void descriptor_set::add(int descriptor) {
	FD_SET(descriptor, &fd_set);
	if(++descriptor > max_fd) {
		max_fd = descriptor;
	}
}

void descriptor_set::remove(int descriptor) {
	FD_CLR(descriptor, &fd_set);
}


bool descriptor_set::contains(int descriptor) const {
	return FD_ISSET(descriptor, &fd_set) != 0;
}

void descriptor_set::clear() {
	FD_ZERO(&fd_set);
	max_fd = 0;
}

bool descriptor_set::empty() const {
	static auto empty_set = [](){ ::fd_set s; FD_ZERO(&s); return s; }();
	return std::memcmp(&fd_set, &empty_set, sizeof(empty_set)) == 0;
}
