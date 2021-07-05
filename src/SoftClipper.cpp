#include "SoftClipper.hpp"

#include <type_traits>
#include <math.h>

template <typename T>
T SoftClipper<T>::processSample (T sampleVal)
{

	float tempVal = sampleVal;
	if (std::is_same<T, uint16_t>::value)
	{
		// convert to float between -1.0f and 1.0f
		tempVal = ( static_cast<float>(sampleVal) * (1.0f / 4095.0f) * 2.0f ) - 1.0f;
	}

	// soft clip
	tempVal = ( 1.5f * tempVal ) - ( 0.5f * tempVal * tempVal * tempVal );
	tempVal = 0.5f * ( abs(sampleVal + tempVal) - abs(sampleVal - tempVal) ); // restricts to -1.0f to 1.0f

	if (std::is_same<T, uint16_t>::value)
	{
		sampleVal = static_cast<uint16_t>( (tempVal + 1.0f) * 4095.0f );
	}
	else
	{
		sampleVal = tempVal;
	}

	return sampleVal;
}

// avoid linker errors
template class SoftClipper<float>;
template class SoftClipper<uint16_t>;
