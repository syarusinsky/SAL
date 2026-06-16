#ifndef ISALSYSEXEVENTLISTENER_HPP
#define ISALSYSEXEVENTLISTENER_HPP

/*******************************************************************
 * An ISalSysexEventListener specifies a simple interface which
 * a subclass can use to be notified of SAL system exclusive events,
 * such as preset sending across devices
 *
 * Preset data can be exchanged between two devices of the same model
 * by using the preset send/receive events. For the actual preset
 * data, the user should break each byte of the data into two nibbles
 * and then pack the data into consecutive SEND_PRESET_DATA_CHUNK
 * events. This is to get around the fact that midi data bytes must
 * be between 0x00 and 0x7F. Use the writeNibble function to fill
 * with preset data.
*******************************************************************/

#include "MidiConstants.hpp"
#include "IEventListener.hpp"

#include <stdint.h>

enum class SalSysexTypeEnum : unsigned int
{
	// preset send/receive events
	REQUEST_TO_SEND_PRESET = 0x00,
	REQUEST_TO_SEND_ALL_PRESETS = 0x01,
	ACCEPT_PRESET_OR_PRESETS = 0x02,
	DENY_PRESET_OR_PRESETS = 0x03,
	SEND_PRESET_DATA_CHUNK = 0x04,
	RECEIVED_PRESET = 0x05
};

class SalSysexEvent : public IEvent
{
	public:
		SalSysexEvent (const uint8_t* const data); // for interpreting from raw data, such as a received midi data
		~SalSysexEvent() override;

		bool isValid() const; // for checking if the raw data is a valid sal midi system exclusive message

		uint8_t getSizeInBytes() const { return m_SizeInBytes; }
		static uint8_t typeToSizeInBytes (const SalSysexTypeEnum& type);

		// for building preset send/receive events, data should be a buffer of MAX_MIDI_MESSAGE_SIZE
		static SalSysexEvent buildRequestToSendPresetEvent
				(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst);
		static SalSysexEvent buildRequestToSendAllPresetsEvent
				(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst);
		static SalSysexEvent buildAcceptPresetOrPresetsEvent
				(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst);
		static SalSysexEvent buildDenyPresetOrPresetsEvent
				(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst);
		static SalSysexEvent buildSendPresetDataChunkEvent // the preset data needs to be sent in nibbles, read the documentation at the beginning of this page
				(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst);
		static SalSysexEvent buildReceivedPresetEvent
				(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst);

		uint8_t getDevId() const { return m_Data[2]; }
		uint8_t getModelId() const { return m_Data[3]; }
		SalSysexTypeEnum getType() const { return static_cast<SalSysexTypeEnum>(m_Data[4]); }
		uint8_t getRecId() const { return m_Data[5]; } // receiver id (dev id of receiver of this data), may be 0 for unknown or don't care

		// helpers for preset send/receive events, you should check if it's a preset send/receive event type before calling these
		uint8_t getPresetNum() const { return m_Data[6]; }
		uint16_t getNumNibblesInPreset() const { return ( m_Data[7] << 7 ) | m_Data[8]; }
		const uint8_t* getPresetChunkNibbles() const { if ( this->getType() == SalSysexTypeEnum::SEND_PRESET_DATA_CHUNK ) { return &m_Data[9]; } else { return nullptr; } };
		uint8_t getMaxNumNibblesInPresetChunkNibbles() const { return MAX_MIDI_MESSAGE_SIZE - 10; }

		// used to write nibbles for data transfer
		bool writeNibble (uint8_t nibble); // returns false when buffer is full or operation fails

		const uint8_t* getRawData() const { return m_Data; }

	private:
		SalSysexEvent();

		uint8_t 	m_Data[MAX_MIDI_MESSAGE_SIZE];
		uint8_t 	m_NumNibblesWritten = 0;
		uint8_t 	m_SizeInBytes = 0;

		uint8_t getNibbleStart();
		static SalSysexEvent buildPresetEventHelper
			(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst, const SalSysexTypeEnum& type);
};

class ISalSysexEventListener : public IEventListener
{
	public:
		virtual ~ISalSysexEventListener();

		virtual void onSalSysexEvent (const SalSysexEvent& salSysexEvent) = 0;

		void bindToSalSysexEventSystem();
		void unbindFromSalSysexEventSystem();

		static void PublishEvent (const SalSysexEvent& salSysexEvent);

	private:
		static EventDispatcher<ISalSysexEventListener, SalSysexEvent, &ISalSysexEventListener::onSalSysexEvent> m_EventDispatcher;
};

#endif // ISALSYSEXEVENTLISTENER_HPP
