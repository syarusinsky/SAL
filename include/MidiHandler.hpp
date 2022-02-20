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

		void setShouldPublishMidi (bool shouldPublish) { m_ShouldPublishMidi = shouldPublish; }
		bool shouldPublishMidi() const { return m_ShouldPublishMidi; }
		void setShouldPublishNoteOn (bool shouldPublish) { m_ShouldPublishNoteOn = shouldPublish; }
		bool shouldPublishNoteOn() const { return m_ShouldPublishNoteOn; }
		void setShouldPublishNoteOff (bool shouldPublish) { m_ShouldPublishNoteOff = shouldPublish; }
		bool shouldPublishNoteOff() const { return m_ShouldPublishNoteOff; }
		void setShouldPublishPitch (bool shouldPublish) { m_ShouldPublishPitch = shouldPublish; }
		bool shouldPublishPitch() const { return m_ShouldPublishPitch; }

		void setNumberOfSemitonesToPitchBend (unsigned int numSemitones);
		unsigned int getNumberOfSemitonesToPitchBend();

	private:
		uint8_t m_WorkingStatusByte;
		unsigned int m_WorkingMessageNumDataBytes;
		unsigned int m_WorkingDataByteIndex;
		unsigned int m_CurrentWriteIndex;
		unsigned int m_CurrentReadIndex;
		MidiEvent m_MsgBuffer[MIDI_BUFFER_SIZE];

		bool 		m_ShouldPublishMidi;
		bool 		m_ShouldPublishNoteOn;
		bool 		m_ShouldPublishNoteOff;
		bool 		m_ShouldPublishPitch;

		unsigned int m_SemitonesToPitchBend;
};

#endif // MIDIHANDLER_HPP
