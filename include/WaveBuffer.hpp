#ifndef WAVEBUFFER_HPP
#define WAVEBUFFER_HPP

/************************************************************************
 * A WaveBuffer stores a memory block for a waveform and provides
 * functions for retrieving the next sample in the buffer and for
 * retrieving the buffer itself. When calling nextSample(), the buffer
 * will automatically wrap around to the beginning after reaching the
 * end of the bufffer. Subclasses are intended to fill the buffer with
 * waveforms.
************************************************************************/

#include "AudioConstants.hpp"
#include "IOscillator.hpp"
#include <math.h>
#include <algorithm>

class WaveBuffer : public IOscillator
{
	public:
		WaveBuffer() :
			m_SineBuffer (nullptr),
			m_TriangleBuffer (nullptr),
			m_SquareBuffer (nullptr),
			m_SawtoothBuffer (nullptr),
			m_CurrentBuffer (&m_SineBuffer),
			m_CurrentFrequency (60.0f),
			m_Incr (1.0f),
			m_Pos (0.0f) {}
		virtual ~WaveBuffer() {}

		virtual float* getBuffer() const { return *m_CurrentBuffer; }
		virtual float nextSample()
		{
			// linear interpolation
			unsigned int index1 = (unsigned int) std::max(std::floor(m_Pos), 0.0f);
			unsigned int index2 = (unsigned int) std::min(std::ceil(m_Pos), (float)(WBUFFER_SIZE - 1));
			float sample = ( ((*m_CurrentBuffer)[index1] + (*m_CurrentBuffer)[index2]) / 2.0f );

			m_Pos += m_Incr;
			if (m_Pos >= (float)WBUFFER_SIZE)
			{
				m_Pos = fmod(m_Pos, (float)WBUFFER_SIZE);
			}
			else if (m_Pos < 0.0f)
			{
				m_Pos = (float)WBUFFER_SIZE - fmod(std::abs(m_Pos), (float)WBUFFER_SIZE);
			}

			return sample;
		}

		virtual void setFrequency (const float frequency = 60.0f)
		{
			if (frequency != m_CurrentFrequency)
			{
				m_Incr = (frequency / (float)WBUFFER_FREQ);
				m_CurrentFrequency = frequency;
			}
		}

		virtual void setOscillatorMode (const OscillatorMode& mode) override
		{
			if (mode == OscillatorMode::SINE)
			{
				m_CurrentBuffer = &m_SineBuffer;
			}
			else if (mode == OscillatorMode::TRIANGLE)
			{
				m_CurrentBuffer = &m_TriangleBuffer;
			}
			else if (mode == OscillatorMode::SQUARE)
			{
				m_CurrentBuffer = &m_SquareBuffer;
			}
			else if (mode == OscillatorMode::SAWTOOTH)
			{
				m_CurrentBuffer = &m_SawtoothBuffer;
			}
		}

		virtual OscillatorMode getOscillatorMode() override
		{
			if (m_CurrentBuffer == &m_SineBuffer)
			{
				return OscillatorMode::SINE;
			}
			else if (m_CurrentBuffer == &m_TriangleBuffer)
			{
				return OscillatorMode::TRIANGLE;
			}
			else if (m_CurrentBuffer == &m_SquareBuffer)
			{
				return OscillatorMode::SQUARE;
			}
			else
			{
				return OscillatorMode::SAWTOOTH;
			}
		}

	protected:
		float*  m_SineBuffer;
		float*  m_TriangleBuffer;
		float*  m_SquareBuffer;
		float*  m_SawtoothBuffer;

	private:
		float** m_CurrentBuffer;
		float   m_CurrentFrequency;
		float   m_Incr;
		float   m_Pos;
};

#endif // WAVEBUFFER_HPP
