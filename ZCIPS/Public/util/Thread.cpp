#include "stdafx.h"
#include "thread.h"

Thread::Thread(std::function<void()> in_function, std::atomic<bool>& in_threadRun)
	: d_threadFinished(true), d_fun(in_function), d_threadRun(in_threadRun)
{
	d_threadRun = false;
}

Thread::~Thread()
{
	stopThread();
}

void Thread::realThread()
{
	std::lock_guard<std::mutex> lk(d_mutex);
	d_threadFinished = false;
	d_fun();
	d_threadFinished = true;
	d_con.notify_one();
}

void Thread::detach()
{
	d_threadRun = true;
	std::function<void()> sendThreadFun = std::bind(&Thread::realThread, this);
	d_thread.reset(new std::thread(sendThreadFun));
	d_thread->detach();
}

void Thread::stopThread()
{
	if (d_threadRun)
	{
		d_threadRun = false;
		std::unique_lock<std::mutex> d_lock(d_mutex);
		d_con.wait(d_lock, [this]() { return d_threadFinished == true; });
	}
}

