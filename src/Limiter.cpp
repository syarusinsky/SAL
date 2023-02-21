#include "Limiter.hpp"

#include <stdint.h>

#include "Common.hpp"

template <typename T>
Limiter<T>::Limiter (float attackTime, float releaseTime, float threshold, float makeupGain) :
	m_AttackTime( attackTime ),
	m_AttackCoeff( getFilterCoeff(m_AttackTime) ),
	m_ReleaseTime( releaseTime ),
	m_ReleaseCoeff( getFilterCoeff(m_ReleaseTime) ),
	m_Threshold( threshold ),
	m_MakeupGain( makeupGain ),
	m_Peak( 0.0f ),
	m_Coefficient( 0.0f ),
	m_Gain( 1.0f ),
	m_CircularBufferLength( (SAMPLE_RATE / 1000) * m_AttackTime ),
	m_CircularBuffer( new T[m_CircularBufferLength] ),
	m_WriteIndex( 0 ),
	m_ReadIndex( 1 )
{
	for ( unsigned int sample = 0; sample < m_CircularBufferLength; sample++ )
	{
		m_CircularBuffer[sample] = 0;
	}
}

template <typename T>
Limiter<T>::~Limiter()
{
	delete[] m_CircularBuffer;
}

template <typename T>
void Limiter<T>::call (T* writeBuffer)
{
	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		// envelope follower
		const float absoluteSampleVal = std::fabs( writeBuffer[sample] );

		m_Coefficient = ( absoluteSampleVal > m_Peak ) ? m_AttackCoeff : m_ReleaseCoeff;

		m_Peak = ( (1.0f - m_Coefficient) * m_Peak ) + ( m_Coefficient * absoluteSampleVal );

		// find gain to apply to signal
		const float filter = std::fmin( 1.0f, m_Threshold / m_Peak );

		m_Coefficient = ( filter < m_Gain ) ? m_AttackCoeff : m_ReleaseCoeff;

		m_Gain = ( (1.0f - m_Coefficient) * m_Gain ) + ( m_Coefficient * filter );

		// apply gain to delayed samples in circular buffer
		m_CircularBuffer[m_WriteIndex] = writeBuffer[sample];
		writeBuffer[sample] = m_Gain * m_CircularBuffer[m_ReadIndex] * m_MakeupGain;

		m_ReadIndex = ( m_ReadIndex + 1 ) % m_CircularBufferLength;
		m_WriteIndex = ( m_WriteIndex + 1 ) % m_CircularBufferLength;
	}
}

template class Limiter<float>;
template class Limiter<int16_t>;
