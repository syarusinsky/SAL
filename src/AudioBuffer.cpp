#include "AudioBuffer.hpp"

#include "AudioConstants.hpp"

template <typename T>
AudioBuffer<T, false>::AudioBuffer() :
	m_Buffer1( new T[ABUFFER_SIZE] ),
	m_Buffer2( new T[ABUFFER_SIZE] ),
	m_CurrentBuffer( m_Buffer1 ),
	m_Pos( 0 ),
	m_Callbacks(),
	m_NextReadBlockFilled( true )
{
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		m_Buffer1[sample] = 0;
		m_Buffer2[sample] = 0;
	}
}

template <typename T>
AudioBuffer<T, false>::AudioBuffer (const AudioBuffer& other) :
	m_Buffer1( new T[ABUFFER_SIZE] ),
	m_Buffer2( new T[ABUFFER_SIZE] ),
	m_Callbacks()
{
	const T* const buffer1Other = other.getBuffer1();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer1[sample] = buffer1Other[sample];

	const T* const buffer2Other = other.getBuffer2();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer2[sample] = buffer2Other[sample];

	for ( IBufferCallback<T, false>* bC : other.getCallbacks() )
		m_Callbacks.insert(bC);
}

template <typename T>
AudioBuffer<T, false>::~AudioBuffer()
{
	delete[] m_Buffer1;
	delete[] m_Buffer2;
}

template <typename T>
AudioBuffer<T, false>& AudioBuffer<T, false>::operator= (const AudioBuffer& other)
{
	const T* const buffer1Other = other.getBuffer1();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer1[sample] = buffer1Other[sample];

	const T* const buffer2Other = other.getBuffer2();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer2[sample] = buffer2Other[sample];

	m_Callbacks.clear();
	for ( IBufferCallback<T, false>* bC : other.getCallbacks() )
		m_Callbacks.insert( bC );


	return *this;
}

template <typename T>
unsigned int AudioBuffer<T, false>::getNumSamples() const
{
	return ABUFFER_SIZE;
}

template <typename T>
T AudioBuffer<T, false>::getNextSample (T sampleValToReadBuf)
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
void AudioBuffer<T, false>::pollToFillBuffers()
{
	if ( ! m_NextReadBlockFilled )
	{
		T* nextBuffer = ( m_CurrentBuffer == m_Buffer1 ) ? m_Buffer2 : m_Buffer1;

		for ( IBufferCallback<T, false>* callback : m_Callbacks )
		{
			callback->call( nextBuffer );
		}

		m_NextReadBlockFilled = true;
	}
}

template <typename T>
const T* const AudioBuffer<T, false>::getBuffer1() const
{
	return m_Buffer1;
}

template <typename T>
const T* const AudioBuffer<T, false>::getBuffer2() const
{
	return m_Buffer2;
}

template <typename T>
const std::set<IBufferCallback<T, false>*>& AudioBuffer<T, false>::getCallbacks() const
{
	return m_Callbacks;
}

template <typename T>
void AudioBuffer<T, false>::registerCallback (IBufferCallback<T, false>* callback)
{
	if ( callback )
	{
		m_Callbacks.insert(callback);
	}
}

template <typename T>
T* AudioBuffer<T, false>::getBuffer (bool writeBuffer)
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

template <typename T>
AudioBuffer<T, true>::AudioBuffer() :
	m_BufferL1( new T[ABUFFER_SIZE] ),
	m_BufferL2( new T[ABUFFER_SIZE] ),
	m_BufferR1( new T[ABUFFER_SIZE] ),
	m_BufferR2( new T[ABUFFER_SIZE] ),
	m_CurrentBufferL( m_BufferL1 ),
	m_CurrentBufferR( m_BufferR1 ),
	m_PosL( 0 ),
	m_PosR( 0 ),
	m_Callbacks(),
	m_NextReadBlockFilledL( true ),
	m_NextReadBlockFilledR( true )
{
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		m_BufferL1[sample] = 0;
		m_BufferL2[sample] = 0;
		m_BufferR1[sample] = 0;
		m_BufferR2[sample] = 0;
	}
}

template <typename T>
AudioBuffer<T, true>::AudioBuffer (const AudioBuffer& other) :
	m_BufferL1( new T[ABUFFER_SIZE] ),
	m_BufferL2( new T[ABUFFER_SIZE] ),
	m_BufferR1( new T[ABUFFER_SIZE] ),
	m_BufferR2( new T[ABUFFER_SIZE] ),
	m_Callbacks()
{
	const T* const bufferL1Other = other.getBufferL1();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_BufferL1[sample] = bufferL1Other[sample];

	const T* const bufferL2Other = other.getBufferL2();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_BufferL2[sample] = bufferL2Other[sample];

	const T* const bufferR1Other = other.getBufferR1();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_BufferR1[sample] = bufferR1Other[sample];

	const T* const bufferR2Other = other.getBufferR2();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_BufferR2[sample] = bufferR2Other[sample];

	for ( IBufferCallback<T, false>* bC : other.getCallbacks() )
		m_Callbacks.insert(bC);
}

template <typename T>
AudioBuffer<T, true>::~AudioBuffer()
{
	delete[] m_BufferL1;
	delete[] m_BufferL2;
	delete[] m_BufferR1;
	delete[] m_BufferR2;
}

