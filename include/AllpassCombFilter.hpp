#ifndef ALLPASSCOMBFILTER_HPP
#define ALLPASSCOMBFILTER_HPP

/*****************************************************************
 * An AllpassCombFilter describes a comb filter that uses one
 * delay line in a feedfoward and feedback configuration. The
 * delay line length in sample as well as the feedback gain can
 * be set.
*****************************************************************/

#include "IBufferCallback.hpp"

template <typename T>
class AllpassCombFilter : public IBufferCallback<T>
{
	public:
		AllpassCombFilter (unsigned int delayLength, float feedbackGain, T initVal = 0); // initVal is to initialize the delay buffer with
		~AllpassCombFilter();

		inline T processSample (T sampleVal)
		{
			T delayedVal = m_DelayBuffer[m_DelayReadIncr];
			T inputSum = ( sampleVal - (delayedVal * m_FeedbackGain) );
			m_DelayBuffer[m_DelayWriteIncr] = inputSum;

			T outVal = ( (inputSum * m_FeedbackGain) + delayedVal );

			m_DelayWriteIncr = ( m_DelayWriteIncr + 1 ) % m_DelayLength;
			m_DelayReadIncr = ( m_DelayReadIncr + 1 ) % m_DelayLength;

			return outVal;
		}

		void setDelayLength (unsigned int delayLength); // must be less than or equal to initially defined delay length
		void setFeedbackGain (float feedbackGain);

		void call (T* writeBuffer) override;
		// modSource should be an array of ABUFFER_SIZE floats between 0.0f and 1.0f that modulates delay length by numModSamples
		void call (T* writeBuffer, unsigned int numModSamples, float* modSource); // numModSamples must be less than or equal to delayLength

	private:
		unsigned int 	m_DelayLength;
		T* 		m_DelayBuffer;
		unsigned int 	m_DelayWriteIncr;
		unsigned int 	m_DelayReadIncr;

		float 		m_FeedbackGain;
};

#endif // ALLPASSCOMBFILTER_HPP
