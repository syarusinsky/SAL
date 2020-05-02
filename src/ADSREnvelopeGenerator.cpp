#include "ADSREnvelopeGenerator.hpp"

#include "IResponse.hpp"
#include "AudioConstants.hpp"
#include <algorithm>
#include <cmath>

ADSREnvelopeGenerator::ADSREnvelopeGenerator (const float atkSec, const float decSec, const float susLvl, const float relSec, IResponse* atkResponse, IResponse* decResponse, IResponse* relResponse) :
	m_Attack(0.0f),
	m_AttackSecs(atkSec),
	m_Decay(0.0f),
	m_DecaySecs(decSec),
	m_Sustain(0.0f),
	m_SustainAtResponse(0.0f),
	m_Release(0.0f),
	m_ReleaseSecs(relSec),
	m_Stage(0),
	m_CurrentLvl(0.0f),
	m_AttackResponse(atkResponse),
	m_DecayResponse(decResponse),
	m_ReleaseResponse(relResponse)
{
	// setup the increment values in the order they're needed
	setAttack(m_AttackSecs, m_AttackResponse->getSlope());
	setSustain(susLvl); // setting sustain level also sets decay and release

	// if atkSec, or any other are = to 0.0f, m_Attack will be inf, so we fix that
	if (m_Attack  == std::numeric_limits<float>::infinity() || std::isnan(m_Attack))
		{ m_Attack  = std::numeric_limits<float>::max(); }
	if (m_Decay   == std::numeric_limits<float>::infinity() || std::isnan(m_Decay))
		{ m_Decay   = std::numeric_limits<float>::max(); }
	if (m_Release == std::numeric_limits<float>::infinity() || std::isnan(m_Release))
		{ m_Release = std::numeric_limits<float>::max(); }

	toEnd();
}

ADSREnvelopeGenerator::~ADSREnvelopeGenerator()
{
}

float ADSREnvelopeGenerator::nextValue()
{
	m_SustainAtResponse = m_ReleaseResponse->response(m_Sustain, 0.0f, 1.0f);
	float output = m_SustainAtResponse;

	if (m_Stage == ATTACK)
	{
		output = m_AttackResponse->response(m_CurrentLvl, 0.0f, 1.0f);
		m_CurrentLvl += m_Attack;
		if (m_CurrentLvl > 1.0f)
		{
			m_CurrentLvl = 1.0f;
			m_Stage++;
		}
	}

	if (m_Stage == DECAY)
	{
		output = m_DecayResponse->response(m_CurrentLvl, 0.0f, 1.0f);
		m_CurrentLvl -= m_Decay;
		if (output < m_SustainAtResponse)
		{
			output = m_SustainAtResponse;
			m_CurrentLvl = m_Sustain;
			m_Stage++;
		}
	}

	if (m_Stage == RELEASE)
	{
		if (!m_JustEnteredRelease)
		{
			m_JustEnteredRelease = true;
			// since release depends on m_CurrentLvl, we need to set it again as soon as we reach the release stage
			setRelease(m_ReleaseSecs, m_ReleaseResponse->getSlope());
		}

		if (m_CurrentLvl <= 0.0f)
		{
			m_CurrentLvl = 0.0f;
			output = 0.0f;
		}
		else
		{
			m_CurrentLvl -= m_Release;
			output = m_ReleaseResponse->response(m_CurrentLvl, 0.0f, 1.0f);
		}
	}

	return output;
}

void ADSREnvelopeGenerator::toStart()
{
	m_Stage = ATTACK;
	m_CurrentLvl = 0.0f;
}

void ADSREnvelopeGenerator::toEnd()
{
	m_Stage = RELEASE;
	m_CurrentLvl = 0.0f;
}

void ADSREnvelopeGenerator::onKeyEvent (const KeyEvent& keyEvent)
{
	if (keyEvent.pressed() == KeyPressedEnum::PRESSED)
	{
		m_Stage = ATTACK;
		m_JustEnteredRelease = false;
	}
	else if (keyEvent.pressed() == KeyPressedEnum::RELEASED)
	{
		m_Stage = RELEASE;
	}
}

void ADSREnvelopeGenerator::setAttackResponse (IResponse* response)
{
	m_AttackResponse = response;
}

void ADSREnvelopeGenerator::setDecayResponse (IResponse* response)
{
	m_DecayResponse = response;
}

void ADSREnvelopeGenerator::setReleaseResponse (IResponse* response)
{
	m_ReleaseResponse = response;
}

void ADSREnvelopeGenerator::setAttack (float seconds, float expo)
{
	m_AttackSecs = seconds;
	m_Attack = (1.0f / SAMPLE_RATE) / seconds;
	m_AttackResponse->setSlope(expo);

	if (m_Attack  == std::numeric_limits<float>::infinity() || std::isnan(m_Attack))
		{ m_Attack  = std::numeric_limits<float>::max(); }
}

void ADSREnvelopeGenerator::setDecay (float seconds, float expo)
{
	m_DecaySecs = seconds;
	m_Decay = ((1.0f - m_SustainAtResponse) / SAMPLE_RATE) / seconds;
	m_DecayResponse->setSlope(expo);

	if (m_Decay   == std::numeric_limits<float>::infinity() || std::isnan(m_Decay))
		{ m_Decay   = std::numeric_limits<float>::max(); }
}

void ADSREnvelopeGenerator::setSustain (float lvl)
{
	m_Sustain = lvl;
	m_SustainAtResponse = m_ReleaseResponse->response(m_Sustain, 0.0f, 1.0f);

	// since decay depends on sustain at response
	setDecay(m_DecaySecs, m_DecayResponse->getSlope());
}

void ADSREnvelopeGenerator::setRelease (float seconds, float expo)
{
	m_ReleaseSecs = seconds;
	m_Release = (m_CurrentLvl / SAMPLE_RATE) / seconds;
	m_ReleaseResponse->setSlope(expo);

	if (m_Release == std::numeric_limits<float>::infinity() || std::isnan(m_Release))
		{ m_Release = std::numeric_limits<float>::max(); }
}

float ADSREnvelopeGenerator::getAttack()
{
	return m_AttackSecs;
}

float ADSREnvelopeGenerator::getDecay()
{
	return m_DecaySecs;
}

float ADSREnvelopeGenerator::getSustain()
{
	return m_Sustain;
}

float ADSREnvelopeGenerator::getRelease()
{
	return m_ReleaseSecs;
}
