#pragma once

#include "safe_queue.hpp"
#include <thread>
#include <future>
#include <mutex>

namespace yao{
class ThreadPool{
public:
	ThreadPool(size_t size):shutdown_(false),mutex_(),cv_(),tasks_(){
		for(size_t i = 0; i < size; ++i){
			threads_.push_back(std::thread([this](){
				for(;;){
					std::unique_lock<std::mutex> lock(mutex_);	
					cv_.wait(lock, [this](){return (!this->tasks_.empty())||(this->shutdown_);});

					std::function<void()> task;
					auto is = tasks_.dequeue(task);
					if(is) {
						task();
					} else {
						break;
					}
				}
			}));
		}
	}

	~ThreadPool() {
		shutdown_ = true;
		cv_.notify_all();
		for(auto &thread : threads_){
			thread.join();
		}
	}

	template<typename F, typename...Args>
	auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
		std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		auto task = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
		
		tasks_.enqueue([task](){(*task)();});
		cv_.notify_all();

		return task->get_future();
	}	

	ThreadPool(const ThreadPool&) = delete;
	void operator=(const ThreadPool&) = delete;
private:
	std::vector<std::thread> threads_;
	SafeQueue<std::function<void()>> tasks_;
	std::condition_variable cv_;
	std::mutex mutex_;
	bool shutdown_;
};
}
