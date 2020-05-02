#ifndef MIDIMSG_HPP
#define MIDIMSG_HPP

/*********************************************************************
 * A MidiMsg describes a MIDI message of a maximum size defined
 * by MAX_MIDI_MESSAGE_SIZE. The class also provides functions for
 * determining the type of MIDI messages.
*********************************************************************/

#include "MidiConstants.hpp"

#include <stdint.h>

class MidiMsg
{
	public:
		MidiMsg();
		MidiMsg (uint8_t byte1);
		MidiMsg (uint8_t byte1, uint8_t byte2);
		MidiMsg (uint8_t byte1, uint8_t byte2, uint8_t byte3);
		~MidiMsg();

		uint8_t* getRawData();

		bool isNoteOn();
		bool isNoteOff();
		bool isPitchBend();
		bool isPolyphonicAftertouch();
		bool isControlChange();

	private:
		uint8_t m_Bytes[MAX_MIDI_MESSAGE_SIZE];
};

#endif // MIDIMSG_HPP
