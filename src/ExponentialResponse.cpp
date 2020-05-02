#include "ExponentialResponse.hpp"

#include <math.h>

ExponentialResponse::ExponentialResponse (const float slope) :
	m_Slope (slope)
{
}

ExponentialResponse::~ExponentialResponse()
{
}

float ExponentialResponse::response (const float value, const float minValue, const float maxValue) const
{
	const float span = maxValue - minValue;
	const float normalizedValue = value / span;
	const float normalized = (pow(m_Slope + 1, normalizedValue) - 1.0f) / m_Slope;

	return std::fmin( ((normalized * span) - std::abs(minValue)), maxValue);
}

void ExponentialResponse::setSlope (const float slope)
{
	if (slope < 0.1f)
	{
		m_Slope = 0.1f;
	}
	else
	{
		m_Slope = slope;
	}
}
