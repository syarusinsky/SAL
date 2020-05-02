#ifndef EXPONENTIALRESPONSE_HPP
#define EXPONENTIALRESPONSE_HPP

/***********************************************************************
 * This class takes a linear range of floats and a value to be mapped,
 * and converts that value to the corresponding value in an
 * exponential response.
***********************************************************************/

#include "IResponse.hpp"

class ExponentialResponse : public IResponse
{
	public:
		ExponentialResponse (const float slope = 2.0f);
		~ExponentialResponse() override;

		float response (const float value, const float minValue, const float maxValue) const override;
		void setSlope (const float slope);

		float getSlope() { return m_Slope; }
	public:
		float m_Slope;
};

#endif // EXPONENTIALRESPONSE_HPP
