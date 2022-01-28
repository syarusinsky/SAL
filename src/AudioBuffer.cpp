#include "AudioBuffer.hpp"

#include "AudioConstants.hpp"

template <typename T>
AudioBuffer<T>::AudioBuffer() :
	m_Buffer1{ 0 },
	m_Buffer2{ 0 },
	m_CurrentBuffer( m_Buffer1 ),
	m_Pos( 0 ),
	m_Callbacks(),
	m_NextReadBlockFilled( true )
{
}

template <typename T>
AudioBuffer<T>::AudioBuffer (const AudioBuffer& other) :
	m_Buffer1{ 0 },
	m_Buffer2{ 0 },
	m_Callbacks()
{
	const T* const buffer1Other = other.getBuffer1();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer1[sample] = buffer1Other[sample];

	const T* const buffer2Other = other.getBuffer2();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer2[sample] = buffer2Other[sample];

	for ( IBufferCallback<T>* bC : other.getCallbacks() )
		m_Callbacks.insert(bC);
}

template <typename T>
AudioBuffer<T>::~AudioBuffer()
{
}

template <typename T>
AudioBuffer<T>& AudioBuffer<T>::operator= (const AudioBuffer& other)
{
	const T* const buffer1Other = other.getBuffer1();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer1[sample] = buffer1Other[sample];

	const T* const buffer2Other = other.getBuffer2();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer2[sample] = buffer2Other[sample];

	m_Callbacks.clear();
	for ( IBufferCallback<T>* bC : other.getCallbacks() )
		m_Callbacks.insert( bC );


	return *this;
}

template <typename T>
unsigned int AudioBuffer<T>::getNumSamples() const
{
	return ABUFFER_SIZE;
}

template <typename T>
T AudioBuffer<T>::getNextSample (T sampleValToReadBuf)
{
	T retVal = m_CurrentBuffer[m_Pos];

	m_CurrentBuffer[m_Pos] = sampleValToReadBuf;

	m_Pos++;
	if ( m_Pos == ABUFFER_SIZE )
	{
		m_Pos = 0;

		m_NextReadBlockFilled = false;
		m_CurrentBuffer = ( m_CurrentBuffer == m_Buffer1 ) ? m_Buffer2 : m_Buffer1;
	}

	return retVal;
}

template <typename T>
void AudioBuffer<T>::pollToFillBuffers()
{
	if ( ! m_NextReadBlockFilled )
	{
		T* nextBuffer = ( m_CurrentBuffer == m_Buffer1 ) ? m_Buffer2 : m_Buffer1;

		for ( IBufferCallback<T>* callback : m_Callbacks )
		{
			callback->call( nextBuffer );
		}

		m_NextReadBlockFilled = true;
	}
}

template <typename T>
const T* const AudioBuffer<T>::getBuffer1() const
{
	return m_Buffer1;
}

template <typename T>
const T* const AudioBuffer<T>::getBuffer2() const
{
	return m_Buffer2;
}

template <typename T>
const std::set<IBufferCallback<T>*>& AudioBuffer<T>::getCallbacks() const
{
	return m_Callbacks;
}

template <typename T>
void AudioBuffer<T>::registerCallback (IBufferCallback<T>* callback)
{
	if ( callback )
	{
		m_Callbacks.insert(callback);
	}
}

template <typename T>
T* AudioBuffer<T>::getBuffer (bool writeBuffer)
{
	if ( writeBuffer )
	{
		return m_Buffer2;
	}
	else
	{
		return m_Buffer1;
	}
}

// avoid linker errors
template class AudioBuffer<float>;
template class AudioBuffer<uint16_t>;
template class AudioBuffer<int16_t>;
