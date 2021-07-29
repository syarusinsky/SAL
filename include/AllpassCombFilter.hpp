#ifndef ALLPASSCOMBFILTER_HPP
#define ALLPASSCOMBFILTER_HPP

/*****************************************************************
 * An AllpassCombFilter describes a comb filter that uses one
 * delay line in a feedfoward and feedback configuration. The
 * delay line length in sample as well as the feedback gain can
 * be set.
*****************************************************************/

template <typename T>
class AllpassCombFilter
{
	public:
		AllpassCombFilter (unsigned int delayLength, float feedbackGain, T initVal); // initVal is the value to initialize the delay buffer with
		~AllpassCombFilter();

		T processSample (T sampleVal);

		void setFeedbackGain (float feedbackGain);

	private:
		unsigned int 	m_DelayLength;
		T* 		m_DelayBuffer;
		unsigned int 	m_DelayWriteIncr;
		unsigned int 	m_DelayReadIncr;

		float 		m_FeedbackGain;
};

#endif // ALLPASSCOMBFILTER_HPP
