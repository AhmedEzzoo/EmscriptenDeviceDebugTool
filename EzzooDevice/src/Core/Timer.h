#pragma once

#include <chrono>



namespace Ezzoo {

	class Timer 
	{

		public:

			Timer()
			{

				Start();
			}
			~Timer() = default;

			void Start()
			{
				m_Start = std::chrono::high_resolution_clock::now();
			}



		float ElapsedMillis()
		{
			return  std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001 * 0.001 * 0.001 * 1000;
		}


		private :
			std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;

	};

}