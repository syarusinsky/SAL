#include "ISalSysexEventListener.hpp"

#include <cstring>

// instantiating ISalSysexEventListener's event dispatcher
EventDispatcher<ISalSysexEventListener, SalSysexEvent, &ISalSysexEventListener::onSalSysexEvent> ISalSysexEventListener::m_EventDispatcher;

SalSysexEvent::SalSysexEvent (const uint8_t* const data) :
	IEvent( 0 ),
	m_NumNibblesWritten( 0 ),
	m_SizeInBytes( SalSysexEvent::typeToSizeInBytes(this->getType()) )
{
	std::memcpy( m_Data, data, sizeof(m_Data) );
}

SalSysexEvent::SalSysexEvent() :
	IEvent( 0 ),
	m_NumNibblesWritten( 0 ),
	m_SizeInBytes( 0 )
{
}

SalSysexEvent::~SalSysexEvent()
{
}

bool SalSysexEvent::isValid() const
{
	// uninitialized
	if ( m_SizeInBytes == 0 )
	{
		return false;
	}

	// not a midi system exclusive message with hobbyist manufacturer id
	if ( m_Data[0] != 0xF0 || m_Data[1] != MIDI_SYSEX_HOBBY_MANUFACTURER_ID || m_Data[m_SizeInBytes - 1] != 0xF7 )
	{
		return false;
	}

	return true;
}

uint8_t SalSysexEvent::typeToSizeInBytes (const SalSysexTypeEnum& type)
{
	uint8_t typeUInt = static_cast<uint8_t>( type );
	switch ( typeUInt ) {
		case static_cast<uint8_t>( SalSysexTypeEnum::REQUEST_TO_SEND_PRESET ):

			return 10;
		case static_cast<uint8_t>( SalSysexTypeEnum::REQUEST_TO_SEND_ALL_PRESETS ):

			return 10;
		case static_cast<uint8_t>( SalSysexTypeEnum::ACCEPT_PRESET_OR_PRESETS ):

			return 10;
		case static_cast<uint8_t>( SalSysexTypeEnum::DENY_PRESET_OR_PRESETS ):

			return 10;
		case static_cast<uint8_t>( SalSysexTypeEnum::SEND_PRESET_DATA_CHUNK ):

			return MAX_MIDI_MESSAGE_SIZE;
		case static_cast<uint8_t>( SalSysexTypeEnum::RECEIVED_PRESET ):

			return 10;
	};

	return 0;
}

SalSysexEvent SalSysexEvent::buildPresetEventHelper
	(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst, const SalSysexTypeEnum& type)
{
	SalSysexEvent event;

	// write midi data
	event.m_Data[0] = 0xF0; // system exclusive
	event.m_Data[1] = MIDI_SYSEX_HOBBY_MANUFACTURER_ID; // hobbyist manufacturer id
	event.m_Data[2] = devId;
	event.m_Data[3] = modelId;
	event.m_Data[4] = static_cast<uint8_t>( type );
	event.m_Data[5] = recId;
	event.m_Data[6] = prstNum;
	event.m_Data[7] = ( numNibblesInPrst >> 7 );
	event.m_Data[8] = numNibblesInPrst & 0x7F;

	if ( type == SalSysexTypeEnum::SEND_PRESET_DATA_CHUNK )
	{
		// zero out nibbles
		for ( unsigned int byteNum = event.getNibbleStart(); byteNum < MAX_MIDI_MESSAGE_SIZE - 1; byteNum++ )
		{
			event.m_Data[byteNum] = 0;
		}

		event.m_Data[MAX_MIDI_MESSAGE_SIZE - 1] = 0xF7; // end of system exclusive
		event.m_SizeInBytes = MAX_MIDI_MESSAGE_SIZE;
	}
	else
	{
		event.m_Data[9] = 0xF7; // end of system exclusive
		event.m_SizeInBytes = 10;
	}

	return event;
}

