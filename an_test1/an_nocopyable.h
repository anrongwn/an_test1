#pragma once

namespace an{
	struct NoCopyable {
	protected:
		NoCopyable() = default;
		~NoCopyable() = default;

		NoCopyable(const NoCopyable&) = delete;
		NoCopyable& operator=(const NoCopyable&) = delete;
	};

}