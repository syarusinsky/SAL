#ifndef ISTORAGEMEDIA_HPP
#define ISTORAGEMEDIA_HPP

/**************************************************************************
 * An IStorageMedia presents an interface for reading and writing to
 * a specific storage media, whether that be a hdd, eeprom, or any other
 * storage media.
**************************************************************************/

#include "Variant.hpp"

class IStorageMedia
{
	public:
		virtual ~IStorageMedia() {}

		virtual void writeToMedia (const Variant& data, const unsigned int sizeInBytes, const unsigned int offsetInBytes) = 0;
		virtual Variant readFromMedia (const unsigned int sizeInBytes, const unsigned int offsetInBytes) = 0;

		virtual bool needsInitialization() = 0;
		virtual void initialize() = 0;
		virtual void afterInitialize() = 0;
};

#endif // ISTORAGEMEDIA_HPP
