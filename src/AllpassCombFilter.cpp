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
T AllpassCombFilter<T>::processSample (T sampleVal)
{
	return processSampleHelper( sampleVal );
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
		writeBuffer[sample] = this->processSampleHelper( writeBuffer[sample] );
	}
}

template <typename T>
T AllpassCombFilter<T>::processSampleHelper (T sampleVal)
{
	T delayedVal = m_DelayBuffer[m_DelayReadIncr];
	T inputSum = ( sampleVal - (delayedVal * m_FeedbackGain) );
	m_DelayBuffer[m_DelayWriteIncr] = inputSum;

	T outVal = ( (inputSum * m_FeedbackGain) + delayedVal );

	m_DelayWriteIncr = ( m_DelayWriteIncr + 1 ) % m_DelayLength;
	m_DelayReadIncr = ( m_DelayReadIncr + 1 ) % m_DelayLength;

	return outVal;
}

// avoid linker errors
template class AllpassCombFilter<float>;
template class AllpassCombFilter<int16_t>;
