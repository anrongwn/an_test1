// an_test1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <memory>
#include <string>
#include <iostream>
#include <functional>
#include "an_aop1.h"
#include "an_any.h"
#include "an_function_traits.h"
#include "an_messagebus.h"

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

	system("pause");
    return 0;
}

