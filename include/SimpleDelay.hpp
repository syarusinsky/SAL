#ifndef SIMPLEDELAY_HPP
#define SIMPLEDELAY_HPP

/*****************************************************************
 * A SimpleDelay describes a simple delay line whose processSample
 * function writes a sample to the delay line while returning a
 * sample stored at the tapped index. The tap index can also be
 * set with the setDelayLength function as long as the given delay
 * length is within the maximum delay length.
*****************************************************************/

#include "IBufferCallback.hpp"

template <typename T>
class SimpleDelay : public IBufferCallback<T>
{
	public:
		// initVal is the value to initialize the delay buffer with
		SimpleDelay (unsigned int maxDelayLength, unsigned int delayLength, T initVal);
		~SimpleDelay();

		T processSample (T sampleVal);

		void setDelayLength (unsigned int delayLength); // must be within maximum delay length defined in constructor

		void call (T* writeBuffer) override;

	private:
		unsigned int 	m_DelayLength;
		T* 		m_DelayBuffer;
		unsigned int 	m_DelayWriteIncr;
		unsigned int 	m_DelayReadIncr;

		inline T processSampleHelper (T sampleVal);
};

#endif // SIMPLEDELAY_HPP
