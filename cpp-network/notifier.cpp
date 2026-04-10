//
//  notifier.cpp
//  Network
//
//  Created by Piotr Brzeski on 2022-12-04.
//  Copyright © 2022-2026 Brzeski.net. All rights reserved.
//

#include "notifier.h"
#include "exception.h"
#include <unistd.h>

using namespace network;

notifier::notifier()
{
	if(::pipe(m_descriptors) != 0) {
		throw error::exception("notifier: pipe failed");
	}
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
		if(::write(m_descriptors[1], "", 1) != 1) {
			throw error::exception("notifier::notify failed");
		}
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
