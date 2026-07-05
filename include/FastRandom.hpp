#ifndef FASTRANDOM_HPP
#define FASTRANDOM_HPP

/***********************************************************************
 * The FastRandom class is capable of producing fairly random numbers
 * without having to resort to large crypographically secure tables that
 * bloat data memory usage and slow down runtime speed.
***********************************************************************/

#include <cstdint>

class FastRandom
{
	public:
		uint32_t next()
		{
			m_State ^= m_State << 13;
			m_State ^= m_State >> 17;
			m_State ^= m_State << 5;

			return m_State;
		}

		// returns a float between 0.0f and 1.0f using the FPU
		float nextFloat()
		{
			return static_cast<float>( next() ) / static_cast<float>( 0xFFFFFFFFU );
		}

	private:
		uint32_t m_State = 2463534242U; // Seed (must be non-zero)
};

#endif // FASTRANDOM_HPP
