#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <stdexcept>
#include <string>

class SecureCounter {
public:
	SecureCounter() : count(0) {}
	void increment() {
		std::lock_guard<std::mutex> lock(mtx);
		count++;
	}
	
	int getCount() const {
		std::lock_guard<std::mutex> lock(mtx);
		return count;
	}

private:
	mutable std::mutex mtx;
	int count;
	const int max_count = 100; // Example maximum limit
};

void threadFunction(SecureCounter& counter, int increments) {
	try {
		for (int i = 0; i < increments; ++i) {
			counter.increment();
		}
	}
	catch (const std::overflow_error& e) {
		std::cerr << "Error in thread: " << e.what() << std::endl;
	}
}

int main(int argc, char* argv[]) {
	SecureCounter counter;
	const int num_threads = 5;
	const int increments_per_thread = 30;
	std::vector<std::jthread> threads;

	try {
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(threadFunction, std::ref(counter), increments_per_thread);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception occurred while creating threads: " << e.what() << std::endl;
	}

	// Wait for all threads to finish
	for (auto& thread : threads) {
		// No need to join, std::jthread handles it automatically
		std::cout << "Joining thread..." << &thread << std::endl;
		thread.join();
	}

	std::cout << "Final counter value: " << counter.getCount() << std::endl;
	return 0;
}