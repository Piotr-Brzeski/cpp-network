//
//  select.h
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#pragma once

#include "descriptor_set.h"
#include "notifier.h"
#include <chrono>
#include <optional>

namespace network {

class select {
public:
	select();
	~select();
	
	select(select const&) = delete;
	select(select&&) = delete;
	select& operator=(select const&) = delete;
	select& operator=(select&&) = delete;

	template<typename Arg, typename... Args>
	descriptor_set wait(Arg const& arg, Args... args) {
		descriptor_set descriptors;
		descriptors.add(arg);
		return wait(descriptors, args...);
	}
	template<typename Arg, typename... Args>
	descriptor_set wait(descriptor_set descriptors, Arg arg, Args... args) {
		descriptors.add(arg);
		return wait(descriptors, args...);
	}
	descriptor_set wait(descriptor_set descriptors) {
		return wait(descriptors, std::optional<std::chrono::nanoseconds>());
	}
	descriptor_set wait(descriptor_set descriptors, std::nullopt_t) {
		return wait(descriptors, std::optional<std::chrono::nanoseconds>());
	}
	template<typename Rep, typename Period>
	descriptor_set wait(descriptor_set descriptors, std::chrono::duration<Rep, Period> timeout) {
		return wait(descriptors, std::optional(std::chrono::duration_cast<std::chrono::nanoseconds>(timeout)));
	}
	descriptor_set wait(descriptor_set descriptors, std::optional<std::chrono::nanoseconds> timeout);
	
	void wake();
	
private:
	notifier m_notifier;
};

}
