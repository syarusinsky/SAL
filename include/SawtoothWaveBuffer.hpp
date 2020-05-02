#ifndef SAWTOOTHWAVEBUFFER_HPP
#define SAWTOOTHWAVEBUFFER_HPP

/*************************************************************************
 * A SawtoothWaveBuffer stores a memory block corresponding to a
 * sawtooth wave at the frequency given by the sample rate and buffer 
 * frequency in the audio constants file.
*************************************************************************/

#include "WaveBuffer.hpp"

class SawtoothWaveBuffer : public WaveBuffer
{
	public:
		SawtoothWaveBuffer();
		~SawtoothWaveBuffer() override;
};

#endif // SAWTOOTHWAVEBUFFER_HPP
