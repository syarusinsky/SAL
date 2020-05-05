#include "IPitchEventListener.hpp"

// instantiating IPitchEventListener's event dispatcher
EventDispatcher<IPitchEventListener, PitchEvent, &IPitchEventListener::onPitchEvent> IPitchEventListener::m_EventDispatcher;

PitchEvent::PitchEvent (float pitchFactor, unsigned int channel) :
	IEvent ( channel ),
	m_PitchFactor( pitchFactor )
{
}

PitchEvent::~PitchEvent()
{
}

float PitchEvent::getPitchFactor() const
{
	return m_PitchFactor;
}

void PitchEvent::setPitchFactor (float pitchFactor)
{
	m_PitchFactor = pitchFactor;
}

IPitchEventListener::~IPitchEventListener()
{
	this->unbindFromPitchEventSystem();
}

void IPitchEventListener::bindToPitchEventSystem()
{
	m_EventDispatcher.bind( this );
}

void IPitchEventListener::unbindFromPitchEventSystem()
{
	m_EventDispatcher.unbind( this );
}

void IPitchEventListener::PublishEvent (const PitchEvent& pitchEvent)
{
	m_EventDispatcher.dispatch( pitchEvent );
}
