#include "TriangleWaveBuffer.hpp"

static float triangleWaveArray[WBUFFER_SIZE];

TriangleWaveBuffer::TriangleWaveBuffer() :
	WaveBuffer()
{
	m_TriangleBuffer = triangleWaveArray;

	// first zero out buffer
	for (int sample = 0; sample < WBUFFER_SIZE; sample++)
	{
		m_TriangleBuffer[sample] = 0.0f;
	}

	float maxValue = 0.0f;

	// perform fourier series
	for (int harmonicNum = 1; (harmonicNum * WBUFFER_FREQ) < NYQUIST_FREQ; harmonicNum += 2)
	{
		float angularFreq = 2.0f * M_PI * harmonicNum * (1.0f / WBUFFER_SIZE);

		for (int sample = 0; sample < WBUFFER_SIZE; sample++)
		{
			m_TriangleBuffer[sample] += (1.0f / (harmonicNum * harmonicNum)) * (cos(angularFreq * sample));
			maxValue = std::max(maxValue, std::abs(m_TriangleBuffer[sample]));
		}
	}

	// limit to range
	for (int sample = 0; sample < WBUFFER_SIZE; sample++)
	{
		m_TriangleBuffer[sample] = (m_TriangleBuffer[sample] / maxValue / 2.0f);
	}
}

TriangleWaveBuffer::~TriangleWaveBuffer()
{
}
