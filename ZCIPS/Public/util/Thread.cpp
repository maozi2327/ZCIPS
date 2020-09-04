#include "stdafx.h"
#include "thread.h"

Thread::Thread(std::function<void()>  in_function, bool& in_threadRun)
{
	d_threadFinished = false;
	d_threadRun = in_threadRun;
	d_fun = in_function;
}

Thread::~Thread()
{
	stopThread();
}
bool Thread::join()
{
	std::lock_guard<std::mutex> lk(d_mutex);
	std::function<void()> sendThreadFun = std::bind(&Thread::realThread, this);
	d_thread.reset(new std::thread(sendThreadFun));
	d_thread->join();
	return true;
}

bool Thread::detach()
{
	std::lock_guard<std::mutex> lk(d_mutex);
	std::function<void()> sendThreadFun = std::bind(&Thread::realThread, this);
	d_thread.reset(new std::thread(sendThreadFun));
	d_thread->detach();
	return true;
}

void Thread::realThread()
{
	d_fun();
	d_threadFinished = true;
	d_con.notify_one();
}

void Thread::stopThread()
{
	std::unique_lock<std::mutex> d_lock(d_mutex);
	d_con.wait(d_lock, []() {; });
	d_threadRun = false;
}

