#include "IMidiEventListener.hpp"

#include <cstdarg>

// instantiating IMidiEventListener's event dispatcher
EventDispatcher<IMidiEventListener, MidiEvent, &IMidiEventListener::onMidiEvent> IMidiEventListener::m_EventDispatcher;

MidiEvent::MidiEvent() :
	IEvent( 0 ),
	m_Bytes{ 0 },
	m_Valid( false ),
	m_NumBytes( 0 )
{
}

MidiEvent::MidiEvent (uint8_t byte1, bool valid) :
	IEvent( 0 ),
	m_Bytes{ 0 },
	m_Valid( valid ),
	m_NumBytes( 1 )
{
	m_Bytes[0] = byte1;
}

MidiEvent::MidiEvent (uint8_t byte1, uint8_t byte2, bool valid) :
	IEvent( 0 ),
	m_Bytes{ 0 },
	m_Valid( valid ),
	m_NumBytes( 2 )
{
	m_Bytes[0] = byte1;
	m_Bytes[1] = byte2;
}

MidiEvent::MidiEvent (uint8_t byte1, uint8_t byte2, uint8_t byte3, bool valid) :
	IEvent( 0 ),
	m_Bytes{ 0 },
	m_Valid( valid ),
	m_NumBytes( 3 )
{
	m_Bytes[0] = byte1;
	m_Bytes[1] = byte2;
	m_Bytes[2] = byte3;
}

MidiEvent::MidiEvent (unsigned int numBytes, uint8_t bytes...) :
	IEvent( 0 ),
	m_Bytes{ 0 },
	m_Valid( true ),
	m_NumBytes( numBytes )
{
	va_list args;
	va_start( args, bytes );

	// first byte
	m_Bytes[0] = bytes;

	for ( unsigned int byteNum = 1; byteNum < numBytes; byteNum++ )
	{
		m_Bytes[byteNum] = static_cast<uint8_t>( va_arg(args, unsigned int) );
	}
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

bool MidiEvent::isValid() const
{
	return m_Valid;
}

void MidiEvent::setValid (bool valid)
{
	m_Valid = valid;
}

unsigned int MidiEvent::getNumBytes() const
{
	return m_NumBytes;
}

void MidiEvent::setNumBytes (unsigned int numBytes)
{
	m_NumBytes = numBytes;
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
