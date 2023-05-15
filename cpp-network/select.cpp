//
//  select.cpp
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022-2023 Brzeski.net. All rights reserved.
//

#include "select.h"
#include <cerrno>
#include <initializer_list>
#include <chrono>
#include <stdexcept>

using namespace network;

namespace {

int pselect(descriptor_set& in_set, descriptor_set& err_set, std::optional<std::chrono::nanoseconds> timeout) {
	if(timeout) {
		if(*timeout >= std::chrono::nanoseconds::zero()) {
			auto seconds = std::chrono::floor<std::chrono::seconds>(*timeout);
			auto nanoseconds = *timeout - seconds;
			::timespec ts;
			ts.tv_sec = seconds.count();
			ts.tv_nsec = nanoseconds.count();
			return ::pselect(in_set.max_fd, &in_set.fdset, nullptr, &err_set.fdset, &ts, nullptr);
		}
		in_set.clear();
		err_set.clear();
		return 0;
	}
	return ::pselect(in_set.max_fd, &in_set.fdset, nullptr, &err_set.fdset, nullptr, nullptr);
}

}

select::select() {
	
}

select::~select() {
	
}

descriptor_set select::wait(descriptor_set descriptors, std::optional<std::chrono::nanoseconds> timeout) {
	m_notifier.clear();
	descriptors.add(m_notifier);
	while(true) {
		auto in_set = descriptors;
		auto err_set = descriptors;
		auto start_time = std::chrono::steady_clock::now();
		int sel = pselect(in_set, err_set, timeout);
		if(sel >= 0) {
			if(err_set.empty()) {
				in_set.remove(m_notifier);
				return in_set;
			}
			throw std::runtime_error("Select error.");
		}
		if(errno != EINTR) {
			throw std::runtime_error("Select error.");
		}
		if(timeout) {
			auto now = std::chrono::steady_clock::now();
			auto select_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time);
			*timeout -= select_duration;
		}
	}
}

void select::wake() {
	m_notifier.notify();
}
