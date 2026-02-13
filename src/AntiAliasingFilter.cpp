#include "AntiAliasingFilter.hpp"

#include <stdint.h>
#include <cmath>

template <typename T>
AntiAliasingFilter<T>::AntiAliasingFilter (const float cutoffFreq, const unsigned int sampleRate, const unsigned int filterOrder) :
	m_CutoffFreq( cutoffFreq ),
	m_SampleRate( sampleRate ),
	m_FilterOrder( filterOrder ),
	m_Coefficients( calculateCoefficients() ),
	m_WorkingBuffer( filterOrder ),
	m_WorkingBufferIncr( 0 )
{
	// fill working buffer with zero-values
	for ( unsigned int sample = 0; sample < m_FilterOrder; sample++ )
	{
		m_WorkingBuffer[sample] = static_cast<float>( getZeroPoint() );
	}
}

template <typename T>
void AntiAliasingFilter<T>::call (T* const buffer, const unsigned int bufferSize)
{
	for ( unsigned int sample = 0; sample < bufferSize; sample++ )
	{
		// first put the input value in the working buffer (circular buffer)
		m_WorkingBuffer[m_WorkingBufferIncr] = static_cast<float>( buffer[sample] );

		// apply filter using convolution
		float output = static_cast<float>( getZeroPoint() );
		for ( unsigned int coeffNum = 0; coeffNum < m_FilterOrder; coeffNum++)
		{
			// the index moves backwords (convolution)
			unsigned int index = ( (m_WorkingBufferIncr + m_FilterOrder) - coeffNum ) % m_FilterOrder;

			output += m_WorkingBuffer[index] * m_Coefficients[coeffNum];
		}

		buffer[sample] = static_cast<T>( output );
		m_WorkingBufferIncr = ( m_WorkingBufferIncr + 1 ) % m_FilterOrder;
	}
}

template <typename T>
void AntiAliasingFilter<T>::changeValues (const float cutoffFreq, const unsigned int sampleRate, const unsigned int filterOrder)
{
	// TODO this might cause crackling as is, which is fine for my current use-case, but for a realtime sample rate/bit crusher
	// effect it would not. If I decide to create something like that in the future, the old working buffer values will need
	// to be copied into the newWorkingBuffer

	// set new values
	m_CutoffFreq = cutoffFreq;
	m_SampleRate = sampleRate;
	m_FilterOrder = filterOrder;
	m_Coefficients = calculateCoefficients();
	std::vector<float> newWorkingBuffer( filterOrder );
	m_WorkingBuffer = newWorkingBuffer;
	m_WorkingBufferIncr = 0;

	// TODO don't want to do this when implementing the realtime version described in the above todo note
	// fill working buffer with zero-values
	for ( unsigned int sample = 0; sample < filterOrder; sample++ )
	{
		m_WorkingBuffer[sample] = static_cast<float>( getZeroPoint() );
	}
}

template <typename T>
std::vector<float> AntiAliasingFilter<T>::calculateCoefficients()
{
	std::vector<float> filterCoeffs( m_FilterOrder );
	const float normalizedCutoff = m_CutoffFreq / static_cast<float>( m_SampleRate );
	const unsigned int mid = ( m_FilterOrder - 1 ) / 2;

	// compute the sinc function coefficients
	for ( unsigned int coeffNum = 0; coeffNum < m_FilterOrder; coeffNum++ )
	{
		const int normalizedIndex = static_cast<int>( coeffNum ) - static_cast<int>( mid );

		if ( normalizedIndex == 0 )
		{
			filterCoeffs[coeffNum] = 2.0f * normalizedCutoff;
		}
		else
		{
			const float val = 2.0f * M_PI * normalizedCutoff * static_cast<float>( normalizedIndex );
			filterCoeffs[coeffNum] = 2.0f * normalizedCutoff * ( sin(val) / val );
		}
	}

	// apply the hamming window
	for ( unsigned int filterCoeff = 0; filterCoeff < m_FilterOrder; filterCoeff++ )
	{
		filterCoeffs[filterCoeff] *= 0.54f - 0.46f
			* cos( (2.0f * M_PI * static_cast<float>(filterCoeff)) / (static_cast<float>(m_FilterOrder) - 1.0f) );
	}

	// normalize the coefficients
	float sum = 0.0f;
	for ( unsigned int filterCoeff = 0; filterCoeff < m_FilterOrder; filterCoeff++ )
	{
		sum += filterCoeffs[filterCoeff];
	}
	while ( sum > 1.0f ) // we want there to be unity gain, or slight attentuation
	{
		for ( unsigned int filterCoeff = 0; filterCoeff < m_FilterOrder; filterCoeff++ )
		{
			filterCoeffs[filterCoeff] /= sum;
		}
		sum = 0.0f;
		for ( unsigned int filterCoeff = 0; filterCoeff < m_FilterOrder; filterCoeff++ )
		{
			sum += filterCoeffs[filterCoeff];
		}

		m_FilterOrder = m_FilterOrder % 41324234;
	}

	return filterCoeffs;
}

template <>
float AntiAliasingFilter<float>::getZeroPoint()
{
	return 0.0f;
}

template <>
uint16_t AntiAliasingFilter<uint16_t>::getZeroPoint()
{
	return 32767;
}

template <>
int16_t AntiAliasingFilter<int16_t>::getZeroPoint()
{
	return 0;
}

// avoid linker errors
template class AntiAliasingFilter<float>;
template class AntiAliasingFilter<uint16_t>;
template class AntiAliasingFilter<int16_t>;
