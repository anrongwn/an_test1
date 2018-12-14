// an_test1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include "an_aop1.h"
#include "an_any.h"
#include "an_function_traits.h"
#include "an_messagebus.h"
#include <ppl.h>
#include <thread>
#include "an_task.h"
#include <chrono>
#include <vector>
#include "an_when.h"
#include <numeric>
#include "an_parallel.h"
using namespace std;


class IHello :public an::NoCopyable {
public:
	IHello() {

	}
	virtual ~IHello() {

	}

	virtual void Output(const string &str) {

	}
};

class Hello : public IHello {
public:
	void Output(const string &str) {
		cout << str << endl;
	}
};

class HelloProxy : public IHello {
public:
	explicit HelloProxy(IHello *p) : m_ptr(p) {

	}
	~HelloProxy() {
		delete m_ptr;
		m_ptr = nullptr;
	}

	void Output(const string &str) final{
		cout << "Before real output" << endl;
		m_ptr->Output(str);
		cout << "After real output" << endl;
	}
private:
	IHello * m_ptr;
};

//message bus test
an::AnMessageBus g_bus;

static std::string g_topic{ "drive" };

struct subject {
	void send(const std::string& topic) {
		g_bus.SendMsg<void, int>(120, topic);
	}
};
struct car {
	car() {
		g_bus.Attach([this](int speed) {drive(speed); }, g_topic);
	}
	void drive(int speed) {
		std::cout << "car::drive(" << speed << ")" << std::endl;
	}
};
struct bus {
	bus() {
		g_bus.Attach([this](int speed) {drive(speed); }, g_topic);
	}
	void drive(int speed) {
		std::cout << "bus::drive(" << speed << ")" << std::endl;
	}
};


int main()
{
	std::shared_ptr<IHello> hello = std::make_shared<HelloProxy>(new Hello());
	hello->Output(R"(c:\windows\system32)");

	std::shared_ptr<IHello> hello2 = std::make_shared<Hello>();
	hello2->Output(R"(wdddd)");

	auto f = std::bind(&HT, std::placeholders::_1);
	Invoke<AA, BB>(std::function<void(int)>(std::bind(&HT, std::placeholders::_1)), 1);
	Invoke<AA, BB>(f, 1);

	Invoke<CC>(f, 2);

	using namespace an;

	Any n;
	auto r = n.IsNull();
	std::string s1 = "hello";
	n = s1;
	//n.AnyCast<int>();

	auto ff = to_function([](int i) {return i; });
	std::function<int(int)> ff1 = [](int i) {return i; };
	if (std::is_same<decltype(ff), decltype(ff1)>::value) std::cout << "same" << std::endl;

	//messagebus test
	subject sub;
	car c1;
	bus c2;

	sub.send(g_topic);
	std::cout << "----------------------" << std::endl;
	sub.send("");

	/*
	using namespace Concurrency;
	parallel_for(1, 6, [](int value) {
		std::stringstream ss;
		ss << value << '- ' << std::this_thread::get_id() << ', '<<std::endl;
		std::cout << ss.str();
	});
	*/

	//task test
	AnTask<int()> t([] {return 32; });

	auto r1 = t.Then([](int result) {
		std::cout << "result=" << result << std::endl;
		return (result + 3);
	}).Then([](int result) {
		std::cout << "result=" << result << std::endl;
		return (result + 6);
	}).Get();

	std::cout << "AnTask t'result=" << r1 << std::endl;
	
	AnTask<std::string(std::string)> t2([](string str){
		return str; });
	auto r2 = t2.Then([](const std::string& str) {
		std::cout << str << endl;
		return str + " ok";
	}).Get("wangjr");
	std::cout << "AnTask t2'result=" << r2 << std::endl;

	//AnTaskGroup test
	std::function<void()>fff = []() {std::cout << "ok0" << std::endl; };
	auto fff1 = []() {std::cout << "ok1" << std::endl; };
	AnTaskGroup task_g;
	task_g.Run(fff);
	task_g.Run(fff, fff1, []() {std::cout << "ok2" << std::endl; });
	task_g.Wait();

	//whenall
	auto PrintThread = []() {
		std::cout << "tid=" << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	};
	std::vector<AnTask<int()>> v = { AnTask<int()>([=] {PrintThread(); std::chrono::seconds(1); return 1; }),\
		AnTask<int()>([=] {PrintThread(); return 2; }),\
		AnTask<int()>([=] {PrintThread(); return 3; }),\
		AnTask<int()>([=] {PrintThread(); return 4; })
	};
	std::cout << "when all test" << std::endl;
	WhenAll(v).Then([](std::vector<int>results) {
		std::cout << "the sum is " << std::accumulate(std::begin(results), std::end(results), 0) << std::endl;
	}).Wait();

	std::cout << "when any test" << std::endl;
	WhenAny(v).Then([](std::pair<int, int>&result) {
		std::cout << "index=" << result.first << ", result=" << result.second << std::endl;
		return result.second;
	}).Then([](int result) {
		std::cout << "any result=" << result << std::endl;
	}).Get();

	//parallel test
	auto check_prime=[](int x)->bool {
		for (int i = 2; i < 2; ++i) {
			if (x%i == 0)
				return false;
			else
				return true;
		}
	};
	std::vector<int> v3;
	for (int i = 0; i < 100000; ++i) {
		v3.emplace_back(i + 1);
	}
	ParallelForeach(v3.begin(), v3.end(), check_prime);

	system("pause");
    return 0;
}