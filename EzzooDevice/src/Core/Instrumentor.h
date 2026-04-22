#pragma once

#include <chrono>
#include <algorithm>
#include <fstream>

namespace Ezzoo 
{

	struct ProfilResult
	{

		std::string Name;

		long long Start, End;
	};

	class Instrumentor
	{

	public :
		Instrumentor() : m_ProfileCount(0) {}

		inline static Instrumentor& Get()
		{
			static Instrumentor instrumentor;
			return instrumentor;
		}

		inline void BeginSession(const std::string& filepath = "EzzooProfile.json")
		{
			m_OutputStream.open(filepath);
			m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
			m_OutputStream.flush();
		}

		inline void EndSession()
		{
			m_OutputStream.close();
			m_OutputStream << "]}";
			m_OutputStream.flush();

			m_ProfileCount = 0;
		}

		inline void WriteProfile(const ProfilResult& result)
		{
			if (m_ProfileCount++ > 0)
				m_OutputStream << ",";

			std::string name = result.Name;
			std::replace(name.begin(), name.end(), '"', '\'');

			m_OutputStream << "{";
			m_OutputStream << "\"cat\":\"function\",";
			m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
			m_OutputStream << "\"name\":\"" << name << "\",";
			m_OutputStream << "\"ph\":\"X\",";
			m_OutputStream << "\"pid\":0,";
			m_OutputStream << "\"ts\":" << result.Start;
			m_OutputStream << "}";

			m_OutputStream.flush();
		}

	private :
		std::ofstream m_OutputStream;
		int m_ProfileCount;

	};

	class InstrumentTimer
	{
	public :
		InstrumentTimer(const char* name) : m_Name(name)
		{
			m_StartTimePoint = std::chrono::steady_clock::now();
		}

		~InstrumentTimer()
		{
			if (!m_Stop)
			{
				Stop();
			}
		}

		inline void Stop()
		{
			auto endTimePoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTimePoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTimePoint).time_since_epoch().count();

			Instrumentor::Get().WriteProfile({ m_Name, start, end });

			m_Stop = true;
		}

	private :
		
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
		const char* m_Name;
		bool m_Stop{ false };

	};
}

#ifdef EZZOO_PROFILE_ENABLE
	#define EZZOO_BEGINE_SESSION()  ::Ezzoo::Instrumentor::Get().BeginSession()
	#define EZZOO_END_SESSION()		::Ezzoo::Instrumentor::Get().EndSession()
	#define EZZOO_FUNCTION_SCOPE(x) ::Ezzoo::InstrumentTimer timer##__LINE__(x)

#else 
	#define EZZOO_BEGINE_SESSION()  
	#define EZZOO_END_SESSION()		
	#define EZZOO_FUNCTION_SCOPE(x) 
#endif