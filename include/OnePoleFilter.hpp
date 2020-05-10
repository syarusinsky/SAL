#ifndef ONEPOLEFILTER_HPP
#define ONEPOLEFILTER_HPP

/*****************************************************************
 * A simple one pole filter that processes a single sample.
 * The filter coefficients are set by an input frequency,
 * which means the filter cutoff frequency is tied to the
 * sample rate. This filter is incapable of resonance, so
 * setting does nothing.
*****************************************************************/

#include "IFilter.hpp"

class OnePoleFilter : public IFilter
{
	public:
		OnePoleFilter();
		~OnePoleFilter() override;

		float processSample (float sample) override;
		void setCoefficients (float frequency) override;
		void setResonance (float resonance) override {}
		float getResonance() override { return 0.0f; }

	private:
		float m_A0;
		float m_B1;
		float m_PrevSample;
};

#endif // ONEPOLEFILTER_HPP
