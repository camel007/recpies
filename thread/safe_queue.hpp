#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <queue>

namespace yao{
template<class T>
class SafeQueue{
	public:
		SafeQueue():mutex_(),q_(){}

		bool empty() const {
			std::lock_guard<std::mutex> lock(mutex_);
			return q_.empty();
		}

		bool size() const {
			std::lock_guard<std::mutex> lock(mutex_);
			return q_.size();
		}

		void enqueue(T& value) {
			std::lock_guard<std::mutex> lock(mutex_);
			q_.push(value);
		}

		void enqueue(T&& value) {
			std::lock_guard<std::mutex> lock(mutex_);
			q_.emplace(std::move(value));
		}

		bool dequeue(T& value){
			std::lock_guard<std::mutex> lock(mutex_);
			if(q_.empty()) {
				return false;
			}

			value = std::move(q_.front()); // T must support move asignment operator
			q_.pop();

			return true;
		}

		SafeQueue(const SafeQueue&) = delete;
		void operator=(const SafeQueue&) = delete;
	private:
		mutable std::mutex mutex_;
		std::queue<T> q_;
};
}
