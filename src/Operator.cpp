#include "Operator.hpp"

#include "AudioConstants.hpp"
#include "MidiConstants.hpp"
#include "WaveBuffer.hpp"
#include "IEnvelopeGenerator.hpp"

Operator::Operator(IOscillator* wave, IEnvelopeGenerator* eg, IFilter* filt, float amplitude, float frequency) :
	m_Osc(wave),
	m_EG(eg),
	m_Filter(filt),
	m_FilterCenterFreq(20000.0f),
	m_UseRatio(false),
	m_ModSources(),
	m_EGModDestinations(),
	m_Amplitude(amplitude),
	m_Frequency(frequency),
	m_Detune(0),
	m_Ratio(1.0f),
	m_RatioFrequency(frequency),
	m_CurrentValue(0.0f),
	m_AmpVelSens(0.0f),
	m_FiltVelSens(0.0f),
	m_FrequencyOffset(0.0f),
	m_GlideFrequency(0.0f),
	m_GlideTime(0.0f),
	m_GlideNumSamples(1),
	m_GlideIncr(0.0f),
	m_UseGlideRetrigger(true),
	m_UseGlide(false)
{
}

Operator::~Operator()
{
}

float Operator::nextSample()
{
	if (m_Osc)
	{
		float egValue = 1.0f;
		if (m_EG)
		{
			egValue = m_EG->nextValue();
		}

		float currentValAmplitude = 1.0f - (m_AmpVelSens * m_CurrentVelocity);

		if (m_EGModDestinations.count(EGModDestination::AMPLITUDE))
		{
			currentValAmplitude *= egValue;
		}

		if ( (m_GlideIncr >= 0.0f && m_GlideFrequency < m_RatioFrequency) || (m_GlideIncr <  0.0f && m_GlideFrequency > m_RatioFrequency) )
		{
			m_GlideFrequency += m_GlideIncr;
		}

		float frequency = (m_UseRatio) ? m_GlideFrequency : m_Frequency;
		frequency += m_FrequencyOffset;

		if (m_EGModDestinations.count(EGModDestination::FREQUENCY))
		{
			frequency = (frequency * egValue) + 1.0f;
		}

		for (IModulationSource* modSource : m_ModSources)
		{
			std::map<IModulationSource*, float>::iterator amplitude = m_ModAmplitudes.find(modSource);
			if (amplitude != m_ModAmplitudes.end())
			{
				frequency += ((*amplitude).second * modSource->currentValue());
			}
		}

		m_Osc->setFrequency( frequency );
		m_CurrentValue = (m_Osc->nextSample()) * currentValAmplitude;

		if (m_Filter)
		{
			float frequency = m_FilterCenterFreq;
			frequency *= ( 1.0f - (m_FiltVelSens * m_CurrentVelocity) );

			if (m_EGModDestinations.count(EGModDestination::FILT_FREQUENCY))
			{
				frequency *= egValue;
			}

			m_Filter->setCoefficients(frequency);
			m_CurrentValue = m_Filter->processSample(m_CurrentValue);
		}

		return m_CurrentValue * m_Amplitude;
	}

	return 0.0f;
}

float Operator::currentValue()
{
	return m_CurrentValue;
}