SalSysexEvent SalSysexEvent::buildRequestToSendPresetEvent
	(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst)
{
	return SalSysexEvent::buildPresetEventHelper( devId, modelId, recId, prstNum, numNibblesInPrst, SalSysexTypeEnum::REQUEST_TO_SEND_PRESET );
}

SalSysexEvent SalSysexEvent::buildRequestToSendAllPresetsEvent
	(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst)
{
	return SalSysexEvent::buildPresetEventHelper( devId, modelId, recId, prstNum, numNibblesInPrst, SalSysexTypeEnum::REQUEST_TO_SEND_ALL_PRESETS );
}

SalSysexEvent SalSysexEvent::buildAcceptPresetOrPresetsEvent
	(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst)
{
	return SalSysexEvent::buildPresetEventHelper( devId, modelId, recId, prstNum, numNibblesInPrst, SalSysexTypeEnum::ACCEPT_PRESET_OR_PRESETS );
}

SalSysexEvent SalSysexEvent::buildDenyPresetOrPresetsEvent
	(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst)
{
	return SalSysexEvent::buildPresetEventHelper( devId, modelId, recId, prstNum, numNibblesInPrst, SalSysexTypeEnum::DENY_PRESET_OR_PRESETS );
}

SalSysexEvent SalSysexEvent::buildSendPresetDataChunkEvent // the preset data needs to be sent in nibbles, read the documentation at the beginning of this page
	(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst)
{
	return SalSysexEvent::buildPresetEventHelper( devId, modelId, recId, prstNum, numNibblesInPrst, SalSysexTypeEnum::SEND_PRESET_DATA_CHUNK );
}

SalSysexEvent SalSysexEvent::buildReceivedPresetEvent
	(const uint8_t devId, const uint8_t modelId, const uint8_t recId, const uint8_t prstNum, const uint16_t numNibblesInPrst)
{
	return SalSysexEvent::buildPresetEventHelper( devId, modelId, recId, prstNum, numNibblesInPrst, SalSysexTypeEnum::RECEIVED_PRESET );
}

uint8_t SalSysexEvent::getNibbleStart()
{
	uint8_t typeUInt = static_cast<uint8_t>( this->getType() );
	switch ( typeUInt ) {
		case static_cast<uint8_t>( SalSysexTypeEnum::REQUEST_TO_SEND_PRESET ):

			return 0;
		case static_cast<uint8_t>( SalSysexTypeEnum::REQUEST_TO_SEND_ALL_PRESETS ):

			return 0;
		case static_cast<uint8_t>( SalSysexTypeEnum::ACCEPT_PRESET_OR_PRESETS ):

			return 0;
		case static_cast<uint8_t>( SalSysexTypeEnum::DENY_PRESET_OR_PRESETS ):

			return 0;
		case static_cast<uint8_t>( SalSysexTypeEnum::SEND_PRESET_DATA_CHUNK ):

			return 9;
		case static_cast<uint8_t>( SalSysexTypeEnum::RECEIVED_PRESET ):

			return 0;
	};

	return 0;
}

bool SalSysexEvent::writeNibble (uint8_t nibble)
{
	const uint8_t nibbleStart = this->getNibbleStart();
	const uint8_t nibbleIndex = nibbleStart + m_NumNibblesWritten;

	if ( nibbleStart != 0 && nibbleIndex != this->getSizeInBytes() - 1 )
	{
		m_Data[nibbleIndex] = nibble;
		m_NumNibblesWritten++;

		return true;
	}

	return false;
}

ISalSysexEventListener::~ISalSysexEventListener()
{
	this->unbindFromSalSysexEventSystem();
}

void ISalSysexEventListener::bindToSalSysexEventSystem()
{
	m_EventDispatcher.bind( this );
}

void ISalSysexEventListener::unbindFromSalSysexEventSystem()
{
	m_EventDispatcher.unbind( this );
}

void ISalSysexEventListener::PublishEvent (const SalSysexEvent& salSysexEvent)
{
	m_EventDispatcher.dispatch( salSysexEvent );
}
