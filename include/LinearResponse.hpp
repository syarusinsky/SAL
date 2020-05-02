#ifndef LINEARRESPONSE_HPP
#define LINEARRESPONSE_HPP

/**********************************************************************
 * The LinearResponse class defines a linear response, essentially
 * just returning the value given to it.
**********************************************************************/

#include "IResponse.hpp"

class LinearResponse : public IResponse
{
	public:
		LinearResponse();
		~LinearResponse() override;

		float response(const float value, const float minValue, const float maxValue) const override;
};

#endif // LINEARRESPONSE_HPP
