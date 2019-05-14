#include "Sorter.h"

void Sorter::split(uint64_t* const buf, const int id)
{
	int file = 0;
	while (input.good()) 
	{
		{
			std::unique_lock<std::mutex> lock(streamReadMutex);
			input.read(reinterpret_cast<char*>(buf), memoryPerThread);
			lock.unlock();
		}

		auto readAmount = input.gcount() / sizeof(uint64_t);

		if (readAmount)
		{
			std::sort(buf, buf + readAmount);
			std::string outputFileName = std::to_string(0) + '-' + std::to_string(id) + '-' + std::to_string(file);
			std::ofstream outputStream(outputFileName, std::ios::binary | std::ios::out);
			outputStream.write(reinterpret_cast<char*>(buf), readAmount * sizeof(uint64_t));

			outputFiles.push(outputFileName);

			file++;
		}
	}
}

void Sorter::merge(const std::string & fileName1
				   , const std::string & fileName2
				   , uint64_t * const buffer
				   , const int id
				   , const int iter
				   , const int file)
{
	std::ifstream file1(fileName1, std::ios::binary | std::ios::in);
	std::ifstream file2(fileName2, std::ios::binary | std::ios::in);

	const auto name = std::to_string(iter) + '-' + std::to_string(id) + '-' + std::to_string(file);

	std::ofstream out(name, std::ios::binary | std::ios::out);

	const auto limit = (memoryPerThread / sizeof(uint64_t) / 4), limitM = 2 * limit;

	uint64_t* const bufL = buffer;
	uint64_t* const bufR = (buffer + limit);
	uint64_t* const bufM = (buffer + limitM);

	file1.read(reinterpret_cast<char*>(bufL), static_cast<uint64_t>(limit) * sizeof(uint64_t));
	auto readL = file1.gcount() / sizeof(uint64_t);

	file2.read(reinterpret_cast<char*>(bufR), static_cast<uint64_t>(limit) * sizeof(uint64_t));
	auto readR = file2.gcount() / sizeof(uint64_t);

	auto left = 0, middle = 0, right = 0;

	for (;  file1.good() || file2.good() || left < readL || right < readR; middle++)
	{
		if (left == readL && file1.good()) 
		{
			file1.read(reinterpret_cast<char*>(bufL), static_cast<uint64_t>(limit) * sizeof(uint64_t));
			readL = file1.gcount() / sizeof(uint64_t);
			left = 0;
		}
		if (right == readR && file2.good()) 
		{
			file2.read(reinterpret_cast<char*>(bufR), static_cast<uint64_t>(limit) * sizeof(uint64_t));
			readR = file2.gcount() / sizeof(uint64_t);
			right = 0;
		}
		if (middle == limitM) 
		{
			out.write(reinterpret_cast<char*>(bufM), static_cast<uint64_t>(middle) * sizeof(uint64_t));
			middle = 0;
		}
		if (left < readL && right < readR) 
		{
			if (bufR[right] < bufL[left]) 
			{
				bufM[middle] = bufR[right];
				right++;
			}
			else 
			{
				bufM[middle] = bufL[left];
				left++;
			}
		}
		else if (left == readL && right < readR) 
		{
			bufM[middle] = bufR[right];
			right++;
		}
		else if (right == readR && left < readL) 
		{
			bufM[middle] = bufL[left];
			left++;
		}
	}

	out.write(reinterpret_cast<char*>(bufM), static_cast<uint64_t>(middle) * sizeof(uint64_t));

	std::unique_lock<std::mutex> queueLock(queueMutex);
	outputFiles.push(name);
	queueLock.unlock();
}

void Sorter::SortInThreads(const int id, std::string& res) 
{
	uint64_t* const buf = buffer.data() + id * memoryPerThread / sizeof(uint64_t);

	split(buf, id);

	{
		std::unique_lock<std::mutex> lock(iterationFinishMutex);
		finishedStep++;
		condition.notify_all();
		while (finishedStep < nThreads){
			condition.wait(lock);
		}
		lock.unlock();
	}

	for (auto file = 0; outputFiles.size() > 1; file++) 
	{
		std::unique_lock<std::mutex> queueLock(queueMutex);

		auto tmp1 = outputFiles.front();
		outputFiles.pop();
		auto tmp2 = outputFiles.front();
		outputFiles.pop();

		queueLock.unlock();

		merge(tmp1, tmp2, buf, id, 1, file);

		std::remove(tmp1.c_str());
		std::remove(tmp2.c_str());
	}

	std::unique_lock<std::mutex> fLock(sortDoneMutex);
	finishedSort++;
	if (finishedSort == nThreads) 
	{
		if (outputFiles.empty()) {
			throw std::runtime_error("No output files");
		}
		else {
			res = outputFiles.front();
		}
	}
}
