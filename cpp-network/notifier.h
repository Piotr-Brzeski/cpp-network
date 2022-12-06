//
//  notifier.h
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#pragma once

#include <mutex>

namespace network {

class notifier {
public:
	notifier();
	~notifier();
	
	notifier(notifier const&) = delete;
	notifier(notifier&&) = delete;
	notifier& operator=(notifier const&) = delete;
	notifier& operator=(notifier&&) = delete;
	
	void notify();
	void clear();
	
	int descriptor() const {
		return m_descriptors[0];
	}
	
private:
	std::mutex m_mutex;
	bool       m_notified = false;
	int        m_descriptors[2];
};

}

