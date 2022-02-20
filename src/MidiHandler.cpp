#include "MidiHandler.hpp"
#include "IKeyEventListener.hpp"
#include "IPitchEventListener.hpp"

#include <cmath>


MidiHandler::MidiHandler() :
	m_WorkingStatusByte( 0 ),
	m_WorkingMessageNumDataBytes( 0 ),
	m_WorkingDataByteIndex( 0 ),
	m_CurrentWriteIndex( 0 ),
	m_CurrentReadIndex( 0 ),
	m_ShouldPublishMidi( true ),
	m_ShouldPublishNoteOn( true ),
	m_ShouldPublishNoteOff( true ),
	m_ShouldPublishPitch( true ),
	m_SemitonesToPitchBend( 1 )
{
}

MidiHandler::~MidiHandler()
{
}

void MidiHandler::processByte (uint8_t byte)
{
	if ( (byte & MIDI_STATUS_BYTE) ) // if status byte
	{
		m_WorkingStatusByte = byte;
		uint8_t statusByteNybble = (m_WorkingStatusByte >> 4);

		MidiEvent* midiEvent = &m_MsgBuffer[m_CurrentWriteIndex];
		midiEvent->setValid( false );
		uint8_t* midiMessageBytes = midiEvent->getRawData();
		midiMessageBytes[0] = byte;
		m_WorkingMessageNumDataBytes = 0;

		if ( statusByteNybble != MIDI_SYSTEM_COMMON ) // if MIDI channel message
		{
			switch ( statusByteNybble )
			{
				case MIDI_NOTE_OFF:
					m_WorkingMessageNumDataBytes = MIDI_NOTE_OFF_NUM_DATA;
					break;
				case MIDI_NOTE_ON:
					m_WorkingMessageNumDataBytes = MIDI_NOTE_ON_NUM_DATA;
					break;
				case MIDI_AFTERTOUCH:
					m_WorkingMessageNumDataBytes = MIDI_AFTERTOUCH_NUM_DATA;
					break;
				case MIDI_CONTROL_CHANGE:
					m_WorkingMessageNumDataBytes = MIDI_CONTROL_CHANGE_NUM_DATA;
					break;
				case MIDI_PROGRAM_CHANGE:
					m_WorkingMessageNumDataBytes = MIDI_PROGRAM_CHANGE_NUM_DATA;
					break;
				case MIDI_AFTERTOUCH_MONO:
					m_WorkingMessageNumDataBytes = MIDI_AFTERTOUCH_MONO_NUM_DATA;
					break;
				case MIDI_PITCH_BEND:
					m_WorkingMessageNumDataBytes = MIDI_PITCH_BEND_NUM_DATA;
					break;
				default:
					break;
			}
		}
		else // if MIDI System Common or Realtime Message
		{
			switch (m_WorkingStatusByte)
			{
				case MIDI_TIME_CODE:
					m_WorkingMessageNumDataBytes = MIDI_TIME_CODE_NUM_DATA;
					break;
				case MIDI_SONG_POSITION:
					m_WorkingMessageNumDataBytes = MIDI_SONG_POSITION_NUM_DATA;
					break;
				case MIDI_SONG_SELECT:
					m_WorkingMessageNumDataBytes = MIDI_SONG_SELECT_NUM_DATA;
					break;
				case MIDI_TUNE_REQUEST:
					m_WorkingMessageNumDataBytes = MIDI_TUNE_REQUEST_NUM_DATA;
					break;
				case MIDI_END_OF_EXCLUSIVE:
					m_WorkingMessageNumDataBytes = MIDI_END_OF_EXCLUSIVE_NUM_DATA;
					break;
				case MIDI_TIMING_CLOCK:
					m_WorkingMessageNumDataBytes = MIDI_TIMING_CLOCK_NUM_DATA;
					break;
				case MIDI_START:
					m_WorkingMessageNumDataBytes = MIDI_START_NUM_DATA;
					break;
				case MIDI_CONTINUE:
					m_WorkingMessageNumDataBytes = MIDI_CONTINUE_NUM_DATA;
					break;
				case MIDI_STOP:
					m_WorkingMessageNumDataBytes = MIDI_STOP_NUM_DATA;
					break;
				case MIDI_ACTIVE_SENSING:
					m_WorkingMessageNumDataBytes = MIDI_ACTIVE_SENSING_NUM_DATA;
					break;
				case MIDI_RESET:
					m_WorkingMessageNumDataBytes = MIDI_RESET;
					break;
				default:
					break;
			}
		}

		m_WorkingDataByteIndex = 0;
		m_CurrentWriteIndex = (m_CurrentWriteIndex + 1) % MIDI_BUFFER_SIZE;

		// some messages have no data bytes
		if ( m_WorkingDataByteIndex == m_WorkingMessageNumDataBytes)
		{
			midiEvent->setValid( true );
			midiEvent->setNumBytes( 1 );
		}
	}
	else // if data byte
	{
		MidiEvent* midiEvent = &m_MsgBuffer[(m_CurrentWriteIndex - 1 + MIDI_BUFFER_SIZE) % MIDI_BUFFER_SIZE];
		uint8_t* midiMessageBytes = midiEvent->getRawData();
		m_WorkingDataByteIndex++;

		if ( m_WorkingDataByteIndex < m_WorkingMessageNumDataBytes )
		{
			midiMessageBytes[m_WorkingDataByteIndex] = byte;
		}
		else if ( m_WorkingDataByteIndex == m_WorkingMessageNumDataBytes )
		{
			midiMessageBytes[m_WorkingDataByteIndex] = byte;
			midiEvent->setValid( true );
			midiEvent->setNumBytes( m_WorkingMessageNumDataBytes + 1 );
		}
		else
		{
			// in case of a running midi message, eg: a note data byte is receieved after a full note on midi message
			m_CurrentWriteIndex = (m_CurrentWriteIndex + 1) % MIDI_BUFFER_SIZE;

			midiEvent = &m_MsgBuffer[(m_CurrentWriteIndex - 1 + MIDI_BUFFER_SIZE) % MIDI_BUFFER_SIZE];
			midiEvent->setValid( false );
			midiMessageBytes = midiEvent->getRawData();

			midiMessageBytes[0] = m_WorkingStatusByte;
			m_WorkingDataByteIndex = 1;
			midiMessageBytes[m_WorkingDataByteIndex] = byte;

			if ( m_WorkingDataByteIndex == m_WorkingMessageNumDataBytes )
			{
				midiEvent->setValid( true );
				midiEvent->setNumBytes( m_WorkingMessageNumDataBytes + 1 );
			}
		}
	}
}

