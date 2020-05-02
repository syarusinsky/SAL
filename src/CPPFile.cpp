#include "CPPFile.hpp"

CPPFile::CPPFile (const std::string& fileName) :
	m_FileName( fileName ),
	m_NeedsInitialization( false )
{
	m_File.open( "./" + m_FileName, std::fstream::in | std::fstream::out | std::ios::binary );
	if ( !m_File.is_open() )
	{
		m_NeedsInitialization = true;
	}
}

CPPFile::~CPPFile()
{
	m_File.close();
}

void CPPFile::writeToMedia (const Variant& data, const unsigned int sizeInBytes, const unsigned int offsetInBytes)
{
	m_File.seekp( offsetInBytes );
	m_File.write( reinterpret_cast<char*>(data.getRaw()), sizeInBytes );
}

Variant CPPFile::readFromMedia (const unsigned int sizeInBytes, const unsigned int offsetInBytes)
{
	char* data = new char[sizeInBytes];

	m_File.seekg( offsetInBytes );
	m_File.read( data, sizeInBytes );

	Variant retVal( static_cast<void*>(data) );

	return retVal;
}

bool CPPFile::needsInitialization()
{
	return m_NeedsInitialization;
}

void CPPFile::initialize()
{
	m_File.open( "./" + m_FileName, std::fstream::out | std::ios::binary );
}

void CPPFile::afterInitialize()
{
	m_File.close();
	m_File.open( "./" + m_FileName, std::fstream::in | std::fstream::out | std::ios::binary );
}
