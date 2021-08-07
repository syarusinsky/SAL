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
#include "IBufferCallback.hpp"

template <typename T>
class OnePoleFilter : public IFilter<T>, public IBufferCallback<T>
{
	public:
		OnePoleFilter();
		~OnePoleFilter() override;

		T processSample (T sample) override;
		void setCoefficients (float frequency) override;
		void setResonance (float resonance) override {}
		float getResonance() override { return 0.0f; }

		void call (T* writeBuffer) override;

	private:
		float m_A0;
		float m_B1;
		T m_PrevSample;

		inline T processSampleHelper (T sample);
};

#endif // ONEPOLEFILTER_HPP
