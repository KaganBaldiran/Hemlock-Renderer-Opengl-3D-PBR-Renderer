#pragma once
#include "Log.h"
#include <functional>
#include <queue>
#include <thread>
#include <mutex>

class ThreadPool
{
public:

	ThreadPool(size_t numberOfThreads , size_t maxTaskCount)
	{
		this->maxTaskCount = maxTaskCount;

		for (size_t i = 0; i < numberOfThreads; i++)
		{
			threads.emplace_back([this] {
				while (true)
				{
					std::function<void()> task;

					{
						std::unique_lock<std::mutex> lock(mutex);
						condition.wait(lock, [this] {return !tasks.empty() || stop; });
						if (stop && tasks.empty())
						{
							return;
						}
						task = std::move(tasks.front());
						tasks.pop();
					}
					try
					{
						task();
					}
					catch (const std::exception& ex)
					{
						LOG_ERR("Task ended with an exception: :: "<< ex.what() << typeid(task).name());
					}

					LOG_INF("Thread executed the task!");
				}
			});
		}

		LOG_INF("Thread pool initialized!");
	}

	template<class F>
	void enqueue(F&& task)
	{
		if (tasks.size() <= maxTaskCount)
		{
			{
				std::unique_lock<std::mutex> lock(mutex);
				tasks.emplace(std::forward<F>(task));
			}
			condition.notify_one();
		}
		else
		{
			LOG_WARN("Reached the maximum count of tasks in queue!");
		}
	}

	~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(mutex);
			stop = true;
		}

		condition.notify_all();

		int Index = 0;
		for (auto &thread : threads)
		{
			thread.join();
			LOG_INF("Thread "<< Index <<" is distrupted!");
			Index++;
		}
	}


private:

	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;
	std::mutex mutex;
	std::condition_variable condition;
	bool stop = false;
	size_t maxTaskCount;
};
