#include "stdafx.h"
#include "thread.h"

Thread::Thread(std::function<void()> _function, std::atomic<bool>& _threadRun)
	: d_threadFinished(true), d_fun(_function), d_threadRun(_threadRun)
{
	d_threadRun = false;
}

Thread::~Thread()
{
	stopThread();
}

void Thread::realThread()
{
	std::lock_guard<std::mutex> lock(d_mutex);
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
	if (!d_threadFinished)
	{
		d_threadRun = false;
		std::unique_lock<std::mutex> lock(d_mutex);
		d_con.wait(lock, [this]() { return d_threadFinished == true; });
	}
}

