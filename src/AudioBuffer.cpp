#include "AudioBuffer.hpp"

#include "AudioConstants.hpp"

AudioBuffer::AudioBuffer() :
	m_Buffer1( new float[ABUFFER_SIZE] ),
	m_Buffer2( new float[ABUFFER_SIZE] ),
	m_Pos( 0 ),
	m_Callbacks(),
	m_CurrentReadBlock( false ),
	m_NextReadBlockFilled( false )
{
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		m_Buffer1[sample] = 0;
		m_Buffer2[sample] = 0;
	}
}

AudioBuffer::AudioBuffer (const AudioBuffer& other) :
	m_Buffer1( new float[ABUFFER_SIZE] ),
	m_Buffer2( new float[ABUFFER_SIZE] ),
	m_Callbacks()
{
	const float* const buffer1Other = other.getBuffer1();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer1[sample] = buffer1Other[sample];

	const float* const buffer2Other = other.getBuffer2();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer2[sample] = buffer2Other[sample];

	for ( IBufferCallback* bC : other.getCallbacks() )
		m_Callbacks.insert(bC);
}

AudioBuffer::~AudioBuffer()
{
	delete[] m_Buffer1;
	delete[] m_Buffer2;
}

AudioBuffer& AudioBuffer::operator= (const AudioBuffer& other)
{
	const float* const buffer1Other = other.getBuffer1();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer1[sample] = buffer1Other[sample];

	const float* const buffer2Other = other.getBuffer2();
	for ( int sample = 0; sample < ABUFFER_SIZE; sample++ )
		m_Buffer2[sample] = buffer2Other[sample];

	m_Callbacks.clear();
	for ( IBufferCallback* bC : other.getCallbacks() )
		m_Callbacks.insert( bC );


	return *this;
}

unsigned int AudioBuffer::getNumSamples() const
{
	return ABUFFER_SIZE;
}

float AudioBuffer::getNextSample()
{
	float retVal = getBuffer( m_CurrentReadBlock )[m_Pos];

	m_Pos++;
	if ( m_Pos >= ABUFFER_SIZE )
	{
		m_Pos = 0;

		m_CurrentReadBlock = ! m_CurrentReadBlock;
		m_NextReadBlockFilled = false;
	}

	return retVal;
}

void AudioBuffer::pollToFillBuffers()
{
	if ( ! m_NextReadBlockFilled )
	{
		for ( IBufferCallback* callback : m_Callbacks )
		{
			callback->call( getBuffer(!m_CurrentReadBlock) );
		}

		m_NextReadBlockFilled = true;
	}
}

const float* const AudioBuffer::getBuffer1() const
{
	return m_Buffer1;
}

const float* const AudioBuffer::getBuffer2() const
{
	return m_Buffer2;
}

const std::set<IBufferCallback*>& AudioBuffer::getCallbacks() const
{
	return m_Callbacks;
}

void AudioBuffer::registerCallback (IBufferCallback* callback)
{
	if ( callback )
	{
		m_Callbacks.insert(callback);
	}
}

float* AudioBuffer::getBuffer (bool writeBuffer)
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
