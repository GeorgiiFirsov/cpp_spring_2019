/*
 *	Usage:
 *		Sorter sorter
 *		sorter(input, output)
 *
 */

#include <functional>
#include <fstream>
#include <string>
#include <vector>

#include <queue>
#include <stack>
#include <condition_variable>
#include <thread>
#include <mutex>

class Sorter final
{
public:
	Sorter(size_t working_memory_size = 4 * 1024 * 1024
	       , size_t nThreads = std::thread::hardware_concurrency())	
		: parts_count_(nThreads)
		, part_size_(working_memory_size / (nThreads + 1) / sizeof(uint64_t))
		, total_parts_(0)
	{
		parts_ = split(parts_count_, part_size_);
	}

	Sorter(const Sorter&) = delete;
	Sorter& operator=(const Sorter&) = delete;

	void operator()(const std::string& infname, const std::string& outfname)
	{
		/* preparing */
		total_parts_ = 0;
		while (!free_parts_.empty()) {
			free_parts_.pop();
		}
		for (size_t i = 0, size = parts_count_; i < size; ++i) {
			free_parts_.push(i);
		}
		file_buf_.resize(part_size_);

		/* opening source */
		std::ifstream source(infname, std::ios::binary);
		if (!source) {
			throw std::runtime_error("can't open '" + infname + "'");
		}

		/* reading */
		while (!source.eof())
		{
			if (!source.read(reinterpret_cast<char*>(file_buf_.data())
					 , file_buf_.size() * sizeof(uint64_t)))
			{
				if (!source.eof()) {
					throw std::runtime_error("can't read '" + infname + "'");
				}
			}

			size_t bytes_read = static_cast<size_t>(source.gcount());
			file_buf_.resize(bytes_read / sizeof(uint64_t));

			std::unique_lock<std::mutex> lock(mutex_);
			while (free_parts_.empty()) {
				part_processed_.wait(lock);
			}

			const size_t part_index = free_parts_.top();
			free_parts_.pop();

			parts_[part_index].swap(file_buf_);
			file_buf_.resize(part_size_);

			/* starting thread */
			std::thread thread(std::bind(&Sorter::sortAndSave
						     , this, getSortedFname(total_parts_)
						     , part_index));
			thread.detach();

			total_parts_++;
		}

		/* waiting completeness*/
		std::unique_lock<std::mutex> lock(mutex_);
		while (free_parts_.size() < parts_count_)
			part_processed_.wait(lock);

		std::ofstream dest_(outfname, std::ios::binary);
		if (!dest_) {
			throw std::runtime_error("can't open '" + outfname + "'");
		}

		mergeAndSave(dest_);

		/* erase temp files */
		for (int i = 0; i < total_parts_; ++i) {
			std::remove(getSortedFname(i).c_str());
		}
	}

private:

	void mergeAndSave(std::ofstream& dest_) const
	{
		std::vector<std::ifstream> sortedFiles;
		for (int i = 0; i < total_parts_; ++i)
		{
			sortedFiles.emplace_back(getSortedFname(i), std::ios::binary);
			if (!sortedFiles.back()) {
				throw std::runtime_error("can't open temporary file");
			}
		}

		auto gt = [](const std::pair<uint64_t, size_t> & x, const std::pair<uint64_t, size_t> & y) {
			return x.first > y.first;
		};

		std::priority_queue<std::pair<uint64_t, size_t>
				    , std::vector<std::pair<uint64_t, size_t>>
				    , decltype(gt)> fqueue(gt);

		for (size_t i = 0, size = sortedFiles.size(); i < size; ++i)
		{
			uint64_t value = getNum(sortedFiles[i]);
			fqueue.push(std::make_pair(value, i));
		}

		while (!fqueue.empty())
		{
			const auto minFilePair = fqueue.top();
			fqueue.pop();

			const auto min = minFilePair.first;
			const auto file_index = minFilePair.second;
			auto& file = sortedFiles[file_index];

			while (true)
			{
				flushNum(dest_, min);
				uint64_t value = getNum(file);
				if (file.eof()) {
					break;
				}
				if (value != min)
				{
					fqueue.push(std::make_pair(value, file_index));
					break;
				}
			}
		}
	}

	uint64_t getNum(std::ifstream& file) const
	{
		uint64_t res;
		if (!file.read(reinterpret_cast<char*>(&res), sizeof(uint64_t)))
		{
			if (!file.eof()) {
				throw std::runtime_error("Unable to read temporary file");
			}
		}
		return res;
	}

	void flushNum(std::ofstream& file, uint64_t value) const
	{
		if (!file.write(reinterpret_cast<const char*>(&value), sizeof(uint64_t))) {
			throw std::runtime_error("unable to write target file");
		}
	}

	std::vector<std::vector<uint64_t>> split(size_t count, size_t size) const
	{
		std::vector<std::vector<uint64_t>> parts;

		for (size_t i = 0; i < count; ++i)
		{
			std::vector<uint64_t> new_part;
			new_part.resize(size);
			parts.push_back(std::move(new_part));
		}

		return parts;
	}

	std::string getSortedFname(int n) const
	{
		return "sorted_" + std::to_string(n) + ".tmp";
	}

	void sortAndSave(const std::string & file_name, size_t part_index)
	{
		auto& part = parts_[part_index];
		std::sort(std::begin(part), std::end(part));
		std::ofstream sorted(file_name, std::ios::binary);
		std::unique_lock<std::mutex> lock(mutex_);
		free_parts_.push(part_index);
		part_processed_.notify_one();
	};

	const size_t parts_count_;
	const size_t part_size_;
	int total_parts_;

	std::vector<std::vector<uint64_t>> parts_;
	std::stack<size_t> free_parts_;

	std::vector<uint64_t> file_buf_;

	std::mutex mutex_;
	std::condition_variable part_processed_;
};
