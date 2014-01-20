#include <vector>
#include <iostream>
#include <thread>

#include "tbb/concurrent_queue.h"
#include "tbb/parallel_for.h"

bool stop = false;

class MyTask
{
public:
	MyTask(int id, int delay, tbb::concurrent_queue<int> &buffer) : id_(id), delay_(delay), buffer_(buffer) {}

	void operator()()
	{
		++this->count_;

		// Delay.
		for (auto count_1 = 0; count_1 != delay; ++count_1)
		{
			if (!::stop)
			for (int n = 0, limit = std::numeric_limits<int>::max(), dummy; n != limit; ++n)
				dummy = n;
			else
			{
				std::cout << "Stopping " << this->id << std::endl;
				continue;
			}				
		}
		if (!::stop)
		{
			this->buffer_.push(this->id);
			std::cout << "Job finished: " << this->id << ": " << this->count << std::endl;
		}
		else
		{
			std::cout << "Job interrupted: " << this->id << std::endl;
		}
	}

	int &id = this->id_;
	int &delay = this->delay_;
	unsigned int &count = this->count_;

protected:
	tbb::concurrent_queue<int> &buffer_;
	int id_;
	int delay_;
	unsigned int count_ = 0;
};

// Using blocked_range is confusing.
class ExecutionRange
{
public:
	ExecutionRange(std::vector<MyTask> &tasks) : tasks_(tasks) {}

	void operator()(const tbb::blocked_range<::size_t> &r) const
	{
		// NOTE: Do NOT run anything before or after this for loop.
		// This part is already parallelized even though this is before the for loop.
		for (auto it = r.begin(); it != r.end(); ++it)
		{
			std::cout << "Starting task " << this->tasks_[it].id << std::endl;
			this->tasks_[it]();
			std::cout << "Completed task " << this->tasks_[it].id << std::endl;
		}
		// NOTE: Do NOT run anything before or after this for loop.
	}

protected:
	std::vector<MyTask> &tasks_;
};

class Execution
{
public:
	Execution(std::vector<MyTask> &tasks) : tasks_(tasks) {}

	// More straightforward than using blocked_range.
	void operator()(std::size_t i) const
	{
		std::cout << "Executing task " << this->tasks_[i].id << " began." << std::endl;
		this->tasks_[i]();
		std::cout << "Executing task " << this->tasks_[i].id << " ended." << std::endl;
	}

protected:
	std::vector<MyTask> &tasks_;
};

void StopAll(void)
{	
	char ch;
	std::cin >> ch;
	::stop = true;
}

int main(void)
{
	tbb::concurrent_queue<int> sharedBuffer;

	// Set multiple tasks with different amount of load.
	std::vector<MyTask> tasks;
	tasks.push_back(MyTask(1, 3, sharedBuffer));
	tasks.push_back(MyTask(2, 4, sharedBuffer));
	tasks.push_back(MyTask(3, 2, sharedBuffer));
	tasks.push_back(MyTask(4, 1, sharedBuffer));

	// Run tasks in parallel with range.
	//std::cout << "Starting tasks." << std::endl;
	//tbb::parallel_for(tbb::blocked_range<::size_t>(0, tasks.size()), ExecutionRange(tasks));
	//std::cout << "Completed all tasks." << std::endl;

	// Run tasks in parallel with index.
	std::cout << "Starting tasks." << std::endl;
	std::thread t1(StopAll);
	tbb::parallel_for(static_cast<std::size_t>(0), tasks.size(), Execution(tasks));
	std::cout << "Completed all tasks." << std::endl;
	t1.join();
}