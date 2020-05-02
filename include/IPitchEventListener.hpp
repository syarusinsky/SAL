#ifndef IPITCHEVENTLISTENER_HPP
#define IPITCHEVENTLISTENER_HPP

/*******************************************************************
 * An IPitchEventListener specifies a simple interface which
 * a subclass can use to be notified of pitch events.
 *
 * The pitch factor is a number to multiply a frequency by to get
 * a 'pitch bent' frequency.
*******************************************************************/

#include "IEventListener.hpp"

class PitchEvent : IEvent
{
	public:
		PitchEvent (float pitchFactor);
		~PitchEvent() override;

		float getPitchFactor() const;
		void setPitchFactor (float pitchFactor);

	private:
		float m_PitchFactor;
};

class IPitchEventListener : public IEventListener
{
	public:
		virtual ~IPitchEventListener();

		virtual void onPitchEvent (const PitchEvent& pitchEvent) = 0;

		void bindToPitchEventSystem();
		void unbindFromPitchEventSystem();

		static void PublishEvent (const PitchEvent& pitchEvent);

	private:
		static EventDispatcher<IPitchEventListener, PitchEvent, &IPitchEventListener::onPitchEvent> m_EventDispatcher;
};

#endif // IPITCHEVENTLISTENER_HPP
