//
//  descriptor_set.h
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#pragma once

#include <sys/select.h>

namespace network {

template <typename T>
concept Container = requires(T const& t) {
	t.begin();
	t.end();
};

struct descriptor_set {
	descriptor_set();
	
	void add(int descriptor);
	template<typename T>
	void add(T const& a) {
		add(a.descriptor());
	}
	template<Container T>
	void add(T const& c) {
		for(auto& a : c) {
			add(a);
		}
	}
	
	void remove(int descriptor);
	template<typename T>
	void remove(T const& a) {
		remove(a.descriptor());
	}
	
	bool contains(int descriptor) const;
	template<typename T>
	bool contains(T const& a) const {
		return contains(a.descriptor());
	}
	
	void clear();
	
	bool empty() const;
	
	::fd_set fd_set;
	int      max_fd;
};

}
