#include "IKeyEventListener.hpp"

// instantiating IKeyEventListener's event dispatcher
EventDispatcher<IKeyEventListener, KeyEvent, &IKeyEventListener::onKeyEvent> IKeyEventListener::m_EventDispatcher;

KeyEvent::KeyEvent (KeyPressedEnum pressed, unsigned int note, unsigned int velocity, unsigned int channel) :
	IEvent( channel ),
	m_Pressed( pressed ),
	m_Note( note ),
	m_Velocity( velocity )
{
}

KeyEvent::~KeyEvent()
{
}

bool KeyEvent::operator== (const KeyEvent& other) const
{
	if ( other.m_Pressed == m_Pressed && other.m_Note == m_Note && other.m_Velocity == m_Velocity )
	{
		return true;
	}

	return false;
}

KeyPressedEnum KeyEvent::pressed() const
{
	return m_Pressed;
}

unsigned int KeyEvent::note() const
{
	return m_Note;
}

unsigned int KeyEvent::velocity() const
{
	return m_Velocity;
}

bool KeyEvent::isNoteAndType (const KeyEvent& other) const
{
	if ( other.m_Pressed == m_Pressed && other.m_Note == m_Note )
	{
		return true;
	}

	return false;
}

bool KeyEvent::isNoteAndType (const KeyEvent& other, const KeyPressedEnum& pressed) const
{

	if ( other.m_Note == m_Note && pressed == m_Pressed )
	{
		return true;
	}

	return false;
}

IKeyEventListener::~IKeyEventListener()
{
	this->unbindFromKeyEventSystem();
}

void IKeyEventListener::bindToKeyEventSystem()
{
	m_EventDispatcher.bind( this );
}

void IKeyEventListener::unbindFromKeyEventSystem()
{
	m_EventDispatcher.unbind( this );
}

void IKeyEventListener::PublishEvent (const KeyEvent& keyEvent)
{
	m_EventDispatcher.dispatch( keyEvent );
}
