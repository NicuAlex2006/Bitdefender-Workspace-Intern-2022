#pragma once
#include <functional>
#include <mutex>
#include <queue>
#include <vector>
#include <condition_variable>
#include <thread>

struct task {
	using function_type = void(*) (void*);
	void* params = nullptr;
	function_type pfn = nullptr;
};
class TP {
public:
	void Start();
	void add_task(const task& job);
	void Stop();
	bool Busy();
private:
	void ThreadLoop();
	bool should_terminate = false;
	std::mutex queue_mutex;
	std::condition_variable mutex_condition;
	std::vector<std::thread> threads;
	std::queue<task> jobs;
};