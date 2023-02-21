#include "AllpassCombFilter.hpp"

#include "AudioConstants.hpp"

#include <stdint.h>

template <typename T>
AllpassCombFilter<T>::AllpassCombFilter (unsigned int delayLength, float feedbackGain, T initVal) :
	m_DelayLength( delayLength + 1 ),
	m_DelayBuffer( new T[m_DelayLength] ),
	m_DelayWriteIncr( 0 ),
	m_DelayReadIncr( 1 ),
	m_FeedbackGain( feedbackGain )
{
	for ( unsigned int sample = 0; sample < m_DelayLength; sample++ )
	{
		m_DelayBuffer[sample] = initVal;
	}
}

template <typename T>
AllpassCombFilter<T>::~AllpassCombFilter()
{
	delete[] m_DelayBuffer;
}

template <typename T>
void AllpassCombFilter<T>::setDelayLength (unsigned int delayLength)
{
	m_DelayReadIncr = ( m_DelayWriteIncr + (m_DelayLength - delayLength) ) % m_DelayLength;
}

template <typename T>
void AllpassCombFilter<T>::setFeedbackGain (float feedbackGain)
{
	m_FeedbackGain = feedbackGain;
}

template <typename T>
void AllpassCombFilter<T>::call (T* writeBuffer)
{
	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		writeBuffer[sample] = this->processSample( writeBuffer[sample] );
	}
}

template <typename T>
void AllpassCombFilter<T>::call (T* writeBuffer, unsigned int numModSamples, float* modSource)
{
	unsigned int unmodulatedSamples = m_DelayLength - numModSamples;
	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		writeBuffer[sample] = this->processSample( writeBuffer[sample] );
		this->setDelayLength( unmodulatedSamples + static_cast<unsigned int>(numModSamples * modSource[sample]) );
	}
}

// avoid linker errors
template class AllpassCombFilter<float>;
template class AllpassCombFilter<int16_t>;
