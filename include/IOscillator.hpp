#ifndef IOSCILLATOR_HPP
#define IOSCILLATOR_HPP

enum class OscillatorMode : unsigned int
{
	SINE,
	TRIANGLE,
	SQUARE,
	SAWTOOTH
};

class IOscillator
{
	public:
		virtual ~IOscillator(){}

		virtual float nextSample() = 0;
		virtual void setFrequency (float frequency) = 0;

		virtual void setOscillatorMode (const OscillatorMode& mode) = 0;
		virtual OscillatorMode getOscillatorMode() = 0;

		virtual void resetPhase() = 0;
};

#endif // IOSCILLATOR_HPP
