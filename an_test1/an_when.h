#pragma once

#include "an_task.h"
#include <vector>
#include <utility>
#include <chrono>

namespace an {
	template<typename Range>
	AnTask<std::vector<typename Range::value_type::return_type>() > WhenAll(Range& range) {
		typedef typename Range::value_type::return_type ReturnType;
		auto task = [&range]() {
			std::vector<std::shared_future<ReturnType>> fv;
			for (auto& task : range) {
				fv.emplace_back(task.Run());
			}

			std::vector<ReturnType> v;
			for (auto& item : fv) {
				v.emplace_back(item.get());
			}

			return v;
		};

		return task;
	}

	template<typename Range>
	AnTask<std::pair<int, typename Range::value_type::return_type>()>WhenAny(Range& range) {
		auto task = [&range]() {
			using namespace detail;
			return GetAnyResultPair(TransForm(range));
		};
		return task;
	}

	namespace detail {
		template<typename R>
		struct RangeTrait {
			using Type = R;
		};

		template<typename R>
		struct RangeTrait<std::shared_future<R>> {
			using Type = R;
		};

		template<typename Range>
		std::vector<std::shared_future<typename Range::value_type::return_type>>TransForm(Range& range) {
			typedef typename Range::value_type::return_type ReturnType;
			std::vector < std::shared_future<ReturnType>> fv;
			for (auto& task : range) {
				fv.emplace_back(task.Run());
			}
			return fv;
		}

		template<typename Range>
		std::pair<int, typename RangeTrait<typename Range::value_type>::Type>GetAnyResultPair(Range& fv) {
			size_t size = fv.size();
			while (1) {
				for (size_t i = 0; i < size; ++i) {
					if (fv[i].wait_for(std::chrono::milliseconds(1)) == std::future_status::ready) {
						return std::make_pair(i, fv[i].get());
					}
				}
			}
		}
	}
}