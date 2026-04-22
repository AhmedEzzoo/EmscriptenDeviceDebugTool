#pragma once

#include <chrono>
#include <functional>
#include <future>
#include <thread>
using namespace std::chrono_literals;

namespace Ezzoo {

	
	class AsyncTimer
	{
	public:

		AsyncTimer() = default;

		template<typename T, class _Rep, class _Period>
		std::future<T> TimerAsyncCall(std::chrono::duration<_Rep, _Period> duration, std::function<void()> callback)
		{
			return std::async(std::launch::async, [duration, callback](){

					std::this_thread::sleep_for(duration);
					callback();
					return false;
				});
		}

	private :

		//std::function <void()> m_Func;

		

	};



}