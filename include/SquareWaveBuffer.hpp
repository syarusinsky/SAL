#ifndef SQUAREWAVEBUFFER_HPP
#define SQUAREWAVEBUFFER_HPP

/*************************************************************************
 * A SquareWaveBuffer stores a memory block corresponding to a square
 * wave at the frequency given by the sample rate and buffer frequency
 * in the audio constants file.
*************************************************************************/

#include "WaveBuffer.hpp"

class SquareWaveBuffer : public WaveBuffer
{
	public:
		SquareWaveBuffer();
		~SquareWaveBuffer() override;
};

#endif
