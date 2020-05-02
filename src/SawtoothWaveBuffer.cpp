#include "SawtoothWaveBuffer.hpp"

static float sawtoothWaveArray[WBUFFER_SIZE];

SawtoothWaveBuffer::SawtoothWaveBuffer() :
	WaveBuffer()
{
	m_SawtoothBuffer = sawtoothWaveArray;

	// first zero out buffer
	for (int sample = 0; sample < WBUFFER_SIZE; sample++)
	{
		m_SawtoothBuffer[sample] = 0.0f;
	}

	float maxValue = 0.0f;

	// perform fourier series
	for (int harmonicNum = 1; (harmonicNum * WBUFFER_FREQ) < NYQUIST_FREQ; harmonicNum++)
	{
		bool isEven = (harmonicNum % 2 == 0);
		float addOrSubtract = 1.0f;

		if (isEven)
		{
			addOrSubtract = -1.0f;
		}

		float angularFreq = 2.0f * M_PI * harmonicNum * (1.0f / WBUFFER_SIZE);

		for (int sample = 0; sample < WBUFFER_SIZE; sample++)
		{
			m_SawtoothBuffer[sample] += (addOrSubtract) * (1.0f / harmonicNum) * (sin(angularFreq * sample));
			maxValue = std::max(maxValue, std::abs(m_SawtoothBuffer[sample]));
		}
	}

	// limit to range
	for (int sample = 0; sample < WBUFFER_SIZE; sample++)
	{
		m_SawtoothBuffer[sample] = (m_SawtoothBuffer[sample] / maxValue / 2.0f);
	}
}

SawtoothWaveBuffer::~SawtoothWaveBuffer()
{
}