template <typename T>
AudioBuffer<T, true>& AudioBuffer<T, true>::operator= (const AudioBuffer& other)
{
	const T* const bufferL1Other = other.getBufferL1();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_BufferL1[sample] = bufferL1Other[sample];

	const T* const bufferL2Other = other.getBufferL2();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_BufferL2[sample] = bufferL2Other[sample];

	const T* const bufferR1Other = other.getBufferR1();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_BufferR1[sample] = bufferR1Other[sample];

	const T* const bufferR2Other = other.getBufferR2();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_BufferR2[sample] = bufferR2Other[sample];

	m_Callbacks.clear();
	for ( IBufferCallback<T, false>* bC : other.getCallbacks() )
		m_Callbacks.insert( bC );


	return *this;
}

template <typename T>
unsigned int AudioBuffer<T, true>::getNumSamples() const
{
	return ABUFFER_SIZE;
}

template <typename T>
T AudioBuffer<T, true>::getNextSampleL (T sampleValToReadBuf)
{
	T retVal = m_CurrentBufferL[m_PosL];

	m_CurrentBufferL[m_PosL] = sampleValToReadBuf;

	m_PosL++;
	if ( m_PosL == ABUFFER_SIZE )
	{
		m_PosL = 0;

		m_NextReadBlockFilledL = false;
		m_CurrentBufferL = ( m_CurrentBufferL == m_BufferL1 ) ? m_BufferL2 : m_BufferL1;
	}

	return retVal;
}

template <typename T>
T AudioBuffer<T, true>::getNextSampleR (T sampleValToReadBuf)
{
	T retVal = m_CurrentBufferR[m_PosR];

	m_CurrentBufferR[m_PosR] = sampleValToReadBuf;

	m_PosR++;
	if ( m_PosR == ABUFFER_SIZE )
	{
		m_PosR = 0;

		m_NextReadBlockFilledR = false;
		m_CurrentBufferR = ( m_CurrentBufferR == m_BufferR1 ) ? m_BufferR2 : m_BufferR1;
	}

	return retVal;
}

template <typename T>
void AudioBuffer<T, true>::pollToFillBuffers()
{
	if ( ! m_NextReadBlockFilledL && ! m_NextReadBlockFilledR )
	{
		T* nextBufferL = ( m_CurrentBufferL == m_BufferL1 ) ? m_BufferL2 : m_BufferL1;
		T* nextBufferR = ( m_CurrentBufferR == m_BufferR1 ) ? m_BufferR2 : m_BufferR1;

		for ( IBufferCallback<T, true>* callback : m_Callbacks )
		{
			callback->call( nextBufferL, nextBufferR );
		}

		m_NextReadBlockFilledL = true;
		m_NextReadBlockFilledR = true;
	}
}

template <typename T>
const T* const AudioBuffer<T, true>::getBufferL1() const
{
	return m_BufferL1;
}

template <typename T>
const T* const AudioBuffer<T, true>::getBufferL2() const
{
	return m_BufferL2;
}

template <typename T>
const T* const AudioBuffer<T, true>::getBufferR1() const
{
	return m_BufferR1;
}

template <typename T>
const T* const AudioBuffer<T, true>::getBufferR2() const
{
	return m_BufferR2;
}

template <typename T>
const std::set<IBufferCallback<T, true>*>& AudioBuffer<T, true>::getCallbacks() const
{
	return m_Callbacks;
}

template <typename T>
void AudioBuffer<T, true>::registerCallback (IBufferCallback<T, true>* callback)
{
	if ( callback )
	{
		m_Callbacks.insert(callback);
	}
}

template <typename T>
T* AudioBuffer<T, true>::getBufferL (bool writeBuffer)
{
	if ( writeBuffer )
	{
		return m_BufferL2;
	}
	else
	{
		return m_BufferL1;
	}
}

template <typename T>
T* AudioBuffer<T, true>::getBufferR (bool writeBuffer)
{
	if ( writeBuffer )
	{
		return m_BufferR2;
	}
	else
	{
		return m_BufferR1;
	}
}

// avoid linker errors
template class AudioBuffer<float, false>;
template class AudioBuffer<uint16_t, false>;
template class AudioBuffer<int16_t, false>;

// TODO no idea why I have to do this, look into a better way later
template AudioBuffer<float, true>::AudioBuffer();
template AudioBuffer<uint16_t, true>::AudioBuffer();
template AudioBuffer<int16_t, true>::AudioBuffer();
template void AudioBuffer<float, true>::registerCallback(IBufferCallback<float, true>*);
template void AudioBuffer<uint16_t, true>::registerCallback(IBufferCallback<uint16_t, true>*);
template void AudioBuffer<int16_t, true>::registerCallback(IBufferCallback<int16_t, true>*);
template float AudioBuffer<float, true>::getNextSampleL(float sampleValToRead);
template uint16_t AudioBuffer<uint16_t, true>::getNextSampleL(uint16_t sampleValToRead);
template int16_t AudioBuffer<int16_t, true>::getNextSampleL(int16_t sampleValToRead);
template float AudioBuffer<float, true>::getNextSampleR(float sampleValToRead);
template uint16_t AudioBuffer<uint16_t, true>::getNextSampleR(uint16_t sampleValToRead);
template int16_t AudioBuffer<int16_t, true>::getNextSampleR(int16_t sampleValToRead);
template void AudioBuffer<float, true>::pollToFillBuffers();
template void AudioBuffer<uint16_t, true>::pollToFillBuffers();
template void AudioBuffer<int16_t, true>::pollToFillBuffers();
