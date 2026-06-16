#include "MidiHandler.hpp"
#include "IKeyEventListener.hpp"
#include "IPitchEventListener.hpp"
#include "ISalSysexEventListener.hpp"

#include <cmath>
#include <algorithm>


MidiHandler::MidiHandler() :
	m_WorkingStatusByte( 0 ),
	m_WorkingMessageNumDataBytes( 0 ),
	m_WorkingDataByteIndex( 0 ),
	m_InputCurrentWriteIndex( 0 ),
	m_InputCurrentReadIndex( 0 ),
	m_ShouldPublishMidi( false ),
	m_ShouldPublishNoteOn( true ),
	m_ShouldPublishNoteOff( true ),
	m_ShouldPublishPitch( true ),
	m_ShouldPublishSalSysex( true ),
	m_SemitonesToPitchBend( 1 ),
	m_OutputCurrentWriteIndex( 0 ),
	m_OutputCurrentReadIndex( 0 )
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

		MidiEvent* midiEvent = &m_MsgInputBuffer[m_InputCurrentWriteIndex];
		midiEvent->setValid( false );
		uint8_t* midiMessageBytes = midiEvent->getRawData();
		midiMessageBytes[0] = byte;
		m_WorkingMessageNumDataBytes = 0;

		if ( statusByteNybble != MIDI_SYSTEM_STATUS_BYTE ) // if MIDI channel message
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
		else // if MIDI System Exclusive, MIDI System Common or Realtime Message
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
				{
					// finish the previous midi system exclusive message
					const unsigned int sysexMsgIndex = (m_InputCurrentWriteIndex - 1 + MIDI_BUFFER_SIZE) % MIDI_BUFFER_SIZE;
					midiEvent = &m_MsgInputBuffer[sysexMsgIndex];
					midiMessageBytes = midiEvent->getRawData();
					m_WorkingDataByteIndex++;
					midiMessageBytes[m_WorkingDataByteIndex] = byte;
					midiEvent->setValid( true );
					midiEvent->setNumBytes( m_WorkingDataByteIndex + 1 );
				}

					return;
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
					// this will be the case with a midi system exclusive message
					m_WorkingMessageNumDataBytes = MAX_MIDI_MESSAGE_SIZE - 1;

					break;
			}
		}

		m_WorkingDataByteIndex = 0;
		m_InputCurrentWriteIndex = (m_InputCurrentWriteIndex + 1) % MIDI_BUFFER_SIZE;

		// some messages have no data bytes
		if ( m_WorkingDataByteIndex == m_WorkingMessageNumDataBytes)
		{
			midiEvent->setValid( true );
			midiEvent->setNumBytes( 1 );
		}
	}
	else // if data byte
	{
		MidiEvent* midiEvent = &m_MsgInputBuffer[(m_InputCurrentWriteIndex - 1 + MIDI_BUFFER_SIZE) % MIDI_BUFFER_SIZE];
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
			m_InputCurrentWriteIndex = (m_InputCurrentWriteIndex + 1) % MIDI_BUFFER_SIZE;

			midiEvent = &m_MsgInputBuffer[(m_InputCurrentWriteIndex - 1 + MIDI_BUFFER_SIZE) % MIDI_BUFFER_SIZE];
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

MidiEvent* MidiHandler::nextInputMidiMessage()
{
	MidiEvent* retVal = nullptr;

	if ( m_InputCurrentReadIndex != m_InputCurrentWriteIndex && m_MsgInputBuffer[m_InputCurrentReadIndex].isValid() )
	{
		retVal = &(m_MsgInputBuffer[m_InputCurrentReadIndex]);
		m_InputCurrentReadIndex = (m_InputCurrentReadIndex + 1) % MIDI_BUFFER_SIZE;
	}

	return retVal;
}

void MidiHandler::dispatchEvents()
{
	MidiEvent* nextMidiEvent = this->nextInputMidiMessage();

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

			IPitchEventListener::PublishEvent( PitchEvent(pitchBendFactor, midiRawData[0] & 0b1111) );
		}
		else if ( m_ShouldPublishNoteOn && midiEvent.isNoteOn() )
		{
			IKeyEventListener::PublishEvent( KeyEvent(KeyPressedEnum::PRESSED, midiRawData[1], midiRawData[2], midiRawData[0] & 0b1111) );
		}
		else if ( m_ShouldPublishNoteOff && midiEvent.isNoteOff() )
		{
			IKeyEventListener::PublishEvent( KeyEvent(KeyPressedEnum::RELEASED, midiRawData[1], midiRawData[2], midiRawData[0] & 0b1111) );
		}
		else if ( m_ShouldPublishSalSysex && midiEvent.isSalSysex() )
		{
			ISalSysexEventListener::PublishEvent( SalSysexEvent(midiRawData) );
		}

		// get next MIDI message in the queue and continue while loop
		nextMidiEvent = this->nextInputMidiMessage();
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

void MidiHandler::processKeyEvent (const KeyEvent& keyEvent)
{
	const uint8_t statusByte = keyEvent.getChannel()
		| ( (keyEvent.pressed() == KeyPressedEnum::PRESSED) ? MIDI_NOTE_ON << 4 : MIDI_NOTE_OFF << 4 );
	const uint8_t dataByte1 = static_cast<uint8_t>( keyEvent.note() );
	const uint8_t dataByte2 = static_cast<uint8_t>( keyEvent.velocity() );

	MidiEvent* midiEvent = &m_MsgOutputBuffer[m_OutputCurrentWriteIndex];
	new (midiEvent) MidiEvent( statusByte, dataByte1, dataByte2, true );

	m_OutputCurrentWriteIndex = ( m_OutputCurrentWriteIndex + 1 ) % MIDI_BUFFER_SIZE;
}

void MidiHandler::processPitchEvent (const PitchEvent& pitchEvent)
{
	const uint8_t statusByte = pitchEvent.getChannel() | ( MIDI_PITCH_BEND << 4 );

	const float pitchBendNormalized = ( log2f(pitchEvent.getPitchFactor()) * 12.0f ) / static_cast<float>(m_SemitonesToPitchBend);
	const float pitchBendValFloat = ( (pitchBendNormalized + 1.0f) / 2.0f ) * 16383.0f;

	// clamp to 14 bits and round to be safe
	const int pitchBendVal = std::clamp( static_cast<int>(std::round(pitchBendValFloat)), 0, 16383 );

	const uint8_t lsb = static_cast<uint8_t>(pitchBendVal & 0x7F);
	const uint8_t msb = static_cast<uint8_t>((pitchBendVal >> 7) & 0x7F);

	MidiEvent* midiEvent = &m_MsgOutputBuffer[m_OutputCurrentWriteIndex];
	new (midiEvent) MidiEvent( statusByte, lsb, msb, true );

	m_OutputCurrentWriteIndex = ( m_OutputCurrentWriteIndex + 1 ) % MIDI_BUFFER_SIZE;
}

void MidiHandler::processSalSysexEvent (const SalSysexEvent& salSysex)
{
	MidiEvent* midiEvent = &m_MsgOutputBuffer[m_OutputCurrentWriteIndex];
	new (midiEvent) MidiEvent( static_cast<unsigned int>(salSysex.getSizeInBytes()), true, salSysex.getRawData() );

	m_OutputCurrentWriteIndex = ( m_OutputCurrentWriteIndex + 1 ) % MIDI_BUFFER_SIZE;
}

MidiEvent* MidiHandler::nextOutputMidiMessage()
{
	MidiEvent* retVal = nullptr;

	if ( m_OutputCurrentReadIndex != m_OutputCurrentWriteIndex && m_MsgOutputBuffer[m_OutputCurrentReadIndex].isValid() )
	{
		retVal = &(m_MsgOutputBuffer[m_OutputCurrentReadIndex]);
		m_OutputCurrentReadIndex = (m_OutputCurrentReadIndex + 1) % MIDI_BUFFER_SIZE;
	}

	return retVal;
}
