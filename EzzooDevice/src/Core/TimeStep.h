#pragma once

namespace Ezzoo {

	class TimeStep
	{
	public:
		TimeStep(float time = 0.0f ): m_Time(time)
		{

		}

		operator float() const { return m_Time; }

	private:
		float m_Time;
	};
}