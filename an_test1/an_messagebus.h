#pragma once
#include "an_any.h"
#include "an_function_traits.h"
#include "an_nocopyable.h"
#include <unordered_map>
#include <string>
#include <functional>

namespace an {
	class AnMessageBus : NoCopyable {
	public:
		AnMessageBus() = default;
		~AnMessageBus() = default;

		template<typename F>
		void Attach(F&&f, const std::string& topic = "") {
			auto func = an::to_function(std::forward<F>(f));

			Add(topic, std::move(func));
		}

		template<typename R, typename...Args>
		void Remove(const std::string& topic) {
			using function_type = std::function<R(Args...)>;
			std::string key = topic + typeid(function_type).name();

			auto range = m_map.equal_range(key);
			m_map.erase(range.first, range.second);

		}

		template<typename R>
		void SendMsg(const std::string& topic = "") {
			using function_type = std::function<R()>;
			std::string key = topic + typeid(function_type).name();
			auto range = m_map.equal_range(key);
			for (auto it = range.first; it != range.second;++it) {
				auto f = it->second.AnyCast<function_type>();
				f();
			}
		}

		template<typename R, typename...Args>
		void SendMsg(Args&&...args, const std::string& topic = "") {
			using function_type = std::function<R(Args...)>;
			std::string key = topic + typeid(function_type).name();
			auto range = m_map.equal_range(key);
			for (auto it = range.first; it != range.second; ++it) {
				auto f = it->second.AnyCast<function_type>();
				f(std::forward<Args>(args)...);
			}
		}
	private:
		template<typename F>
		void Add(const std::string &topic, F&& f) {
			std::string key = topic + typeid(f).name();

			m_map.emplace(key, std::forward<F>(f));
		}
	private:
		std::unordered_multimap<std::string, Any> m_map;
	};
}
