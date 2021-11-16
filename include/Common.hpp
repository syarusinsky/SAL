#ifndef COMMON_HPP
#define COMMON_HPP

#include "AudioConstants.hpp"
#include <cmath>

// for calculating filter coefficients of the form ( (1.0f - m_Coefficient) * oldValue ) + ( m_Coefficient * newValue );
inline float getFilterCoeff (float timeInMs)
{
	return (1.0f - pow(M_E, (-2.2f * SAMPLE_PERIOD) / (timeInMs / 1000.0f))) * 2.0f;
}

#endif // COMMON_HPP
