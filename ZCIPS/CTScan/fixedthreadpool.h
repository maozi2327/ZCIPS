#pragma once
#include <functional>
#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>
#include <memory>
#include <queue>

class FixedThreadPool
{
private:
	struct data
	{
		std::mutex mtx_;
		std::condition_variable con_;
		bool is_ShutDown_ = false;
		std::queue<std::function<void()>> task_;
	};
	std::shared_ptr<data> data_;
public:
	explicit FixedThreadPool(size_t thread_count) : data_(std::make_shared<data>())
	{
		for (size_t i = 0; i < thread_count; ++i)
		{
			std::thread([data = data_]
				{
					std::unique_lock<std::mutex> lock(data->mtx_);

					for(;;)
					{
						if (!data->task_.empty())
						{
							auto current = std::move(data->task_.front());
							data->task_.pop();
							lock.unlock();
							current();
							lock.lock();
						}
						else if (data->is_ShutDown_)
							break;
						else
							data->con_.wait(lock);
					}
				}
			).detach();
		}
	}
	~FixedThreadPool()
	{
		if((bool)data_)
		{
			std::lock_guard<std::mutex> lock(data_->mtx_);
			data_->is_ShutDown_ = true;
		}
		data_->con_.notify_all();
	}
	
	FixedThreadPool() = default;
	FixedThreadPool(const FixedThreadPool&) = default;

	template<typename T>
	bool excute(T&& _fun)
	{
		{
			std::lock_guard<std::mutex> lock(data_->mtx_);
			data_->task_.emplace(std::forward<T>(_fun));
		}
		data_->con.notify_one();
	}
}