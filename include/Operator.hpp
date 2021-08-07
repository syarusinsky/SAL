#ifndef OPERATOR_HPP
#define OPERATOR_HPP

/*******************************************************************************
 * An Operator describes an oscillator who's frequency can be modulated by
 * other IModulationSources. It is an IModulationSource itself, so Operators
 * can modulate each other. Operators also have an IEnvelopeGenerator which
 * can provide additional modulation to amplitude and frequency.
*******************************************************************************/

#include <set>
#include <map>

#include "IModulationSource.hpp"
#include "IFilter.hpp"
#include "IKeyEventListener.hpp"
#include "IPitchEventListener.hpp"

class IOscillator;
enum class OscillatorMode : unsigned int;
class IEnvelopeGenerator;

enum class EGModDestination : unsigned int
{
	AMPLITUDE,
	FREQUENCY,
	FILT_FREQUENCY
};

class Operator : public IModulationSource, public IKeyEventListener, public IPitchEventListener
{
	public:
		Operator (IOscillator* osc, IEnvelopeGenerator* eg, IFilter<float>* filt, float amplitude, float frequency);
		~Operator() override;

		float nextSample();
		float currentValue() override;

		void onKeyEvent (const KeyEvent& keyEvent) override;

		void onPitchEvent (const PitchEvent& pitchEvent) override;

		OscillatorMode getWave();
		void setWave (const OscillatorMode& wave);
		IEnvelopeGenerator* getEnvelopeGenerator();
		void setEnvelopeGenerator (IEnvelopeGenerator* eg);
		void setModSourceAmplitude (IModulationSource* modSource, float amplitude = 1.0f);
		void setEGModDestination (const EGModDestination& modDest, const bool on);
		void unsetEGModDestination (const EGModDestination& modDest);
		void setFrequency (const float frequency);
		void setDetune (const int cents);
		void setEGFreqModAmount (const float modAmount);
		void setAmplitude (const float amplitude);
		void setFilterFreq (const float frequency);
		void setFilterRes (const float resonance);
		void setRatio (const bool useRatio);
		void setAmpVelSens (const float ampVelSens);
		void setFiltVelSens (const float filtVelSens);
		void setFrequencyOffset (const float freqOffset);
		void setGlideTime (const float glideTime);
		void setGlideRetrigger (const bool useRetrigger);
		void setUseGlide (const bool useGlide);

		float getFrequency() { return m_Frequency; }
		int getDetune() { return m_Detune; }
		bool egModAmplitudeSet() { if (m_EGModDestinations.count(EGModDestination::AMPLITUDE)) { return true; } return false; }
		bool egModFrequencySet() { if (m_EGModDestinations.count(EGModDestination::FREQUENCY)) { return true; } return false; }
		bool egModFilterSet(){ if (m_EGModDestinations.count(EGModDestination::FILT_FREQUENCY)){ return true; } return false; }
		float getModulationAmount (IModulationSource* modSource)
		{
			if (m_ModAmplitudes.count(modSource))
			{
				return m_ModAmplitudes.at(modSource);
			}

			return 0.0f;
		}
		float getAmplitude() { return m_Amplitude; }
		float getFilterFreq() { return m_FilterCenterFreq; }
		float getFilterRes() { return m_Filter->getResonance(); }
		bool getRatio() { return m_UseRatio; }
		float getAmpVelSens() { return m_AmpVelSens; }
		float getFiltVelSens() { return m_FiltVelSens; }
		float getRatioFrequency() { return m_RatioFrequency; }
		float getGlideTime() { return m_GlideTime; }
		bool getGlideRetrigger() { return m_UseGlideRetrigger; }
		bool getUseGlide() { return m_UseGlide; }

	private:
		IOscillator* m_Osc;
		IEnvelopeGenerator* m_EG;
		IFilter<float>* m_Filter;
		float m_FilterCenterFreq;
		bool m_UseRatio;
		std::set<IModulationSource*> m_ModSources;
		std::map<IModulationSource*, float> m_ModAmplitudes;
		std::set<EGModDestination> m_EGModDestinations;
		float         m_Amplitude;
		float         m_Frequency;
		int           m_Detune;
		float         m_Ratio;
		float         m_RatioFrequency;
		float         m_CurrentValue;
		float         m_AmpVelSens;
		float         m_FiltVelSens;
		float         m_CurrentVelocity;
		float         m_FrequencyOffset;
		float         m_GlideFrequency;
		float         m_GlideTime;
		unsigned int  m_GlideNumSamples;
		float         m_GlideIncr;
		bool          m_UseGlideRetrigger;
		bool          m_UseGlide;
};

#endif // OPERATOR_HPP
