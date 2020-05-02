#ifndef IRESPONSE_HPP
#define IRESPONSE_HPP

/**************************************************************************
 * An IResponse specifies a response between a certain number range.
 * For example, taking a linear range of numbers and generating a value
 * from a exponential range of numbers.
**************************************************************************/

class IResponse
{
	public:
		virtual ~IResponse(){}
		virtual float response (const float value, const float minValue, const float maxValue) const = 0;
		virtual void setSlope (const float slope) {}
		virtual float getSlope() { return 0.0f; }
};

#endif // IRESPONSE_HPP
