#include "OnePoleFilter.hpp"
#include "AudioConstants.hpp"
#include <algorithm>
#include <math.h>

OnePoleFilter::OnePoleFilter() :
	m_A0(1.0f),
	m_B1(0.0f),
	m_PrevSample(0.0f)
{
}

OnePoleFilter::~OnePoleFilter()
{
}

float OnePoleFilter::processSample (float sample)
{
	m_PrevSample = ( (sample * m_A0) + (m_PrevSample * m_B1) );
	return m_PrevSample;
}

void OnePoleFilter::setCoefficients (float frequency)
{
	m_B1 = exp(-2.0f * M_PI * (frequency / SAMPLE_RATE / 2.0f));
	m_A0 = 1.0f - m_B1;
}
