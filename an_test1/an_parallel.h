#pragma once
#include <thread>
#include <future>
#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>

namespace an {
	template<class Iterator, class Function>
	void ParallelForeach(Iterator& begin, Iterator& end, Function& func) {
		auto partNum = std::thread::hardware_concurrency();
		auto blockSize = std::distance(begin, end) / partNum;
		Iterator last = begin;
		if (blockSize > 0) {
			std::advance(last, (partNum - 1)*blockSize);
		}
		else {
			last = end;
			blockSize = 1;
		}

		std::vector<std::future<void>> futures;
		//前面N-1个区段
		for (; begin != last; std::advance(begin, blockSize)) {
			futures.emplace_back(std::async([begin, blockSize, &func]() {
				std::for_each(begin, begin + blockSize, func);
			}));
		}

		//最后一个区段
		futures.emplace_back(std::async([&begin, &end, &func] {
			std::for_each(begin, end, func);
		}));

		//等结果
		std::for_each(futures.begin(), futures.end(), [](std::future<void>&f) {
			f.get();
		});
	}
}