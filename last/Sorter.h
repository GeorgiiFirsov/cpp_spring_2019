#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <string>
#include <exception>
#include <vector>


class Sorter final
{
public:
	Sorter(size_t memLimit, const std::string& infile)
		: input(std::ifstream(infile, std::ios::binary))
		, memory(memLimit)
		, memoryPerThread(memory / nThreads)
		, buffer(std::vector<uint64_t>(memory / sizeof(uint64_t)))
	{
		if (!input.good()) {
			throw std::runtime_error("Unable to open file");
		}
	}

	~Sorter() {}

	void Sort(std::string& resultStr) 
	{
		std::vector<std::thread> threads;
		for (int i = 0; i < nThreads; ++i) {
			threads.emplace_back(&Sorter::SortInThreads, this, i, std::ref(resultStr));
		}
		for (auto& thread : threads) {
			thread.join();
		}
	}

private:

	/* In other places two threads are hardcoded! Sorry :(*/
	static constexpr auto nThreads = 2;

	std::ifstream input;

	size_t memory;
	size_t memoryPerThread;

	std::vector<uint64_t> buffer;
	std::queue<std::string> outputFiles;

	std::atomic<int> finishedStep = 0;
	std::atomic<int> finishedSort = 0;
	std::condition_variable condition;
	std::mutex sortDoneMutex;
	std::mutex queueMutex;
	std::mutex streamReadMutex;
	std::mutex iterationFinishMutex;

	void SortInThreads(const int id, std::string& res); 
	void split(uint64_t* const buffer, const int id);

	void merge(const std::string& fileName1
			   , const std::string& fileName2
			   , uint64_t* const buffer
			   , const int id
			   , const int iter
			   , const int file);

};