MidiEvent* MidiHandler::nextMidiMessage()
{
	MidiEvent* retVal = nullptr;

	if ( m_CurrentReadIndex != m_CurrentWriteIndex && m_MsgBuffer[m_CurrentReadIndex].isValid() )
	{
		retVal = &(m_MsgBuffer[m_CurrentReadIndex]);
		m_CurrentReadIndex = (m_CurrentReadIndex + 1) % MIDI_BUFFER_SIZE;
	}

	return retVal;
}

void MidiHandler::dispatchEvents()
{
	MidiEvent* nextMidiEvent = this->nextMidiMessage();

	while ( nextMidiEvent != nullptr )
	{
		MidiEvent& midiEvent = *(nextMidiEvent);
		midiEvent.setValid( false );
		const uint8_t* const midiRawData = midiEvent.getRawData();
		if ( m_ShouldPublishMidi ) IMidiEventListener::PublishEvent( midiEvent );

		if ( m_ShouldPublishPitch && midiEvent.isPitchBend() )
		{
			uint8_t lsb = midiRawData[1];
			uint8_t msb = midiRawData[2];
			short pitchBendVal = (msb << 7) | lsb;

			// get normalized range between -1.0 and 1.0
			float pitchBendNormalized = ( (static_cast<float>(pitchBendVal) / 16383.0f) * 2.0f ) - 1.0f;

			// multiply by the number of semitones in bend range then divide by 12, then raise 2 to that power
			float pitchBendFactor = powf( 2.0f, (pitchBendNormalized * static_cast<float>(m_SemitonesToPitchBend)) / 12.0f );

			IPitchEventListener::PublishEvent( PitchEvent(pitchBendFactor) );
		}
		else if ( m_ShouldPublishNoteOn && midiEvent.isNoteOn() )
		{
			IKeyEventListener::PublishEvent( KeyEvent(KeyPressedEnum::PRESSED, midiRawData[1], midiRawData[2]) );
		}
		else if ( m_ShouldPublishNoteOff && midiEvent.isNoteOff() )
		{
			IKeyEventListener::PublishEvent( KeyEvent(KeyPressedEnum::RELEASED, midiRawData[1], midiRawData[2]) );
		}

		// get next MIDI message in the queue and continue while loop
		nextMidiEvent = this->nextMidiMessage();
	}
}

void MidiHandler::setNumberOfSemitonesToPitchBend (unsigned int numSemitones)
{
	m_SemitonesToPitchBend = numSemitones;
}

unsigned int MidiHandler::getNumberOfSemitonesToPitchBend()
{
	return m_SemitonesToPitchBend;
}
