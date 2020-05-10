#ifndef ADSRENVELOPEGENERATOR_HPP
#define ADSRENVELOPEGENERATOR_HPP

/*******************************************************************************
 * A simple ADSR envelope generator with controls for attack time, decay time,
 * sustain level, and release time. It also has controls for setting
 * IResponses for attack, decay, and release. The sustain level will use
 * the release IResponse. This envelope generator is also an
 * IKeyEventListener, which allows it to accept RELEASED key events to trigger
 * the release stage.
*******************************************************************************/

#include "IEnvelopeGenerator.hpp"

class IResponse;

enum EGStage
{
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE
};

class ADSREnvelopeGenerator : public IEnvelopeGenerator
{
	public:
		ADSREnvelopeGenerator (const float atkSec, const float decSec, const float susLVL, const float relSec,
					IResponse* atkResponse, IResponse* decResponse, IResponse* relResponse);
		~ADSREnvelopeGenerator() override;

		float nextValue() override;
		void toStart() override;
		void toEnd() override;

		void onKeyEvent (const KeyEvent& keyEvent) override;

		void setAttackResponse (IResponse* response);
		void setDecayResponse (IResponse* response);
		void setReleaseResponse (IResponse* response);
		void setAttack (float seconds, float expo);
		void setDecay (float seconds, float expo);
		void setSustain (float lvl);
		void setRelease (float seconds, float expo);

		float getAttack();
		float getDecay();
		float getSustain();
		float getRelease();

		float getAttackExpo();
		float getDecayExpo();
		float getReleaseExpo();

	private:
		float m_Attack; // incr of attack (0 - 1)
		float m_AttackSecs; // the amount of seconds it takes to complete attack stage
		float m_Decay; // incr of decay (0 - 1)
		float m_DecaySecs; // the amount of seconds it takes to complete the decay stage
		float m_Sustain; // level of sustain (0 - 1)
		float m_SustainAtResponse; // level of sustain at release response
		float m_Release; // incr of release (0 - 1)
		float m_ReleaseSecs; // the amount of seconds it takes to complete the release stage
		unsigned int m_Stage; // 0 for attack, 1 for decay, 2 for sustain, 3 for release
		float m_CurrentLvl;
		bool  m_JustEnteredRelease; // used to signal when to set release (since can happen at any lvl)

		IResponse* m_AttackResponse;
		IResponse* m_DecayResponse;
		IResponse* m_ReleaseResponse;
};

#endif // ADSRENVELOPEGENERATOR_HPP
