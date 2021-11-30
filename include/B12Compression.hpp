#ifndef B12COMPRESSION_HPP
#define B12COMPRESSION_HPP

/***********************************************************************
 * The B12Compression functions offer a compression method for packing
 * 12-bit data contained in uint16_t values into a series of bytes that
 * take up 75% of the space that the uint16_t values take up. Likewise,
 * there is a decompression function that unpacks the data back into
 * the original uint16_t values.
***********************************************************************/

#include <stdint.h>
#include <algorithm>

// returns true if buffer sizes are appropriate, false otherwise
inline bool B12Compress (uint16_t* uncompressedBuf, const unsigned int uncompressedBufSize,
				uint8_t* compressedBuf, const unsigned int compressedBufSize)
{
	// first check if the uncompressed size fits into the compressed size once compressed
	if ( ((uncompressedBufSize * 2) / 4) * 3 != compressedBufSize ) return false;

	// zero compressedBuf
	std::fill( compressedBuf, compressedBuf + compressedBufSize, 0 );

	// pack the data
	unsigned int compressedBufIndex = 0;
	uint8_t bitsLeftInCompressBufByte = 8;
	for ( unsigned int sample = 0; sample < uncompressedBufSize; sample++ )
	{
		uint8_t bitsLeftInValueToWrite = 12;
		uint16_t valueToWrite = uncompressedBuf[sample] & 0xFFF;

		while ( bitsLeftInValueToWrite > 0 )
		{
			// write in nibbles
			uint8_t nibble = valueToWrite & 0xF;
			valueToWrite = valueToWrite >> 4;
			compressedBuf[compressedBufIndex] |= nibble << ( bitsLeftInCompressBufByte - 4 );

			bitsLeftInCompressBufByte -= 4;
			bitsLeftInValueToWrite -= 4;

			if ( bitsLeftInCompressBufByte == 0 )
			{
				compressedBufIndex++;
				bitsLeftInCompressBufByte = 8;
			}
		}
	}

	return true;
}

// returns true if buffer sizes are appropriate, false otherwise
inline bool B12Decompress (uint8_t* compressedBuf, const unsigned int compressedBufSize,
				uint16_t* decompressedBuf, const unsigned int decompressedBufSize)
{
	// first check if the uncompressed size fits into the compressed size once compressed
	if ( ((decompressedBufSize * 2) / 4) * 3 != compressedBufSize ) return false;

	// zero decompressedBuf
	std::fill( decompressedBuf, decompressedBuf + decompressedBufSize, 0 );

	// unpack the data
	unsigned int compressedBufIndex = 0;
	uint8_t bitsLeftInCompressBufByte = 8;
	uint16_t compressedBufValue = static_cast<uint16_t>( compressedBuf[compressedBufIndex] );
	for ( unsigned int sample = 0; sample < decompressedBufSize; sample++ )
	{
		uint8_t bitsReadInValueToRead = 0;
		uint16_t valueToRead = 0;

		while ( bitsReadInValueToRead != 12 )
		{
			// read in nibbles
			valueToRead |= ( (compressedBufValue >> 4) & 0xF ) << ( bitsReadInValueToRead );
			compressedBufValue = compressedBufValue << 4;

			bitsLeftInCompressBufByte -= 4;
			bitsReadInValueToRead += 4;

			if ( bitsLeftInCompressBufByte == 0 )
			{
				compressedBufIndex++;
				compressedBufValue = compressedBuf[compressedBufIndex];
				bitsLeftInCompressBufByte = 8;
			}
		}

		decompressedBuf[sample] = valueToRead;
	}

	return true;
}

#endif // B12COMPRESSION_HPP
