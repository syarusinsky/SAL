#ifndef SOFTCLIPPER_HPP
#define SOFTCLIPPER_HPP

#include <stdint.h>

/*************************************************************************
 * A SoftClipper takes in a sample value and clips it in a manner
 * that resembles analog saturation.
 *
 * It can be used with uint16_t and float types and clips between 0 and
 * 4095 (for the 12 bit adcs I'm using) and -1.0f and 1.0f respectively.
*************************************************************************/

template <typename T>
class SoftClipper
{
	public:
		SoftClipper() {}
		~SoftClipper() {}

		T processSample (T sampleVal);

	private:
};

#endif // SOFTCLIPPER_HPP
