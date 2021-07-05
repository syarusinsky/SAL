#ifndef POLYBLEPOSC_HPP
#define POLYBLEPOSC_HPP

/*******************************************************************************
 * A PolyBLEPOsc is a band-limited oscillator that can be used to produce
 * audible oscillations.
*******************************************************************************/

#include "IOscillator.hpp"
#include "IBufferCallback.hpp"

class PolyBLEPOsc : public IOscillator, public IBufferCallback<float>
{
	public:
		PolyBLEPOsc();
		~PolyBLEPOsc() override;

		float nextSample() override;
		void setFrequency (float frequency) override;
		void setOscillatorMode (const OscillatorMode& mode) override;
		OscillatorMode getOscillatorMode() override;

		void call (float* writeBuffer) override;

	private:
		float m_Phase;
		float m_PhaseIncr;
		float m_LastOutput;
		float m_LastLastOutput;
		float m_A0;
		float m_B1;
		OscillatorMode m_OscMode;
};

#endif // POLYBLEPOSC_HPP
