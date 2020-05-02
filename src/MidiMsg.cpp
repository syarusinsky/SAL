#include "MidiMsg.hpp"

MidiMsg::MidiMsg() :
	m_Bytes { 0 }
{
}

MidiMsg::MidiMsg (uint8_t byte1) :
	m_Bytes { 0 }
{
	m_Bytes[0] = byte1;
}

MidiMsg::MidiMsg (uint8_t byte1, uint8_t byte2) :
	m_Bytes { 0 }
{
	m_Bytes[0] = byte1;
	m_Bytes[1] = byte2;
}

MidiMsg::MidiMsg (uint8_t byte1, uint8_t byte2, uint8_t byte3) :
	m_Bytes { 0 }
{
	m_Bytes[0] = byte1;
	m_Bytes[1] = byte2;
	m_Bytes[2] = byte3;
}

MidiMsg::~MidiMsg()
{
}

uint8_t* MidiMsg::getRawData()
{
	return m_Bytes;
}

bool MidiMsg::isNoteOn()
{
	if ( (m_Bytes[0] >> 4) == MIDI_NOTE_ON )
	{
		return true;
	}

	return false;
}

bool MidiMsg::isNoteOff()
{
	if ( (m_Bytes[0] >> 4) == MIDI_NOTE_OFF )
	{
		return true;
	}

	return false;
}

bool MidiMsg::isPitchBend()
{
	if ( (m_Bytes[0] >> 4) == MIDI_PITCH_BEND )
	{
		return true;
	}

	return false;
}

bool MidiMsg::isPolyphonicAftertouch()
{
	if ( (m_Bytes[0] >> 4) == MIDI_AFTERTOUCH )
	{
		return true;
	}

	return false;
}

bool MidiMsg::isControlChange()
{
	if ( (m_Bytes[0] >> 4) == MIDI_CONTROL_CHANGE )
	{
		return true;
	}

	return false;
}
