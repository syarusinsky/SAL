#include "IButtonEventListener.hpp"

// instantiating IButtonEventListener's event dispatcher
EventDispatcher<IButtonEventListener, ButtonEvent, &IButtonEventListener::onButtonEvent> IButtonEventListener::m_EventDispatcher;

ButtonEvent::ButtonEvent (const BUTTON_STATE& buttonState, unsigned int channel) :
	IEvent( channel ),
	m_ButtonState( buttonState )
{
}

ButtonEvent::~ButtonEvent()
{
}

IButtonEventListener::~IButtonEventListener()
{
	this->unbindFromButtonEventSystem();
}

void IButtonEventListener::bindToButtonEventSystem()
{
	m_EventDispatcher.bind( this );
}

void IButtonEventListener::unbindFromButtonEventSystem()
{
	m_EventDispatcher.unbind( this );
}

void IButtonEventListener::PublishEvent (const ButtonEvent& buttonEvent)
{
	m_EventDispatcher.dispatch( buttonEvent );
}
