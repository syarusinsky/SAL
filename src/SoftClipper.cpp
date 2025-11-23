#include "SoftClipper.hpp"

#include "AudioConstants.hpp"


template <typename T, bool use12Bit>
void SoftClipper<T, use12Bit>::call (T* writeBuffer)
{
	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		writeBuffer[sample] = this->processSample( writeBuffer[sample] );
	}
}

// avoid linker errors
template class SoftClipper<float>;
template class SoftClipper<uint16_t>;
template class SoftClipper<int16_t>;
