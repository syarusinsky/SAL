#include "IPotEventListener.hpp"

// instantiating IPotEventListener's event dispatcher
EventDispatcher<IPotEventListener, PotEvent, &IPotEventListener::onPotEvent> IPotEventListener::m_EventDispatcher;

PotEvent::PotEvent (float percentage, unsigned int channel) :
	IEvent( channel ),
	m_Percentage( std::max(std::min(percentage, 1.0f), 0.0f) ) // ensure between 0 and 1
{
}

PotEvent::~PotEvent()
{
}

IPotEventListener::~IPotEventListener()
{
	this->unbindFromPotEventSystem();
}

void IPotEventListener::bindToPotEventSystem()
{
	m_EventDispatcher.bind( this );
}

void IPotEventListener::unbindFromPotEventSystem()
{
	m_EventDispatcher.unbind( this );
}

void IPotEventListener::PublishEvent (const PotEvent& potEvent)
{
	m_EventDispatcher.dispatch( potEvent );
}
