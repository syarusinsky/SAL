#include "AudioBuffer.hpp"

#include "AudioConstants.hpp"

template <typename T>
AudioBuffer<T, false>::AudioBuffer() :
	m_Buffers{ 0 },
	m_CurrentBuffer( m_Buffers ),
	m_Pos( 0 ),
	m_Callbacks(),
	m_NextReadBlockFilled( true )
{
}

template <typename T>
AudioBuffer<T, false>::AudioBuffer (const AudioBuffer& other) :
	m_Buffers{ 0 },
	m_Callbacks()
{
	const T* bufferOther = other.getBuffer1();
	for ( int sample = 0; sample < ABUFFER_SIZE * 2; sample++ )
		m_Buffers[sample] = bufferOther[sample];

	for ( IBufferCallback<T, false>* bC : other.getCallbacks() )
		m_Callbacks.insert(bC);
}

template <typename T>
AudioBuffer<T, false>::~AudioBuffer()
{
}

template <typename T>
AudioBuffer<T, false>& AudioBuffer<T, false>::operator= (const AudioBuffer& other)
{
	const T* bufferOther = other.getBuffer1();
	for ( int sample = 0; sample < ABUFFER_SIZE * 2; sample++ )
		m_Buffers[sample] = bufferOther[sample];

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
		m_CurrentBuffer = ( m_CurrentBuffer == m_Buffers ) ? &m_Buffers[ABUFFER_SIZE] : m_Buffers;
	}

	return retVal;
}

template <typename T>
void AudioBuffer<T, false>::triggerCallbacksOnNextPoll (bool writeToBuffer1)
{
	m_CurrentBuffer = ( writeToBuffer1 ) ? m_Buffers : &m_Buffers[ABUFFER_SIZE];
	m_NextReadBlockFilled = false;
}

template <typename T>
void AudioBuffer<T, false>::pollToFillBuffers()
{
	if ( ! m_NextReadBlockFilled )
	{
		T* nextBuffer = ( m_CurrentBuffer == m_Buffers ) ? &m_Buffers[ABUFFER_SIZE] : m_Buffers;

		for ( IBufferCallback<T, false>* callback : m_Callbacks )
		{
			callback->call( nextBuffer );
		}

		m_NextReadBlockFilled = true;
	}
}

template <typename T>
bool AudioBuffer<T, false>::buffer1IsNextToWrite() const
{
	return m_Buffers != m_CurrentBuffer;
}

template <typename T>
const T* AudioBuffer<T, false>::getBuffer1() const
{
	return m_Buffers;
}

template <typename T>
const T* AudioBuffer<T, false>::getBuffer2() const
{
	return &m_Buffers[ABUFFER_SIZE];
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
		return &m_Buffers[ABUFFER_SIZE];
	}
	else
	{
		return m_Buffers;
	}
}

template <typename T>
AudioBuffer<T, true>::AudioBuffer() :
	m_BuffersL{ 0 },
	m_BuffersR{ 0 },
	m_CurrentBufferL( m_BuffersL ),
	m_CurrentBufferR( m_BuffersR ),
	m_PosL( 0 ),
	m_PosR( 0 ),
	m_Callbacks(),
	m_NextReadBlockFilledL( true ),
	m_NextReadBlockFilledR( true )
{
}

template <typename T>
AudioBuffer<T, true>::AudioBuffer (const AudioBuffer& other) :
	m_BuffersL{ 0 },
	m_BuffersR{ 0 },
	m_Callbacks()
{
	const T* buffersLOther = other.getBufferL1();
	for ( int sample = 0; sample < ABUFFER_SIZE * 2; sample++ )
		m_BuffersL[sample] = buffersLOther[sample];

	const T* buffersROther = other.getBufferR1();
	for ( int sample = 0; sample < ABUFFER_SIZE * 2; sample++ )
		m_BuffersR[sample] = buffersROther[sample];

	for ( IBufferCallback<T, false>* bC : other.getCallbacks() )
		m_Callbacks.insert(bC);
}

template <typename T>
AudioBuffer<T, true>::~AudioBuffer()
{
}

template <typename T>
AudioBuffer<T, true>& AudioBuffer<T, true>::operator= (const AudioBuffer& other)
{
	const T* buffersLOther = other.getBufferL1();
	for ( int sample = 0; sample < ABUFFER_SIZE * 2; sample++ )
		m_BuffersL[sample] = buffersLOther[sample];

	const T* buffersROther = other.getBufferR1();
	for ( int sample = 0; sample < ABUFFER_SIZE * 2; sample++ )
		m_BuffersR[sample] = buffersROther[sample];

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
		m_CurrentBufferL = ( m_CurrentBufferL == m_BuffersL ) ? &m_BuffersL[ABUFFER_SIZE] : m_BuffersL;
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
		m_CurrentBufferR = ( m_CurrentBufferR == m_BuffersR ) ? &m_BuffersR[ABUFFER_SIZE] : m_BuffersR;
	}

	return retVal;
}

template <typename T>
void AudioBuffer<T, true>::triggerCallbacksOnNextPoll (bool writeToBuffer1)
{
	m_CurrentBufferL = ( writeToBuffer1 ) ? m_BuffersL : &m_BuffersL[ABUFFER_SIZE];
	m_CurrentBufferR = ( writeToBuffer1 ) ? m_BuffersR : &m_BuffersR[ABUFFER_SIZE];
	m_NextReadBlockFilledL = false;
	m_NextReadBlockFilledR = false;
}

template <typename T>
void AudioBuffer<T, true>::pollToFillBuffers()
{
	if ( ! m_NextReadBlockFilledL && ! m_NextReadBlockFilledR )
	{
		T* nextBufferL = ( m_CurrentBufferL == m_BuffersL ) ? &m_BuffersL[ABUFFER_SIZE] : m_BuffersL;
		T* nextBufferR = ( m_CurrentBufferR == m_BuffersR ) ? &m_BuffersR[ABUFFER_SIZE] : m_BuffersR;

		for ( IBufferCallback<T, true>* callback : m_Callbacks )
		{
			callback->call( nextBufferL, nextBufferR );
		}

		m_NextReadBlockFilledL = true;
		m_NextReadBlockFilledR = true;
	}
}

template <typename T>
bool AudioBuffer<T, true>::buffer1IsNextToWrite() const
{
	return ( m_BuffersL != m_CurrentBufferL && m_BuffersR != m_CurrentBufferR );
}

template <typename T>
const T* AudioBuffer<T, true>::getBufferL1() const
{
	return m_BuffersL;
}

template <typename T>
const T* AudioBuffer<T, true>::getBufferL2() const
{
	return &m_BuffersL[ABUFFER_SIZE];
}

template <typename T>
const T* AudioBuffer<T, true>::getBufferR1() const
{
	return m_BuffersR;
}

template <typename T>
const T* AudioBuffer<T, true>::getBufferR2() const
{
	return &m_BuffersR[ABUFFER_SIZE];
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
		return &m_BuffersL[ABUFFER_SIZE];
	}
	else
	{
		return m_BuffersL;
	}
}

template <typename T>
T* AudioBuffer<T, true>::getBufferR (bool writeBuffer)
{
	if ( writeBuffer )
	{
		return &m_BuffersR[ABUFFER_SIZE];
	}
	else
	{
		return m_BuffersR;
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
