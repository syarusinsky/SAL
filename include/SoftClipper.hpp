#ifndef SOFTCLIPPER_HPP
#define SOFTCLIPPER_HPP

#include "IBufferCallback.hpp"

#include <stdint.h>
#include <type_traits>
#include <math.h>


/*************************************************************************
 * A SoftClipper takes in a sample value and clips it in a manner
 * that resembles analog saturation.
 *
 * It can be used with uint16_t, int16_t and float types and clips between
 * 0 and 4095 (for uint16_t for the 12 bit adcs I'm using), half of that
 * signed for int16_t, and -1.0f and 1.0f for float.
*************************************************************************/

template <typename T, bool use12Bit = false>
class SoftClipper : public IBufferCallback<T>
{
	public:
		SoftClipper() {}
		~SoftClipper() {}

		inline T processSample (T sampleVal)
		{
			constexpr float peakVal = ( use12Bit ) ? 4095.0f : 65535.0f;
			constexpr float halfVal = ( use12Bit ) ? 2047.0f : 32767.0f;
			constexpr unsigned int halfValUInt = ( use12Bit ) ? 2047 : 32767;

			float tempVal = sampleVal;
			if constexpr ( std::is_same<T, uint16_t>::value )
			{
				// convert to float between -1.0f and 1.0f
				tempVal = ( static_cast<float>(sampleVal) * (1.0f / peakVal) * 2.0f ) - 1.0f;
			}
			else if constexpr ( std::is_same<T, int16_t>::value )
			{
				// convert to float between -1.0f and 1.0f
				sampleVal += halfValUInt;
				tempVal = ( static_cast<float>(sampleVal) * (1.0f / peakVal) * 2.0f ) - 1.0f;
			}

			// soft clip
			tempVal = ( 1.5f * tempVal ) - ( 0.5f * tempVal * tempVal * tempVal );
			tempVal = 0.5f * ( std::abs(tempVal + 0.99f) - std::abs(tempVal - 0.99f) ); // restricts to -1.0f to 1.0f

			if constexpr ( std::is_same<T, uint16_t>::value )
			{
				sampleVal = static_cast<uint16_t>( (tempVal + 1.0f) * peakVal );
			}
			else if constexpr ( std::is_same<T, int16_t>::value )
			{
				sampleVal = static_cast<uint16_t>( tempVal * halfVal );
			}
			else
			{
				sampleVal = tempVal;
			}

			return sampleVal;
		}

		void call (T* writeBuffer) override;

	private:
};

#endif // SOFTCLIPPER_HPP
