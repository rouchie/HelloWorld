#pragma once

#include "3rdparty.h"

class CThreadPool {
public:
	static CThreadPool& inst() {
		static CThreadPool inst(4);
		return inst;
	}

public:
	template<class F, class... Args>
	void enqueue(F&& f, Args &&... args) {
		auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

		{
			std::unique_lock<std::mutex> lock(queueMutex);
			tasks.emplace(task);
		}

		condition.notify_one();
	}

	~CThreadPool() {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}

		condition.notify_all();

		for (std::thread& worker : workers) {
			worker.join();
		}
	}

private:
	CThreadPool(size_t numThreads) : stop(false) {
		for (size_t i = 0; i < numThreads; ++i) {
			workers.emplace_back(
				[this] {
					while (true) {
						std::function<void()> task;

						{
							std::unique_lock<std::mutex> lock(queueMutex);
							condition.wait(lock, [this] { return stop || !tasks.empty(); });

							if (stop && tasks.empty()) {
								return;
							}

							task = std::move(tasks.front());
							tasks.pop();
						}

						task();
					}
				}
			);
		}
	}

private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;

	std::mutex queueMutex;
	std::condition_variable condition;

	bool stop;
};

//{ 创建一个线程
	template <typename T, typename ...A>
    std::thread CreateThread(T && f, A && ... a) {
		std::thread t(std::forward<T>(f), std::forward<A>(a)...);
		t.detach();
        return t;
	}
//}