void Operator::onKeyEvent(const KeyEvent& keyEvent)
{
	if (keyEvent.pressed() == KeyPressedEnum::PRESSED)
	{
		m_CurrentVelocity = 1.0f - ( (float)keyEvent.velocity() / 127.0f );
	}

	if (m_EG)
	{
		m_EG->onKeyEvent(keyEvent);
	}

	if (m_UseRatio)
	{
		m_RatioFrequency = 0.0f;

		switch ( keyEvent.note() )
		{
			case MIDI_NOTE_A0:
				m_RatioFrequency = MUSIC_A0;
				break;
			case MIDI_NOTE_BB0:
				m_RatioFrequency = MUSIC_BB0;
				break;
			case MIDI_NOTE_B0:
				m_RatioFrequency = MUSIC_B0;
				break;
			case MIDI_NOTE_C1:
				m_RatioFrequency = MUSIC_C1;
				break;
			case MIDI_NOTE_DB1:
				m_RatioFrequency = MUSIC_DB1;
				break;
			case MIDI_NOTE_D1:
				m_RatioFrequency = MUSIC_D1;
				break;
			case MIDI_NOTE_EB1:
				m_RatioFrequency = MUSIC_EB1;
				break;
			case MIDI_NOTE_E1:
				m_RatioFrequency = MUSIC_E1;
				break;
			case MIDI_NOTE_F1:
				m_RatioFrequency = MUSIC_F1;
				break;
			case MIDI_NOTE_GB1:
				m_RatioFrequency = MUSIC_GB1;
				break;
			case MIDI_NOTE_G1:
				m_RatioFrequency = MUSIC_G1;
				break;
			case MIDI_NOTE_AB1:
				m_RatioFrequency = MUSIC_AB1;
				break;
			case MIDI_NOTE_A1:
				m_RatioFrequency = MUSIC_A1;
				break;
			case MIDI_NOTE_BB1:
				m_RatioFrequency = MUSIC_BB1;
				break;
			case MIDI_NOTE_B1:
				m_RatioFrequency = MUSIC_B1;
				break;
			case MIDI_NOTE_C2:
				m_RatioFrequency = MUSIC_C2;
				break;
			case MIDI_NOTE_DB2:
				m_RatioFrequency = MUSIC_DB2;
				break;
			case MIDI_NOTE_D2:
				m_RatioFrequency = MUSIC_D2;
				break;
			case MIDI_NOTE_EB2:
				m_RatioFrequency = MUSIC_EB2;
				break;
			case MIDI_NOTE_E2:
				m_RatioFrequency = MUSIC_E2;
				break;
			case MIDI_NOTE_F2:
				m_RatioFrequency = MUSIC_F2;
				break;
			case MIDI_NOTE_GB2:
				m_RatioFrequency = MUSIC_GB2;
				break;
			case MIDI_NOTE_G2:
				m_RatioFrequency = MUSIC_G2;
				break;
			case MIDI_NOTE_AB2:
				m_RatioFrequency = MUSIC_AB2;
				break;
			case MIDI_NOTE_A2:
				m_RatioFrequency = MUSIC_A2;
				break;
			case MIDI_NOTE_BB2:
				m_RatioFrequency = MUSIC_BB2;
				break;
			case MIDI_NOTE_B2:
				m_RatioFrequency = MUSIC_B2;
				break;
			case MIDI_NOTE_C3:
				m_RatioFrequency = MUSIC_C3;
				break;
			case MIDI_NOTE_DB3:
				m_RatioFrequency = MUSIC_DB3;
				break;
			case MIDI_NOTE_D3:
				m_RatioFrequency = MUSIC_D3;
				break;
			case MIDI_NOTE_EB3:
				m_RatioFrequency = MUSIC_EB3;
				break;
			case MIDI_NOTE_E3:
				m_RatioFrequency = MUSIC_E3;
				break;
			case MIDI_NOTE_F3:
				m_RatioFrequency = MUSIC_F3;
				break;
			case MIDI_NOTE_GB3:
				m_RatioFrequency = MUSIC_GB3;
				break;
			case MIDI_NOTE_G3:
				m_RatioFrequency = MUSIC_G3;
				break;
			case MIDI_NOTE_AB3:
				m_RatioFrequency = MUSIC_AB3;
				break;
			case MIDI_NOTE_A3:
				m_RatioFrequency = MUSIC_A3;
				break;
			case MIDI_NOTE_BB3:
				m_RatioFrequency = MUSIC_BB3;
				break;
			case MIDI_NOTE_B3:
				m_RatioFrequency = MUSIC_B3;
				break;
			case MIDI_NOTE_C4:
				m_RatioFrequency = MUSIC_C4;
				break;
			case MIDI_NOTE_DB4:
				m_RatioFrequency = MUSIC_DB4;
				break;
			case MIDI_NOTE_D4:
				m_RatioFrequency = MUSIC_D4;
				break;
			case MIDI_NOTE_EB4:
				m_RatioFrequency = MUSIC_EB4;
				break;
			case MIDI_NOTE_E4:
				m_RatioFrequency = MUSIC_E4;
				break;
			case MIDI_NOTE_F4:
				m_RatioFrequency = MUSIC_F4;
				break;
			case MIDI_NOTE_GB4:
				m_RatioFrequency = MUSIC_GB4;
				break;
			case MIDI_NOTE_G4:
				m_RatioFrequency = MUSIC_G4;
				break;
			case MIDI_NOTE_AB4:
				m_RatioFrequency = MUSIC_AB4;
				break;
			case MIDI_NOTE_A4:
				m_RatioFrequency = MUSIC_A4;
				break;
			case MIDI_NOTE_BB4:
				m_RatioFrequency = MUSIC_BB4;
				break;
			case MIDI_NOTE_B4:
				m_RatioFrequency = MUSIC_B4;
				break;
			case MIDI_NOTE_C5:
				m_RatioFrequency = MUSIC_C5;
				break;
			case MIDI_NOTE_DB5:
				m_RatioFrequency = MUSIC_DB5;
				break;
			case MIDI_NOTE_D5:
				m_RatioFrequency = MUSIC_D5;
				break;
			case MIDI_NOTE_EB5:
				m_RatioFrequency = MUSIC_EB5;
				break;
			case MIDI_NOTE_E5:
				m_RatioFrequency = MUSIC_E5;
				break;
			case MIDI_NOTE_F5:
				m_RatioFrequency = MUSIC_F5;
				break;
			case MIDI_NOTE_GB5:
				m_RatioFrequency = MUSIC_GB5;
				break;
			case MIDI_NOTE_G5:
				m_RatioFrequency = MUSIC_G5;
				break;
			case MIDI_NOTE_AB5:
				m_RatioFrequency = MUSIC_AB5;
				break;
			case MIDI_NOTE_A5:
				m_RatioFrequency = MUSIC_A5;
				break;
			case MIDI_NOTE_BB5:
				m_RatioFrequency = MUSIC_BB5;
				break;
			case MIDI_NOTE_B5:
				m_RatioFrequency = MUSIC_B5;
				break;
			case MIDI_NOTE_C6:
				m_RatioFrequency = MUSIC_C6;
				break;
			case MIDI_NOTE_DB6:
				m_RatioFrequency = MUSIC_DB6;
				break;
			case MIDI_NOTE_D6:
				m_RatioFrequency = MUSIC_D6;
				break;
			case MIDI_NOTE_EB6:
				m_RatioFrequency = MUSIC_EB6;
				break;
			case MIDI_NOTE_E6:
				m_RatioFrequency = MUSIC_E6;
				break;
			case MIDI_NOTE_F6:
				m_RatioFrequency = MUSIC_F6;
				break;
			case MIDI_NOTE_GB6:
				m_RatioFrequency = MUSIC_GB6;
				break;
			case MIDI_NOTE_G6:
				m_RatioFrequency = MUSIC_G6;
				break;
			case MIDI_NOTE_AB6:
				m_RatioFrequency = MUSIC_AB6;
				break;
			case MIDI_NOTE_A6:
				m_RatioFrequency = MUSIC_A6;
				break;
			case MIDI_NOTE_BB6:
				m_RatioFrequency = MUSIC_BB6;
				break;
			case MIDI_NOTE_B6:
				m_RatioFrequency = MUSIC_B6;
				break;
			case MIDI_NOTE_C7:
				m_RatioFrequency = MUSIC_C7;
				break;
			case MIDI_NOTE_DB7:
				m_RatioFrequency = MUSIC_DB7;
				break;
			case MIDI_NOTE_D7:
				m_RatioFrequency = MUSIC_D7;
				break;
			case MIDI_NOTE_EB7:
				m_RatioFrequency = MUSIC_EB7;
				break;
			case MIDI_NOTE_E7:
				m_RatioFrequency = MUSIC_E7;
				break;
			case MIDI_NOTE_F7:
				m_RatioFrequency = MUSIC_F7;
				break;
			case MIDI_NOTE_GB7:
				m_RatioFrequency = MUSIC_GB7;
				break;
			case MIDI_NOTE_G7:
				m_RatioFrequency = MUSIC_G7;
				break;
			case MIDI_NOTE_AB7:
				m_RatioFrequency = MUSIC_AB7;
				break;
			case MIDI_NOTE_A7:
				m_RatioFrequency = MUSIC_A7;
				break;
			case MIDI_NOTE_BB7:
				m_RatioFrequency = MUSIC_BB7;
				break;
			case MIDI_NOTE_B7:
				m_RatioFrequency = MUSIC_B7;
				break;
			case MIDI_NOTE_C8:
				m_RatioFrequency = MUSIC_C8;
				break;
			case MIDI_NOTE_DB8:
				m_RatioFrequency = MUSIC_DB8;
				break;
			case MIDI_NOTE_D8:
				m_RatioFrequency = MUSIC_D8;
				break;
			case MIDI_NOTE_EB8:
				m_RatioFrequency = MUSIC_EB8;
				break;
			case MIDI_NOTE_E8:
				m_RatioFrequency = MUSIC_E8;
				break;
			case MIDI_NOTE_F8:
				m_RatioFrequency = MUSIC_F8;
				break;
			case MIDI_NOTE_GB8:
				m_RatioFrequency = MUSIC_GB8;
				break;
			case MIDI_NOTE_G8:
				m_RatioFrequency = MUSIC_G8;
				break;
			case MIDI_NOTE_AB8:
				m_RatioFrequency = MUSIC_AB8;
				break;
			case MIDI_NOTE_A8:
				m_RatioFrequency = MUSIC_A8;
				break;
			case MIDI_NOTE_BB8:
				m_RatioFrequency = MUSIC_BB8;
				break;
			case MIDI_NOTE_B8:
				m_RatioFrequency = MUSIC_B8;
				break;
			case MIDI_NOTE_C9:
				m_RatioFrequency = MUSIC_C9;
				break;
			case MIDI_NOTE_DB9:
				m_RatioFrequency = MUSIC_DB9;
				break;
			case MIDI_NOTE_D9:
				m_RatioFrequency = MUSIC_D9;
				break;
			case MIDI_NOTE_EB9:
				m_RatioFrequency = MUSIC_E9;
				break;
			case MIDI_NOTE_E9:
				m_RatioFrequency = MUSIC_E9;
				break;
			case MIDI_NOTE_F9:
				m_RatioFrequency = MUSIC_F9;
				break;
			case MIDI_NOTE_GB9:
				m_RatioFrequency = MUSIC_GB9;
				break;
			case MIDI_NOTE_G9:
				m_RatioFrequency = MUSIC_G9;
				break;
			default:
				m_RatioFrequency = 0.0f;
		}

		m_RatioFrequency *= m_Ratio;
		m_RatioFrequency += m_RatioFrequency * pow( 2.0f, (m_Detune / 1200.0f) );

		if ( !m_UseGlide || (m_UseGlideRetrigger && keyEvent.pressed() == KeyPressedEnum::PRESSED) )
		{
			m_GlideFrequency = m_RatioFrequency;
		}
		else
		{
			m_GlideIncr = (m_RatioFrequency - m_GlideFrequency) / m_GlideNumSamples;
		}
	}
}

