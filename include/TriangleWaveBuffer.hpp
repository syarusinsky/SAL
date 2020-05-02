#ifndef TRIANGLEWAVEBUFFER_HPP
#define TRIANGLEWAVEBUFFER_HPP

/*************************************************************************
 * A TriangleWaveBuffer stores a memory block corresponding to a
 * triangle wave at the frequency given by the sample rate and buffer
 * frequency in the audio constants file.
*************************************************************************/

#include "WaveBuffer.hpp"

class TriangleWaveBuffer : public WaveBuffer
{
	public:
		TriangleWaveBuffer();
		~TriangleWaveBuffer() override;
};

#endif // TRIANGLEWAVEBUFFER_HPP
