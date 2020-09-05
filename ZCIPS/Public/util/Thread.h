#pragma once
#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

class Thread
{
private:
	std::mutex d_mutex;
	std::condition_variable d_con;
	std::unique_ptr<std::thread> d_thread;
	std::function<void()> d_fun;
	std::atomic<bool>& d_threadRun;
	bool d_threadFinished;
	void realThread();
public:
	Thread(std::function<void()> in_function, std::atomic<bool>& in_threadRun);
	~Thread();
	void stopThread();
	void detach();
};

