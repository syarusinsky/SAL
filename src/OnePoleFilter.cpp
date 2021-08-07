#include "OnePoleFilter.hpp"
#include "AudioConstants.hpp"
#include <algorithm>
#include <math.h>
#include <cstdint>

template <typename T>
OnePoleFilter<T>::OnePoleFilter() :
	m_A0( 1.0f ),
	m_B1( 0.0f ),
	m_PrevSample( 0 )
{
}

template <typename T>
OnePoleFilter<T>::~OnePoleFilter()
{
}

template <typename T>
T OnePoleFilter<T>::processSample (T sample)
{
	return this->processSampleHelper( sample );
}

template <typename T>
T OnePoleFilter<T>::processSampleHelper (T sample)
{
	m_PrevSample = ( (sample * m_A0) + (m_PrevSample * m_B1) );

	return m_PrevSample;
}

template <typename T>
void OnePoleFilter<T>::setCoefficients (float frequency)
{
	m_B1 = expf( -2.0f * M_PI * (frequency / SAMPLE_RATE / 2.0f) );
	m_A0 = 1.0f - m_B1;
}

template <typename T>
void OnePoleFilter<T>::call (T* writeBuffer)
{
	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		writeBuffer[sample] = this->processSampleHelper( writeBuffer[sample] );
	}
}

// avoid linker errors
template class OnePoleFilter<float>;
template class OnePoleFilter<int16_t>;
