//
// Created by rouchie on 24-11-6.
//

#pragma once

#include <condition_variable>

// 使用互斥锁和条件变量做信号量

class CSemaphore
{
public:
	explicit CSemaphore(const unsigned int count = 0) : m_count(count) {}

	void Post() {
		std::unique_lock<std::mutex> lock(_mutex);
		++m_count;
		_cv.notify_one();
	}

	void Wait() {
		std::unique_lock<std::mutex> lock(_mutex);
		_cv.wait(lock, [=] {return m_count > 0; });
		--m_count;
	}

	bool TryWait() {
		std::unique_lock<std::mutex> lock(_mutex);
		if (m_count <= 0) return false;

		_cv.wait(lock, [=] {return m_count > 0; });
		--m_count;
		return true;
	}

private:
	std::mutex _mutex;
	std::condition_variable _cv;
	unsigned int m_count;
};

