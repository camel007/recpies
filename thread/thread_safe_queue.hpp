#pragma once

#include <queue>
#include <thread>
#include <mutex>

namespace yao{

template<class T>
class BlockingQueue{
	public:
		BlockingQueue():mutex_(),not_empty_(),q_(){}

		T dequeue(){
			std::unique_lock<std::mutex> lock(mutex_);
			while(q_.empty()){
				not_empty_.wait(lock);
			}
			assert(!q_.empty());
			auto first(std::move(q_.front()));
			q_.pop();
			return first;
		}

		void dequeue(T& value) {
			std::unique_lock<std::mutex> lock(mutex_);
			while(q_.empty()){
				not_empty_.wait(lock);
			}
			assert(!q_.empty());
			value = std::move(q_.front());
			q_.pop();
		}

		size_t size() const {
			std::lock_guard<std::mutex> lock(mutex_);
			return q_.size();
		}

		bool empty() const {
			std::lock_guard<std::mutex> lock(mutex_);
			return q_.empty();
		}

		void enqueue(T& value){
			std::lock_guard<std::mutex> lock(mutex_);
			q_.push(value);
			not_empty_.notify_all();
		}


		BlockingQueue(const BlockingQueue &) = delete;
		void operator=(const BlockingQueue &) = delete;
	private:
		mutable std::mutex mutex_;
		std::condition_variable not_empty_;
		std::queue<T> q_;

};
}