void Operator::onPitchEvent (const PitchEvent& pitchEvent)
{
	this->setFrequencyOffset( (m_RatioFrequency * pitchEvent.getPitchFactor()) - m_RatioFrequency );
}

OscillatorMode Operator::getWave()
{
	return m_Osc->getOscillatorMode();
}

void Operator::setWave (const OscillatorMode& wave)
{
	m_Osc->setOscillatorMode( wave );
}

IEnvelopeGenerator* Operator::getEnvelopeGenerator()
{
	return m_EG;
}

void Operator::setEnvelopeGenerator (IEnvelopeGenerator* eg)
{
	if (eg)
	{
		m_EG = eg;
	}
}

void Operator::setModSourceAmplitude (IModulationSource* modSource, float amplitude)
{
	m_ModSources.insert(modSource);

	if (!m_ModAmplitudes.count(modSource))
	{
		m_ModAmplitudes.insert(std::pair<IModulationSource*, float>(modSource, amplitude));
	}
	else
	{
		m_ModAmplitudes[modSource] = amplitude;
	}
}

void Operator::setEGModDestination (const EGModDestination& modDest, const bool on)
{
	if (on)
	{
		m_EGModDestinations.insert(modDest);
	}
	else
	{
		m_EGModDestinations.erase(modDest);
	}
}

