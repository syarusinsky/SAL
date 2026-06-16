#ifndef MIDIHANDLER_HPP
#define MIDIHANDLER_HPP

/********************************************************
 * The MidiHandler's input takes in bytes from a
 * peripheral MIDI driver and build MidiEvents, then
 * serve the MidiEvents, KeyEvents, Pitch Events, and
 * other related events to the event system. On the
 * output side, the MidiHandler takes in events and
 * provides a queue for the system to output from.
********************************************************/

#include "IMidiEventListener.hpp"

class KeyEvent;
class PitchEvent;
class SalSysexEvent;

class MidiHandler
{
	public:
		MidiHandler();
		~MidiHandler();

		// input
		void processByte (uint8_t byte);
		MidiEvent* nextInputMidiMessage(); // also removes the next MIDI message from the queue

		void dispatchEvents();

		void setShouldPublishMidi (bool shouldPublish) { m_ShouldPublishMidi = shouldPublish; }
		bool shouldPublishMidi() const { return m_ShouldPublishMidi; }
		void setShouldPublishNoteOn (bool shouldPublish) { m_ShouldPublishNoteOn = shouldPublish; }
		bool shouldPublishNoteOn() const { return m_ShouldPublishNoteOn; }
		void setShouldPublishNoteOff (bool shouldPublish) { m_ShouldPublishNoteOff = shouldPublish; }
		bool shouldPublishNoteOff() const { return m_ShouldPublishNoteOff; }
		void setShouldPublishPitch (bool shouldPublish) { m_ShouldPublishPitch = shouldPublish; }
		bool shouldPublishPitch() const { return m_ShouldPublishPitch; }
		void setShouldPublishSalSysex (bool shouldPublish) { m_ShouldPublishSalSysex = shouldPublish; }
		bool shouldPublishSalSysex() const { return m_ShouldPublishSalSysex; }

		void setNumberOfSemitonesToPitchBend (unsigned int numSemitones);
		unsigned int getNumberOfSemitonesToPitchBend();

		// output
		void processKeyEvent (const KeyEvent& keyEvent);
		void processPitchEvent (const PitchEvent& pitchEvent);
		void processSalSysexEvent (const SalSysexEvent& salSysex);

		MidiEvent* nextOutputMidiMessage(); // also removes the next MIDI message from the queue

	private:
		// input
		uint8_t m_WorkingStatusByte;
		unsigned int m_WorkingMessageNumDataBytes;
		unsigned int m_WorkingDataByteIndex;
		unsigned int m_InputCurrentWriteIndex;
		unsigned int m_InputCurrentReadIndex;
		MidiEvent m_MsgInputBuffer[MIDI_BUFFER_SIZE];

		bool 		m_ShouldPublishMidi; // this is set to false by default, since it's easier to respond to specific events
		bool 		m_ShouldPublishNoteOn;
		bool 		m_ShouldPublishNoteOff;
		bool 		m_ShouldPublishPitch;
		bool 		m_ShouldPublishSalSysex;

		unsigned int m_SemitonesToPitchBend;

		// output
		MidiEvent m_MsgOutputBuffer[MIDI_BUFFER_SIZE];

		unsigned int m_OutputCurrentWriteIndex;
		unsigned int m_OutputCurrentReadIndex;
};

#endif // MIDIHANDLER_HPP
