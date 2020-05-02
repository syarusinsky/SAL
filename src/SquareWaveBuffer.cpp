#include "SquareWaveBuffer.hpp"

static float squareWaveArray[WBUFFER_SIZE];

SquareWaveBuffer::SquareWaveBuffer() :
	WaveBuffer()
{
	m_SquareBuffer = squareWaveArray;

	// first zero out buffer
	for (int sample = 0; sample < WBUFFER_SIZE; sample++)
	{
		m_SquareBuffer[sample] = 0.0f;
	}

	float maxValue = 0.0f;

	// perform fourier series
	for (int harmonicNum = 1; (harmonicNum * WBUFFER_FREQ) < NYQUIST_FREQ; harmonicNum += 2)
	{
		float angularFreq = 2.0f * M_PI * harmonicNum * (1.0f / WBUFFER_SIZE);

		for (int sample = 0; sample < WBUFFER_SIZE; sample++)
		{
			m_SquareBuffer[sample] += (1.0f / harmonicNum) * (sin(angularFreq * sample));
			maxValue = std::max(maxValue, std::abs(m_SquareBuffer[sample]));
		}
	}

	// limit to range
	for (int sample = 0; sample < WBUFFER_SIZE; sample++)
	{
		m_SquareBuffer[sample] = (m_SquareBuffer[sample] / maxValue / 2.0f);
	}
}

SquareWaveBuffer::~SquareWaveBuffer()
{
}
