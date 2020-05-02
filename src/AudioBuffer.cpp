#include "AudioBuffer.hpp"

#include "AudioConstants.hpp"

siike92::AudioBuffer::AudioBuffer() :
	m_Buffer1 (new float[ABUFFER_SIZE]),
	m_Buffer2 (new float[ABUFFER_SIZE]),
	m_Pos (0),
	m_Callbacks(),
	m_CurrentReadBlock(0)
{
	for (int sample = 0; sample < ABUFFER_SIZE; sample++)
	{
		m_Buffer1[sample] = 0;
		m_Buffer2[sample] = 0;
	}
}

siike92::AudioBuffer::AudioBuffer(const siike92::AudioBuffer& other) :
	m_Buffer1 (new float[ABUFFER_SIZE]),
	m_Buffer2 (new float[ABUFFER_SIZE]),
	m_Callbacks()
{
	const float* const buffer1Other = other.getBuffer1();
	for (int sample = 0; sample < ABUFFER_SIZE; sample++)
		m_Buffer1[sample] = buffer1Other[sample];

	const float* const buffer2Other = other.getBuffer2();
	for (int sample = 0; sample < ABUFFER_SIZE; sample++)
		m_Buffer2[sample] = buffer2Other[sample];

	for (IBufferCallback* bC : other.getCallbacks())
		m_Callbacks.insert(bC);
}

siike92::AudioBuffer::~AudioBuffer()
{
	delete m_Buffer1;
	delete m_Buffer2;
}

siike92::AudioBuffer& siike92::AudioBuffer::operator= (const siike92::AudioBuffer& other)
{
	const float* const buffer1Other = other.getBuffer1();
	for (int sample = 0; sample < ABUFFER_SIZE; sample++)
		m_Buffer1[sample] = buffer1Other[sample];

	const float* const buffer2Other = other.getBuffer2();
	for (int sample = 0; sample < ABUFFER_SIZE; sample++)
		m_Buffer2[sample] = buffer2Other[sample];

	m_Callbacks.clear();
	for (IBufferCallback* bC : other.getCallbacks())
		m_Callbacks.insert(bC);


	return *this;
}

unsigned int siike92::AudioBuffer::getNumSamples() const
{
	return ABUFFER_SIZE;
}

float siike92::AudioBuffer::getNextSample()
{
	float retVal = getBuffer(m_CurrentReadBlock)[m_Pos];

	m_Pos++;
	if (m_Pos >= ABUFFER_SIZE)
	{
		m_Pos = 0;

		m_CurrentReadBlock = !m_CurrentReadBlock;
		for (siike92::IBufferCallback* callback : m_Callbacks)
		{
			callback->call(getBuffer(!m_CurrentReadBlock));
		}
	}

	return retVal;
}

const float* const siike92::AudioBuffer::getBuffer1() const
{
	return m_Buffer1;
}

const float* const siike92::AudioBuffer::getBuffer2() const
{
	return m_Buffer2;
}

const std::set<siike92::IBufferCallback*>& siike92::AudioBuffer::getCallbacks() const
{
	return m_Callbacks;
}

void siike92::AudioBuffer::registerCallback(IBufferCallback* callback)
{
	if (callback)
	{
		m_Callbacks.insert(callback);
	}
}

float* siike92::AudioBuffer::getBuffer(bool writeBuffer)
{
	if (writeBuffer)
	{
		return m_Buffer2;
	}
	else
	{
		return m_Buffer1;
	}
}
