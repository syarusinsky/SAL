#ifndef IENVELOPEGENERATOR_HPP
#define IENVELOPEGENERATOR_HPP

/***********************************************************
 * An IEnvelopeGenerator specifies an envelope generator,
 * whether that be an ADSR, AR, or any other envelope
 * generator type.
***********************************************************/

#include "IKeyEventListener.hpp"

class IEnvelopeGenerator : public IKeyEventListener
{
	public:
		virtual ~IEnvelopeGenerator() {}
		virtual float nextValue() = 0;
		virtual float currentValue() = 0;
		virtual void toStart() = 0;
		virtual void toEnd() = 0;

		virtual void onKeyEvent (const KeyEvent& keyEvent) override = 0;
};

#endif // IENVELOPEGENERATOR_HPP
