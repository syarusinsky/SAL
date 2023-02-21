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

template <typename T>
class SoftClipper : public IBufferCallback<T>
{
	public:
		SoftClipper() {}
		~SoftClipper() {}

		inline T processSample (T sampleVal)
		{

			float tempVal = sampleVal;
			if constexpr ( std::is_same<T, uint16_t>::value )
			{
				// convert to float between -1.0f and 1.0f
				tempVal = ( static_cast<float>(sampleVal) * (1.0f / 4095.0f) * 2.0f ) - 1.0f;
			}
			else if constexpr ( std::is_same<T, int16_t>::value )
			{
				// convert to float between -1.0f and 1.0f
				sampleVal += 2047;
				tempVal = ( static_cast<float>(sampleVal) * (1.0f / 4095.0f) * 2.0f ) - 1.0f;
			}

			// soft clip
			tempVal = ( 1.5f * tempVal ) - ( 0.5f * tempVal * tempVal * tempVal );
			tempVal = 0.5f * ( std::abs(tempVal + 0.99f) - std::abs(tempVal - 0.99f) ); // restricts to -1.0f to 1.0f

			if constexpr ( std::is_same<T, uint16_t>::value )
			{
				sampleVal = static_cast<uint16_t>( (tempVal + 1.0f) * 4095.0f );
			}
			else if constexpr ( std::is_same<T, int16_t>::value )
			{
				sampleVal = static_cast<uint16_t>( tempVal * 2048.0f );
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
