#include "SimpleDelay.hpp"

#include "AudioConstants.hpp"

#include <stdint.h>
#include <cmath>

template <typename T>
SimpleDelay<T>::SimpleDelay (unsigned int maxDelayLength, unsigned int delayLength, T initVal) :
	m_DelayLength( maxDelayLength + 1 ),
	m_DelayBuffer( new T[m_DelayLength] ),
	m_DelayWriteIncr( 0 ),
	m_DelayReadIncr( 0 )
{
	this->setDelayLength( delayLength );

	for ( unsigned int sample = 0; sample < m_DelayLength; sample++ )
	{
		m_DelayBuffer[sample] = initVal;
	}
}

template <typename T>
SimpleDelay<T>::~SimpleDelay()
{
	delete[] m_DelayBuffer;
}

template <typename T>
T SimpleDelay<T>::processSample (T sampleVal)
{
	return this->processSampleHelper( sampleVal );
}

template <typename T>
T SimpleDelay<T>::processSampleHelper (T sampleVal)
{
	T delayedVal = m_DelayBuffer[m_DelayReadIncr];
	m_DelayBuffer[m_DelayWriteIncr] = sampleVal;

	m_DelayWriteIncr = ( m_DelayWriteIncr + 1 ) % m_DelayLength;
	m_DelayReadIncr = ( m_DelayReadIncr + 1 ) % m_DelayLength;

	return delayedVal;
}

template <typename T>
void SimpleDelay<T>::setDelayLength (unsigned int delayLength)
{
	m_DelayReadIncr = ( m_DelayWriteIncr + (m_DelayLength - delayLength) ) % m_DelayLength;
}

template <typename T>
void SimpleDelay<T>::call (T* writeBuffer)
{
	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		writeBuffer[sample] = this->processSampleHelper( writeBuffer[sample] );
	}
}

// avoid linker errors
template class SimpleDelay<float>;
template class SimpleDelay<int16_t>;
