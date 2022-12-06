//
//  notifier.cpp
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022 Brzeski.net. All rights reserved.
//

#include "notifier.h"
#include <unistd.h>

using namespace network;

notifier::notifier()
{
	::pipe(m_descriptors);
}

notifier::~notifier() {
	if(m_descriptors[1] >= 0) {
		::close(m_descriptors[1]);
	}
	if(m_descriptors[0] >= 0) {
		::close(m_descriptors[0]);
	}
}

void notifier::notify() {
	std::lock_guard lock(m_mutex);
	if(!m_notified) {
		::write(m_descriptors[1], "", 1);
		m_notified = true;
	}
}

void notifier::clear() {
	std::lock_guard lock(m_mutex);
	if(m_notified) {
		char c;
		::read(m_descriptors[0], &c, 1);
		m_notified = false;
	}
}
