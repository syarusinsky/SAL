#include "SineWaveBuffer.hpp"

static float sineWaveArray[WBUFFER_SIZE];

SineWaveBuffer::SineWaveBuffer() :
	WaveBuffer()
{
	m_SineBuffer = sineWaveArray;

	float angularFreq = 2.0f * M_PI * (1.0f / WBUFFER_SIZE);

	for (int sample = 0; sample < WBUFFER_SIZE; sample++)
	{
		m_SineBuffer[sample] = sin(angularFreq * sample) / 2.0f;
	}
}

SineWaveBuffer::~SineWaveBuffer()
{
}
