#pragma once
#include "stdafx.h"
#include <memory>
#include <string>
#include <iostream>
#include <type_traits>
#include <functional>
#include "an_nocopyable.h"


#define HAS_MEMBER(member)\
template<typename T, typename... Args> struct has_member_##member{\
private:\
	template<typename U> \
	static auto Check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...),\
	std::true_type());\
	\
	template<typename U> \
	static std::false_type Check(...);\
public:\
	enum{value=std::is_same<decltype(Check<T>(0)), std::true_type>::value};\
};\

HAS_MEMBER(Foo)
HAS_MEMBER(Before)
HAS_MEMBER(After)

template<typename Func, typename...Args>
struct Aspect : an::NoCopyable {
	Aspect(Func&& f) : m_func(std::forward<Func>(f)){

	}

	template<typename T>
	typename std::enable_if<has_member_Before<T, Args...>::value&&has_member_After<T, Args...>\
		::value>::type Invoke(Args&&... args, T&& aspect) {
		aspect.Before(std::forward<Args>(args)...);

		this->m_func(std::forward<Args>(args)...);

	
		aspect.After(std::forward<Args>(args)...);
	}
	template<typename T>
	typename std::enable_if<has_member_Before<T, Args...>::value&&!has_member_After<T, Args...>\
		::value>::type Invoke(Args&&... args, T&& aspect) {
		aspect.Before(std::forward<Args>(args)...);

		this->m_func(std::forward<Args>(args)...);
	}

	template<typename T>
	typename std::enable_if<!has_member_Before<T, Args...>::value&&has_member_After<T, Args...>\
		::value>::type Invoke(Args&&... args, T&& aspect) {
		
		this->m_func(std::forward<Args>(args)...);


		aspect.After(std::forward<Args>(args)...);
	}

	template<typename Head, typename... Tail>
	void Invoke(Args&&... args, Head&& headAspect, Tail&&... tailAspect) {
		headAspect.Before(std::forward<Args>(args)...);

		Invoke(std::forward<Args>(args)..., std::forward<Tail>(tailAspect)...);
		headAspect.After(std::forward<Args>(args)...);
	}
private:
	Func m_func;
};

template<typename T>
using identity_t = T;


//aop helper
template<typename... AP, typename... Args, typename Func>
void Invoke(Func&&f, Args&&... args) {
	Aspect<Func, Args...> asp(std::forward<Func>(f));
	asp.Invoke(std::forward<Args>(args)..., identity_t<AP>()...);
}

struct AA {
	void Before(int i) {
		std::cout << "Before from AA" << i << std::endl;
	}

	void After(int i) {
		std::cout << "After from AA" << i << std::endl;
	}
};

struct BB {
	void Before(int i) {
		std::cout << "Before from BB" << i << std::endl;
	}

	void After(int i) {
		std::cout << "After from BB" << i << std::endl;
	}
};

struct CC {
	void Before(int i) {
		std::cout << "Before from CC" << i << std::endl;
	}

	void After(int i) {
		std::cout << "After from CC" << i << std::endl;
	}
};

struct DD {
	void Before(int i) {
		std::cout << "Before from DD" << i << std::endl;
	}

	void After(int i) {
		std::cout << "After from DD" << i << std::endl;
	}
};

void GT()
{
	std::cout << "real GT function" << std::endl;
}

void HT(int a) {
	std::cout << "real HT function" << a << std::endl;
}