void Operator::unsetEGModDestination (const EGModDestination& modDest)
{
	m_EGModDestinations.erase(modDest);
}

void Operator::setFrequency (const float frequency)
{
	m_Frequency = frequency;

	if (m_Frequency > 10000.0f)
	{
		m_Ratio = 5.0f;
	}
	else if (m_Frequency > 5000.0f)
	{
		m_Ratio = 4.0f;
	}
	else if (m_Frequency > 2500.0f)
	{
		m_Ratio = 3.0f;
	}
	else if (m_Frequency > 1500.0f)
	{
		m_Ratio = 2.0f;
	}
	else if (m_Frequency > 1000.0f)
	{
		m_Ratio = 1.0f;
	}
	else if (m_Frequency > 500.0f)
	{
		m_Ratio = 0.5f;
	}
	else
	{
		m_Ratio = 0.25f;
	}
}

void Operator::setDetune (const int cents)
{
	// TODO this should actually just set the m_Detune value to the amount in cents, then the keyEvent should set the actual val
	m_Detune = cents; // m_Frequency * pow( 2.0f, (cents/1200.0f) );
}

void Operator::setAmplitude (const float amplitude)
{
	m_Amplitude = amplitude;
}

void Operator::setFilterFreq (const float frequency)
{
	m_FilterCenterFreq = frequency;
}

void Operator::setFilterRes (const float resonance)
{
	m_Filter->setResonance(resonance);
}

void Operator::setRatio (const bool useRatio)
{
	m_UseRatio = useRatio;
}

void Operator::setAmpVelSens (const float ampVelSens)
{
	m_AmpVelSens = ampVelSens;
}

void Operator::setFiltVelSens (const float filtVelSens)
{
	m_FiltVelSens = filtVelSens;
}

void Operator::setFrequencyOffset (const float freqOffset)
{
	m_FrequencyOffset = freqOffset;
}

void Operator::setGlideTime (const float glideTime)
{
	m_GlideTime = glideTime;

	if (glideTime <= 0.0f)
	{
		m_GlideNumSamples = 1;
	}
	else
	{
		m_GlideNumSamples = static_cast<unsigned int>( static_cast<float>(SAMPLE_RATE) * glideTime ) + 1;
	}
}

void Operator::setGlideRetrigger (const bool useRetrigger)
{
	m_UseGlideRetrigger = useRetrigger;
}

void Operator::setUseGlide (const bool useGlide)
{
	m_UseGlide = useGlide;
}
