#ifndef NOISEGATE_HPP
#define NOISEGATE_HPP

/*****************************************************************
 * A simple noise gate that uses an envelope follower to track
 * the output amplitude of a signal and attenuate it to zero
 * when under the threshold for a given amount of time. The signal
 * must stay under the threshold for the given hold time before
 * the gain starts to attenuate. The gain will be smoothed with
 * the attack and release time.
*****************************************************************/

#include "IBufferCallback.hpp"

template <typename T>
class NoiseGate : public IBufferCallback<T>
{
	public:
		NoiseGate (float attackReleaseTimeMS, float holdTimeMS, T peakThreshold); // attack/release and hold times in ms
		~NoiseGate();

		void call (T* writeBuffer) override;

	private:
		float 	m_AttackReleaseTime;
		float 	m_AttackReleaseCoeff;
		float 	m_HoldTime;
		T 	m_Threshold;

		float 	m_Hold;
		float 	m_Coefficient;
		float 	m_Gain;
};

#endif // NOISEGATE_HPP
