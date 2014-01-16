#include <vector>
#include <iostream>
#include <queue>

#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_vector.h"
#include "tbb/parallel_for.h"

class Object
{
public:
	Object(void) = default;
	Object(const Object &) { std::cout << "Copied" << std::endl; }
	Object(Object &&) { std::cout << "Moved" << std::endl; }
};

void TestStdQueue(void)
{
	std::cout << "std::queue<T>" << std::endl;
	std::queue<Object> buffer;
	std::cout << "Push 1" << std::endl;
	buffer.push(Object());				// 1 move
	std::cout << "Push 2" << std::endl;
	buffer.push(Object());				// 1 move
	std::cout << "Pop 1" << std::endl;
	buffer.pop();						// remove object
	std::cout << "Pop 2" << std::endl;
	buffer.pop();						// remove object
	std::cout << std::endl;
}

void TestStdVector(void)
{
	std::cout << "std::vector<T>" << std::endl;
	std::vector<Object> buffer;
	std::cout << "Push back 1" << std::endl;
	buffer.push_back(Object());			// 1 move
	std::cout << "Push back 2" << std::endl;
	buffer.push_back(Object());			// 2 move
	std::cout << "Pop back 2" << std::endl;
	buffer.pop_back();					// remove object
	std::cout << "Pop back 1" << std::endl;
	buffer.pop_back();					// remove object
	std::cout << std::endl;
}

void TestConcurrentQueue(void)
{
	std::cout << "tbb::concurrent_queue<T>" << std::endl;
	tbb::concurrent_queue<Object> buffer;
	std::cout << "Push 1" << std::endl;
	buffer.push(Object());				// 1 copy, not so good as move
	std::cout << "Push 2" << std::endl;
	buffer.push(Object());				// 1 copy, not so good as move
	std::cout << "Pop 1" << std::endl;
	Object out;
	buffer.try_pop(out);
	std::cout << "Pop 2" << std::endl;
	buffer.try_pop(out);
	std::cout << std::endl;
}

void TestConcurrentVector(void)
{
	std::cout << "tbb::concurrent_vector<T>" << std::endl;
	tbb::concurrent_vector<Object> buffer;
	std::cout << "Push back 1" << std::endl;
	buffer.push_back(Object());			// 1 copy, not so good as move
	std::cout << "Push back 2" << std::endl;
	buffer.push_back(Object());			// 1 copy, 1 operation instead of 2
	std::cout << std::endl;
}

int main(void)
{
	TestStdQueue();
	TestConcurrentQueue();
	TestStdVector();
	TestConcurrentVector();
}