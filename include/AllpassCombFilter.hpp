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

		T processSample (T sampleVal);

		void setFeedbackGain (float feedbackGain);

		void call (T* writeBuffer) override;

	private:
		unsigned int 	m_DelayLength;
		T* 		m_DelayBuffer;
		unsigned int 	m_DelayWriteIncr;
		unsigned int 	m_DelayReadIncr;

		float 		m_FeedbackGain;

		inline T processSampleHelper (T sampleVal);
};

#endif // ALLPASSCOMBFILTER_HPP
