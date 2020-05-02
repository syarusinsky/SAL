#ifndef SINEWAVEBUFFER_HPP
#define SINEWAVEBUFFER_HPP

/*************************************************************************
 * A SineWaveBuffer stores a memory block corresponding to a sine wave
 * at the frequency given by the sample rate and buffer frequency in the
 * audio constants file.
*************************************************************************/

#include "WaveBuffer.hpp"

class SineWaveBuffer : public WaveBuffer
{
	public:
		SineWaveBuffer();
		~SineWaveBuffer() override;
};

#endif // SINEWAVEBUFFER_HPP
