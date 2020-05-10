#ifndef IPOTEVENTLISTENER_HPP
#define IPOTEVENTLISTENER_HPP

/*******************************************************************
 * An IPotEventListener specifies a simple interface which
 * a subclass can use to be notified of potentiometer events.
*******************************************************************/

#include "IEventListener.hpp"

class PotEvent : public IEvent
{
	public:
		PotEvent (float percentage, unsigned int channel);
		~PotEvent() override;

		float getPercentage() const { return m_Percentage; }

	private:
		float m_Percentage;
};

class IPotEventListener : public IEventListener
{
	public:
		virtual ~IPotEventListener();

		virtual void onPotEvent (const PotEvent& potEvent) = 0;

		void bindToPotEventSystem();
		void unbindFromPotEventSystem();

		static void PublishEvent (const PotEvent& potEvent);

	private:
		static EventDispatcher<IPotEventListener, PotEvent, &IPotEventListener::onPotEvent> m_EventDispatcher;
};

#endif // IKEYEVENTLISTENER_HPP
