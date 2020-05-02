#ifndef MIDIHANDLER_HPP
#define MIDIHANDLER_HPP

/********************************************************
 * The MidiHandler's job is to take in bytes from a
 * peripheral MIDI driver and build MidiEvents, then
 * serve the MidiEvents, KeyEvents, Pitch Events, and
 * other related events to the event system.
********************************************************/

#include "IMidiEventListener.hpp"

class MidiHandler
{
	public:
		MidiHandler();
		~MidiHandler();

		void processByte (uint8_t byte);
		MidiEvent* nextMidiMessage(); // also removes the next MIDI message from the queue

		void dispatchEvents();

		void setNumberOfSemitonesToPitchBend (unsigned int numSemitones);
		unsigned int getNumberOfSemitonesToPitchBend();

	private:
		uint8_t m_WorkingStatusByte;
		unsigned int m_WorkingMessageLength;
		unsigned int m_WorkingDataByteIndex;
		unsigned int m_CurrentWriteIndex;
		unsigned int m_CurrentReadIndex;
		MidiEvent m_MsgBuffer[MIDI_BUFFER_SIZE];

		unsigned int m_SemitonesToPitchBend;
};

#endif // MIDIHANDLER_HPP
