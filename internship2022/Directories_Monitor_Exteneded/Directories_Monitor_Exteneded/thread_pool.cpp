#include "thread_pool.h"
void TP::Start() {
	const uint32_t num_threads = std::thread::hardware_concurrency(); //threads on system
	threads.resize(num_threads);
	for (uint32_t i = 0; i < num_threads; i++) {
		threads[i] = std::thread(&TP::ThreadLoop, this);
	}
}
void TP::ThreadLoop() {
	while (1) {
		task job; 
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			mutex_condition.wait(lock, [this] {
				return !jobs.empty() || should_terminate;
				});
			if (should_terminate) {
				return;
			}
			job = jobs.front();
			jobs.pop();
		}
		job.pfn(job.params);
	}
}

void TP::add_task(const task& job) {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		jobs.push(job);
	}
	mutex_condition.notify_one();
}

bool TP::Busy() {
	bool poolbusy;
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		poolbusy = !jobs.empty();
	}
	return poolbusy;
}

void TP::Stop() {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		should_terminate = true;
	}
	mutex_condition.notify_all();
	for (std::thread& active_thread : threads) {
		active_thread.join();
	}
	threads.clear();
}