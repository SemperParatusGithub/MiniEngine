#pragma once
#include "EngineBase.h"

#include <random>


namespace Engine
{
	class UUID
	{
	public:
		UUID();
		UUID(const UUID&) = default;
		~UUID() = default;

		inline operator u64() const
		{
			return m_UUID;
		}

	private:
		u64 m_UUID;

		static std::random_device					s_RandomDevice;
		static std::mt19937_64						s_Gen; 
		static std::uniform_int_distribution<u64>	s_UniformDistribution;
	};
}