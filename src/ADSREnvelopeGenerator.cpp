#include "ADSREnvelopeGenerator.hpp"

#include "AudioConstants.hpp"
#include "LinearResponse.hpp"
#include "ExponentialResponse.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

template <typename Response>
ADSREnvelopeGenerator<Response>::ADSREnvelopeGenerator (const float atkSec, const float decSec, const float susLvl,
					const float relSec, Response* atkResponse, Response* decResponse,
					Response* relResponse) :
	m_Attack( 0.0f ),
	m_AttackSecs( atkSec ),
	m_Decay( 0.0f ),
	m_DecaySecs( decSec ),
	m_Sustain( 0.0f ),
	m_SustainAtResponse( 0.0f ),
	m_Release( 0.0f ),
	m_ReleaseSecs( relSec ),
	m_Stage( 0 ),
	m_CurrentLvl( 0.0f ),
	m_AttackResponse( atkResponse ),
	m_DecayResponse( decResponse ),
	m_ReleaseResponse( relResponse )
{
	// setup the increment values in the order they're needed
	this->setAttack( m_AttackSecs, m_AttackResponse->getSlope() );
	this->setSustain( susLvl ); // setting sustain level also sets decay and release

	// if atkSec, or any other are = to 0.0f, m_Attack will be inf, so we fix that
	if ( m_Attack  == std::numeric_limits<float>::infinity() || std::isnan(m_Attack) )
		{ m_Attack  = std::numeric_limits<float>::max(); }
	if ( m_Decay   == std::numeric_limits<float>::infinity() || std::isnan(m_Decay) )
		{ m_Decay   = std::numeric_limits<float>::max(); }
	if ( m_Release == std::numeric_limits<float>::infinity() || std::isnan(m_Release) )
		{ m_Release = std::numeric_limits<float>::max(); }

	this->toEnd();
}

template <typename Response>
ADSREnvelopeGenerator<Response>::~ADSREnvelopeGenerator()
{
}

template <typename Response>
float ADSREnvelopeGenerator<Response>::nextValue()
{
	m_SustainAtResponse = m_ReleaseResponse->response( m_Sustain, 0.0f, 1.0f );
	float output = m_SustainAtResponse;

	if ( m_Stage == ATTACK )
	{
		output = m_AttackResponse->response( m_CurrentLvl, 0.0f, 1.0f );
		m_CurrentLvl += m_Attack;
		if ( m_CurrentLvl > 1.0f )
		{
			m_CurrentLvl = 1.0f;
			m_Stage++;
		}
	}

	if ( m_Stage == DECAY )
	{
		output = m_DecayResponse->response( m_CurrentLvl, 0.0f, 1.0f );
		m_CurrentLvl -= m_Decay;
		if ( output < m_SustainAtResponse )
		{
			output = m_SustainAtResponse;
			m_CurrentLvl = m_Sustain;
			m_Stage++;
		}
	}

	if ( m_Stage == RELEASE )
	{
		if ( !m_JustEnteredRelease )
		{
			m_JustEnteredRelease = true;
			// since release depends on m_CurrentLvl, we need to set it again as soon as we reach the release stage
			this->setRelease( m_ReleaseSecs, m_ReleaseResponse->getSlope() );
		}

		if ( m_CurrentLvl <= 0.0f )
		{
			m_CurrentLvl = 0.0f;
			output = 0.0f;
		}
		else
		{
			m_CurrentLvl -= m_Release;
			output = m_ReleaseResponse->response( m_CurrentLvl, 0.0f, 1.0f );
		}
	}

	return output;
}

template <typename Response>
float ADSREnvelopeGenerator<Response>::currentValue()
{
	return m_CurrentLvl;
}

template <typename Response>
void ADSREnvelopeGenerator<Response>::toStart()
{
	m_Stage = ATTACK;
	m_CurrentLvl = 0.0f;
}

