#include "thread_pool.hpp"
#include <iostream>

using namespace yao;

int worker(int a) {
	return a*a;
}

void worker1(std::string s){
	std::cout << s << std::endl;
}

int main() {
	ThreadPool pool(2);

	int a = 11;
	auto ret1 = pool.submit(worker, std::move(a));

	auto ret2 = pool.submit(worker1, "hello world!");


	std::cout << ret1.get() << std::endl;

	return 0;
}
