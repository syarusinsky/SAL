#include "IMidiEventListener.hpp"

// instantiating IMidiEventListener's event dispatcher
EventDispatcher<IMidiEventListener, MidiEvent, &IMidiEventListener::onMidiEvent> IMidiEventListener::m_EventDispatcher;

MidiEvent::MidiEvent() :
	IEvent ( 0 ),
	m_Bytes { 0 }
{
}

MidiEvent::MidiEvent (uint8_t byte1) :
	IEvent ( 0 ),
	m_Bytes { 0 }
{
	m_Bytes[0] = byte1;
}

MidiEvent::MidiEvent (uint8_t byte1, uint8_t byte2) :
	IEvent ( 0 ),
	m_Bytes { 0 }
{
	m_Bytes[0] = byte1;
	m_Bytes[1] = byte2;
}

MidiEvent::MidiEvent (uint8_t byte1, uint8_t byte2, uint8_t byte3) :
	IEvent ( 0 ),
	m_Bytes { 0 }
{
	m_Bytes[0] = byte1;
	m_Bytes[1] = byte2;
	m_Bytes[2] = byte3;
}

MidiEvent::~MidiEvent()
{
}

uint8_t* MidiEvent::getRawData() const
{
	return const_cast<uint8_t*>( m_Bytes );
}

bool MidiEvent::isNoteOn() const
{
	if ( (m_Bytes[0] >> 4) == MIDI_NOTE_ON )
	{
		return true;
	}

	return false;
}

bool MidiEvent::isNoteOff() const
{
	if ( (m_Bytes[0] >> 4) == MIDI_NOTE_OFF )
	{
		return true;
	}

	return false;
}

bool MidiEvent::isPitchBend() const
{
	if ( (m_Bytes[0] >> 4) == MIDI_PITCH_BEND )
	{
		return true;
	}

	return false;
}

bool MidiEvent::isPolyphonicAftertouch() const
{
	if ( (m_Bytes[0] >> 4) == MIDI_AFTERTOUCH )
	{
		return true;
	}

	return false;
}

bool MidiEvent::isControlChange() const
{
	if ( (m_Bytes[0] >> 4) == MIDI_CONTROL_CHANGE )
	{
		return true;
	}

	return false;
}

IMidiEventListener::~IMidiEventListener()
{
	this->unbindFromMidiEventSystem();
}

void IMidiEventListener::bindToMidiEventSystem()
{
	m_EventDispatcher.bind( this );
}

void IMidiEventListener::unbindFromMidiEventSystem()
{
	m_EventDispatcher.unbind( this );
}

void IMidiEventListener::PublishEvent (const MidiEvent& midiEvent)
{
	m_EventDispatcher.dispatch( midiEvent );
}
