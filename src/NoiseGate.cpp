#include "NoiseGate.hpp"

#include "Common.hpp"

#include <cstdint>

constexpr float MS_PER_SAMPLE = ( static_cast<float>(SAMPLE_PERIOD) * 1000.0f );

template <typename T>
NoiseGate<T>::NoiseGate (float attackReleaseTimeMS, float holdTimeMS, T peakThreshold) :
	m_AttackReleaseTime( attackReleaseTimeMS ),
	m_AttackReleaseCoeff( getFilterCoeff(m_AttackReleaseTime) ),
	m_HoldTime( holdTimeMS ),
	m_Threshold( peakThreshold ),
	m_Hold( 0.0f ),
	m_Coefficient( 0.0f ),
	m_Gain( 1.0f )
{
}

template <typename T>
NoiseGate<T>::~NoiseGate()
{
}

template <typename T>
void NoiseGate<T>::call (T* writeBuffer)
{
	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		// envelope follower
		T absoluteSampleVal = std::abs( writeBuffer[sample] );

		if ( absoluteSampleVal <= m_Threshold )
		{
			m_Hold += MS_PER_SAMPLE;
		}
		else
		{
			m_Hold = 0.0f;
		}

		// get new gain value based on hold time
		float newGain = 1.0f;
		if ( m_Hold >= m_HoldTime )
		{
			newGain = 0.0f;
		}

		// filter gain
		m_Gain = ( (1.0f - m_AttackReleaseCoeff) * m_Gain ) + ( m_AttackReleaseCoeff * newGain );

		// apply gain
		writeBuffer[sample] = writeBuffer[sample] * m_Gain;
	}
}

// avoid linker errors
template class NoiseGate<int16_t>;
template class NoiseGate<float>;
