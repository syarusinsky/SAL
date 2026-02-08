#include "SampleRateConverter.hpp"

#include <stdint.h>
#include <cmath>

template <typename SType, typename TType>
SampleRateConverter<SType, TType>::SampleRateConverter (const unsigned int initialSourceRate, const unsigned int initialTargetRate,
							const unsigned int initialSourceBufferSize, const unsigned int maxTargetBufferSize) :
	m_SourceRate( initialSourceRate ),
	m_TargetRate( initialTargetRate ),
	m_SourceBufferSize( initialSourceBufferSize ),
	m_TargetBufferSize( std::min(maxTargetBufferSize, getTargetBufferSizePerSourceBuffer()) ),
	m_SourceToTargetSourceIncr( 0.0f ),
	m_SourceToTargetTargetIncr( 0.0f ),
	m_TargetToSourceTargetIncr( 0.0f ),
	m_TargetToSourceSourceIncr( 0.0f ),
	m_SourceToTargetDownsamplingAAFilter(
			// cutoff
			m_TargetRate / 2,
			// sample rate
			m_SourceRate,
			// filter order
			63,
			// buffer size
			m_SourceBufferSize ),
	m_SourceToTargetUpsamplingAAFilter(
			// cutoff
			m_TargetRate / 2,
			// sample rate
			m_TargetRate,
			// filter order
			63,
			// buffer size
			m_TargetBufferSize ),
	m_TargetToSourceDownsamplingAAFilter(
			// cutoff
			m_SourceRate / 2,
			// sample rate
			m_TargetRate,
			// filter order
			63,
			// buffer size
			m_TargetBufferSize ),
	m_TargetToSourceUpsamplingAAFilter(
			// cutoff
			m_SourceRate / 2,
			// sample rate
			m_SourceRate,
			// filter order
			63,
			// buffer size
			m_SourceBufferSize )
{
}

template <typename SType, typename TType>
bool SampleRateConverter<SType, TType>::convertFromSourceToTarget (const SType* const sourceBuffer, TType* const targetBuffer)
{
	// this takes place first so that the user can see after a function call if the target incr is out of target buffer bounds
	m_SourceToTargetTargetIncr = std::fmod( m_SourceToTargetTargetIncr, static_cast<float>(m_TargetBufferSize) );

	const float targetSamplesPerSourceSample = static_cast<float>( m_TargetRate ) / static_cast<float>( m_SourceRate );

	// zero out target buffer (zero-stuffing if necessary)
	for ( unsigned int sample = static_cast<unsigned int>(m_SourceToTargetTargetIncr); sample < m_TargetBufferSize; sample++ )
	{
		targetBuffer[sample] = getTargetZeroPoint();
	}

	for ( float& sample = m_SourceToTargetTargetIncr;
			(sample < m_TargetBufferSize)
			&& (static_cast<unsigned int>(m_SourceToTargetSourceIncr) < m_SourceBufferSize); // either might fill first
			sample += targetSamplesPerSourceSample )
	{
		targetBuffer[ static_cast<unsigned int>(sample) ]
			= convertSourceToTargetType( sourceBuffer[static_cast<unsigned int>(m_SourceToTargetSourceIncr)] );

		m_SourceToTargetSourceIncr += 1.0f;
	}

	if ( static_cast<unsigned int>(m_SourceToTargetSourceIncr) >= m_SourceBufferSize )
	{
		// to keep sampling consistent, we need the modulo value for the next source buffer
		m_SourceToTargetSourceIncr = std::fmod( m_SourceToTargetSourceIncr, static_cast<float>(m_SourceBufferSize) );

		return true;
	}

	return false;
}