template <typename Response>
void ADSREnvelopeGenerator<Response>::toEnd()
{
	m_Stage = RELEASE;
	m_CurrentLvl = 0.0f;
}

template <typename Response>
void ADSREnvelopeGenerator<Response>::onKeyEvent (const KeyEvent& keyEvent)
{
	if ( keyEvent.pressed() == KeyPressedEnum::PRESSED )
	{
		m_Stage = ATTACK;
		m_JustEnteredRelease = false;
	}
	else if ( keyEvent.pressed() == KeyPressedEnum::RELEASED )
	{
		m_Stage = RELEASE;
	}
}

template <typename Response>
void ADSREnvelopeGenerator<Response>::setAttackResponse (Response* response)
{
	m_AttackResponse = response;
}

template <typename Response>
void ADSREnvelopeGenerator<Response>::setDecayResponse (Response* response)
{
	m_DecayResponse = response;
}

template <typename Response>
void ADSREnvelopeGenerator<Response>::setReleaseResponse (Response* response)
{
	m_ReleaseResponse = response;
}

template <typename Response>
void ADSREnvelopeGenerator<Response>::setAttack (float seconds, float expo)
{
	m_AttackSecs = seconds;
	m_Attack = (1.0f / SAMPLE_RATE) / seconds;
	m_AttackResponse->setSlope( expo );

	if ( m_Attack  == std::numeric_limits<float>::infinity() || std::isnan(m_Attack) )
		{ m_Attack  = std::numeric_limits<float>::max(); }
}

template <typename Response>
void ADSREnvelopeGenerator<Response>::setDecay (float seconds, float expo)
{
	m_DecaySecs = seconds;
	m_Decay = ( (1.0f - m_SustainAtResponse) / SAMPLE_RATE ) / seconds;
	m_DecayResponse->setSlope( expo );

	if ( m_Decay   == std::numeric_limits<float>::infinity() || std::isnan(m_Decay) )
		{ m_Decay   = std::numeric_limits<float>::max(); }
}

template <typename Response>
void ADSREnvelopeGenerator<Response>::setSustain (float lvl)
{
	m_Sustain = lvl;
	m_SustainAtResponse = m_ReleaseResponse->response( m_Sustain, 0.0f, 1.0f );

	// since decay depends on sustain at response
	setDecay( m_DecaySecs, m_DecayResponse->getSlope() );
}

template <typename Response>
void ADSREnvelopeGenerator<Response>::setRelease (float seconds, float expo)
{
	m_ReleaseSecs = seconds;
	m_Release = (m_CurrentLvl / SAMPLE_RATE) / seconds;
	m_ReleaseResponse->setSlope( expo );

	if ( m_Release == std::numeric_limits<float>::infinity() || std::isnan(m_Release) )
		{ m_Release = std::numeric_limits<float>::max(); }
}

template <typename Response>
float ADSREnvelopeGenerator<Response>::getAttack()
{
	return m_AttackSecs;
}

template <typename Response>
float ADSREnvelopeGenerator<Response>::getDecay()
{
	return m_DecaySecs;
}

template <typename Response>
float ADSREnvelopeGenerator<Response>::getSustain()
{
	return m_Sustain;
}

template <typename Response>
float ADSREnvelopeGenerator<Response>::getRelease()
{
	return m_ReleaseSecs;
}

template <typename Response>
float ADSREnvelopeGenerator<Response>::getAttackExpo()
{
	return m_AttackResponse->getSlope();
}

template <typename Response>
float ADSREnvelopeGenerator<Response>::getDecayExpo()
{
	return m_DecayResponse->getSlope();
}

template <typename Response>
float ADSREnvelopeGenerator<Response>::getReleaseExpo()
{
	return m_ReleaseResponse->getSlope();
}

// avoid linker errors
template class ADSREnvelopeGenerator<LinearResponse>;
template class ADSREnvelopeGenerator<ExponentialResponse>;
