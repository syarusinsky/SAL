#ifndef LIMITER_HPP
#define LIMITER_HPP

/********************************************************************
 * The Limiter class defines a simple limiter that prevents audio
 * from clipping. It uses a circular buffer with a small delay to
 * look ahead for incoming peaks and attentuate them appropriately
 * based on the configuration.
*********************************************************************/

#include "AudioConstants.hpp"
#include "IBufferCallback.hpp"

template <typename T>
class Limiter : public IBufferCallback<T>
{
	public:
		Limiter (float attackTimeMS, float releaseTimeMS, float peakThreshold, float makeupGain); // attack and release times in ms
		~Limiter();

		void call (T* writeBuffer) override;

	private:
		float 		m_AttackTime;
		float 		m_AttackCoeff;
		float 		m_ReleaseTime;
		float 		m_ReleaseCoeff;
		float 		m_Threshold;
		float 		m_MakeupGain;

		float 		m_Peak;
		float 		m_Coefficient;
		float 		m_Gain;

		unsigned int 	m_CircularBufferLength;
		T* 		m_CircularBuffer;
		unsigned int 	m_WriteIndex;
		unsigned int 	m_ReadIndex;
};

#endif // LIMITER_HPP
