#ifndef IBUTTONEVENTLISTENER_HPP
#define IBUTTONEVENTLISTENER_HPP

/*******************************************************************
 * An IButtonEventListener specifies a simple interface which
 * a subclass can use to be notified of button events.
*******************************************************************/

#include "IEventListener.hpp"

enum class BUTTON_STATE : char
{
	PRESSED,  // when the button is first pressed
	HELD,     // when still pressed after first pressed state
	RELEASED, // when released from pressed or hold state
	FLOATING  // when still released after first released state
};

class ButtonEvent : public IEvent
{
	public:
		ButtonEvent (const BUTTON_STATE& buttonState, unsigned int channel);
		~ButtonEvent() override;

		BUTTON_STATE getButtonState() const { return m_ButtonState; }

	private:
		BUTTON_STATE m_ButtonState;
};

class IButtonEventListener : public IEventListener
{
	public:
		virtual ~IButtonEventListener();

		virtual void onButtonEvent (const ButtonEvent& buttonEvent) = 0;

		void bindToButtonEventSystem();
		void unbindFromButtonEventSystem();

		static void PublishEvent (const ButtonEvent& buttonEvent);

	private:
		static EventDispatcher<IButtonEventListener, ButtonEvent, &IButtonEventListener::onButtonEvent> m_EventDispatcher;
};

#endif // IBUTTONEVENTLISTENER_HPP
