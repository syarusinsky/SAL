#ifndef ANTIALIASINGFILTER_HPP
#define ANTIALIASINGFILTER_HPP

/*******************************************************************************
 * A simple anti-aliasing filter implemented by a low-pass FIR filter with a
 * sinc function windowed by a hamming window. FIR filter is used for linear
 * phase properties.
 *
 * Note: Since this filter is mostly intended to be used in resampling sources
 * both inside and outside of this library, it is not implemented as an
 * IBufferCallback. Instead, components such as SampleRateConverter use it as
 * a utility.
*******************************************************************************/

#include <vector>

template <typename T>
class AntiAliasingFilter
{
	public:
		AntiAliasingFilter (const float cutoffFreq, const unsigned int sampleRate, const unsigned int filterOrder,
					const unsigned int bufferSize);

		void call (T* const buffer);

		void changeValues (const float cutoffFreq, const unsigned int sampleRate, const unsigned int filterOrder,
					const unsigned int bufferSize);

	private:
		float 			m_CutoffFreq;
		unsigned int 		m_SampleRate;
		unsigned int 		m_FilterOrder;
		std::vector<float> 	m_Coefficients;
		unsigned int 		m_BufferSize;
		std::vector<float> 	m_WorkingBuffer; 	// this is a circular buffer that the input buffer of call is copied into
		unsigned int 		m_WorkingBufferIncr;

		std::vector<float> calculateCoefficients();

		T getZeroPoint();
};

#endif // ANTIALIASINGFILTER_HPP
