#pragma once
#include <functional>
#include <thread>
#include <memory>
#include <future>
#include <vector>
#include "an_nocopyable.h"

namespace an {
	template<typename R>
	class AnTask;

	template<typename R, typename...Args>
	class AnTask<R(Args...)> {
	public:
		using return_type = R;
		//typedef R return_type;

	private:
		std::function<R(Args...)> m_fn;
	
	public:
		AnTask(std::function<R(Args...)>&& f) :m_fn(std::move(f)) {}
		AnTask(std::function<R(Args...)>& f) :m_fn(f) {}
		
		~AnTask() {

		}

		void Wait() {
			std::async(m_fn).wait();
		}

		template<typename... Args>
		R Get(Args&&... args) {
			return std::async(m_fn, std::forward<Args>(args)...).get();
		}

		std::shared_future<R> Run() {
			return std::async(m_fn);
		}

		//支持链式调用
		template<typename F>
		auto Then(F&& f)->AnTask<typename std::result_of<F(R)>::type(Args...)> {
			using ReturnType = typename std::result_of<F(R)>::type;
			//typedef typename std::result_of<F(R)>::type ReturnType;

			auto func = std::move(m_fn);
			return AnTask<ReturnType(Args...)>([func, &f](Args&&...args) {
				std::future<R> lastf = std::async(func, std::forward<Args>(args)...);
				return std::async(f, lastf.get()).get();
			});
		}
	};


	class AnTaskGroup : NoCopyable {
	private:
		std::vector<std::shared_future<void>> m_voidGroup;
	public:
		AnTaskGroup() {

		}

		~AnTaskGroup() {

		}

		void Run(AnTask<void(void)>&& task) {
			m_voidGroup.emplace_back(task.Run());
		}

		template<typename F>
		void Run(F&& f)
		{
			Run(typename AnTask<std::result_of<F()>::type()>(std::forward<F>(f)));
		}

		template<typename F, typename... Funs>
		void Run(F&& first, Funs&&... rest) {
			Run(std::forward<F>(first));
			Run(std::forward<Funs>(rest)...);
		}

		void Wait() {
			for (auto&& it : m_voidGroup) {
				it.get();
			}
		}
	};
}