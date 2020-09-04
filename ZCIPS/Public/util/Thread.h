	#pragma once
#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <condition_variable>

class Thread
{
private:
	std::mutex d_mutex;
	std::condition_variable d_con;
	std::unique_ptr<std::thread> d_thread;
	std::function<void()> d_fun;
	void realThread();
	bool d_threadRun;
	bool d_threadFinished;
public:
	Thread(std::function<void()> in_function, bool& in_threadRun);
	~Thread();
	bool join();
	bool detach();
	void stopThread();
};