template <typename SType, typename TType>
bool SampleRateConverter<SType, TType>::convertFromTargetToSource (const TType* const targetBuffer, SType* const sourceBuffer)
{
	// this takes place first so that the user can see after a function call if the target incr is out of target buffer bounds
	m_TargetToSourceTargetIncr = std::fmod( m_TargetToSourceTargetIncr, static_cast<float>(m_TargetBufferSize) );

	const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );

	// zero out source buffer from m_TargetToSourceSourceIncr to m_SourceBufferSize (zero-stuffing if necessary)
	for ( unsigned int sample = static_cast<unsigned int>(m_TargetToSourceSourceIncr); sample < m_SourceBufferSize; sample++ )
	{
		sourceBuffer[sample] = getSourceZeroPoint();
	}

	for ( float& sample = m_TargetToSourceSourceIncr;
			(sample < m_SourceBufferSize)
			&& (static_cast<unsigned int>(m_TargetToSourceTargetIncr) < m_TargetBufferSize); // either might fill first
			sample += sourceSamplesPerTargetSample )
	{
		sourceBuffer[ static_cast<unsigned int>(sample) ]
			= convertTargetToSourceType( targetBuffer[static_cast<unsigned int>(m_TargetToSourceTargetIncr)] );

		m_TargetToSourceTargetIncr += 1.0f;
	}

	if ( static_cast<unsigned int>(m_TargetToSourceSourceIncr) >= m_SourceBufferSize )
	{
		// to keep sampling consistent, we need the modulo value for the next source buffer
		m_TargetToSourceSourceIncr = std::fmod( m_TargetToSourceSourceIncr, static_cast<float>(m_SourceBufferSize) );

		return true;
	}

	return false;
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::filterSourceToTargetDownsampling (SType* const buffer)
{
	m_SourceToTargetDownsamplingAAFilter.call( buffer );
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::filterSourceToTargetUpsampling (TType* const buffer)
{
	m_SourceToTargetUpsamplingAAFilter.call( buffer );
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::filterTargetToSourceDownsampling (TType* const buffer)
{
	m_TargetToSourceDownsamplingAAFilter.call( buffer );
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::filterTargetToSourceUpsampling (SType* const buffer)
{
	m_TargetToSourceUpsamplingAAFilter.call( buffer );
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::resetAAFilters()
{
	m_SourceToTargetDownsamplingAAFilter.changeValues(
			// cutoff
			m_TargetRate / 2,
			// sample rate
			m_SourceRate,
			// filter order
			63,
			// buffer size
			m_SourceBufferSize );
	m_SourceToTargetUpsamplingAAFilter.changeValues(
			// cutoff
			m_TargetRate / 2,
			// sample rate
			m_TargetRate,
			// filter order
			63,
			// buffer size
			m_TargetBufferSize );
	m_TargetToSourceDownsamplingAAFilter.changeValues(
			// cutoff
			m_SourceRate / 2,
			// sample rate
			m_TargetRate,
			// filter order
			63,
			// buffer size
			m_TargetBufferSize );
	m_TargetToSourceUpsamplingAAFilter.changeValues(
			// cutoff
			m_SourceRate / 2,
			// sample rate
			m_SourceRate,
			// filter order
			63,
			// buffer size
			m_SourceBufferSize );
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::setTargetBufferSize (const unsigned int maxTargetBufferSize)
{
	m_TargetBufferSize = std::min( maxTargetBufferSize, getTargetBufferSizePerSourceBuffer() );
}

template<>
constexpr float SampleRateConverter<float, float>::convertSourceToTargetType (float sourceType)
{
	return sourceType;
}

template<>
constexpr uint16_t SampleRateConverter<float, uint16_t>::convertSourceToTargetType (float sourceType)
{
	return static_cast<uint16_t>( (sourceType + 1.0f) * 32767.0f );
}

template<>
constexpr int16_t SampleRateConverter<float, int16_t>::convertSourceToTargetType (float sourceType)
{
	return static_cast<uint16_t>( sourceType * 32767.0f );
}

template<>
constexpr float SampleRateConverter<float, float>::convertTargetToSourceType (float targetType)
{
	return targetType;
}

template<>
constexpr float SampleRateConverter<float, uint16_t>::convertTargetToSourceType (uint16_t targetType)
{
	return static_cast<float>( targetType - 32768 ) * ( 1.0f / 32768.0f );
}

template<>
constexpr float SampleRateConverter<float, int16_t>::convertTargetToSourceType (int16_t targetType)
{
	return static_cast<float>( targetType ) * ( 1.0f / 32768.0f );
}

template<>
constexpr float SampleRateConverter<float, float>::getTargetZeroPoint()
{
	return 0.0f;
}

template<>
constexpr uint16_t SampleRateConverter<float, uint16_t>::getTargetZeroPoint()
{
	return 32767;
}

template<>
constexpr int16_t SampleRateConverter<float, int16_t>::getTargetZeroPoint()
{
	return 0;
}

template<>
constexpr float SampleRateConverter<float, float>::getSourceZeroPoint()
{
	return 0.0f;
}

template<>
constexpr float SampleRateConverter<float, uint16_t>::getSourceZeroPoint()
{
	return 0.0f;
}

template<>
constexpr float SampleRateConverter<float, int16_t>::getSourceZeroPoint()
{
	return 0.0f;
}

// avoid linker errors
template class SampleRateConverter<float, float>;
template class SampleRateConverter<float, uint16_t>;
template class SampleRateConverter<float, int16_t>;
