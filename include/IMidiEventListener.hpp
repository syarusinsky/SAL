#ifndef IMIDIEVENTLISTENER_HPP
#define IMIDIEVENTLISTENER_HPP

/*******************************************************************
 * An IMidiEventListener specifies a simple interface which
 * a subclass can use to be notified of midi events.
*******************************************************************/

#include <stdint.h>

#include "MidiConstants.hpp"
#include "IEventListener.hpp"

class MidiEvent : public IEvent
{
	public:
		MidiEvent();
		MidiEvent (uint8_t byte1, bool valid = false);
		MidiEvent (uint8_t byte1, uint8_t byte2, bool valid = false);
		MidiEvent (uint8_t byte1, uint8_t byte2, uint8_t byte3, bool valid = false);
		MidiEvent (unsigned int numBytes, uint8_t bytes...);
		~MidiEvent();

		uint8_t* getRawData() const;

		bool isNoteOn() const;
		bool isNoteOff() const;
		bool isPitchBend() const;
		bool isPolyphonicAftertouch() const;
		bool isControlChange() const;

		bool isValid() const;
		void setValid (bool valid);

		unsigned int getNumBytes() const;
		void setNumBytes (unsigned int numBytes);

	private:
		uint8_t 	m_Bytes[MAX_MIDI_MESSAGE_SIZE];
		bool 		m_Valid;
		unsigned int 	m_NumBytes;
};

class IMidiEventListener : public IEventListener
{
	public:
		virtual ~IMidiEventListener();

		virtual void onMidiEvent (const MidiEvent& midiEvent) = 0;

		void bindToMidiEventSystem();
		void unbindFromMidiEventSystem();

		static void PublishEvent (const MidiEvent& midiEvent);

	private:
		static EventDispatcher<IMidiEventListener, MidiEvent, &IMidiEventListener::onMidiEvent> m_EventDispatcher;
};

#endif // IMIDIEVENTLISTENER_HPP
