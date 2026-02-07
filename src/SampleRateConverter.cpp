#include "SampleRateConverter.hpp"

#include <stdint.h>
#include <cmath>

template <typename SType, typename TType>
SampleRateConverter<SType, TType>::SampleRateConverter (const unsigned int initialSourceRate, const unsigned int initialTargetRate,
							const unsigned int initialSourceBufferSize, const unsigned int initialTargetBufferSize) :
	m_SourceRate( initialSourceRate ),
	m_TargetRate( initialTargetRate ),
	m_SourceBufferSize( initialSourceBufferSize ),
	m_TargetBufferSize( initialTargetBufferSize ),
	m_SourceToTargetSourceIncr( 0.0f ),
	m_SourceToTargetTargetIncr( 0.0f ),
	m_TargetToSourceTargetIncr( 0.0f ),
	m_TargetToSourceSourceIncr( 0.0f )
{
}

template <typename SType, typename TType>
bool SampleRateConverter<SType, TType>::convertFromSourceToTarget (const SType* const sourceBuffer, TType* const targetBuffer)
{
	// this takes place first so that the user can see after a function call if the target incr is out of target buffer bounds
	m_SourceToTargetTargetIncr = std::fmod( m_SourceToTargetTargetIncr, static_cast<float>(m_TargetBufferSize) );

	const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );

	for ( float& sample = m_SourceToTargetSourceIncr;
			(sample < m_SourceBufferSize)
			&& (static_cast<unsigned int>(m_SourceToTargetTargetIncr) < m_TargetBufferSize); // either might fill first
			sample += sourceSamplesPerTargetSample )
	{
		targetBuffer[ static_cast<unsigned int>(m_SourceToTargetTargetIncr) ]
			= convertSourceToTargetType( sourceBuffer[static_cast<unsigned int>(sample)] );

		m_SourceToTargetTargetIncr += 1.0f;
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

	const float targetSamplesPerSourceSample = static_cast<float>( m_TargetRate ) / static_cast<float>( m_SourceRate );

	for ( float& sample = m_TargetToSourceTargetIncr;
			(sample < m_TargetBufferSize)
			&& (static_cast<unsigned int>(m_TargetToSourceSourceIncr) < m_SourceBufferSize); // either might fill first
			sample += targetSamplesPerSourceSample )
	{
		sourceBuffer[ static_cast<unsigned int>(m_TargetToSourceSourceIncr) ]
			= convertTargetToSourceType( targetBuffer[static_cast<unsigned int>(sample)] );

		m_TargetToSourceSourceIncr += 1.0f;
	}

	if ( static_cast<unsigned int>(m_TargetToSourceSourceIncr) >= m_SourceBufferSize )
	{
		m_TargetToSourceSourceIncr = 0;

		return true;
	}

	return false;
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
	return static_cast<float>( targetType ) * ( 1.0f / 65535.0f );
}

template<>
constexpr float SampleRateConverter<float, int16_t>::convertTargetToSourceType (int16_t targetType)
{
	return static_cast<float>( targetType ) * ( 1.0f / 32767.0f );
}

// avoid linker errors
template class SampleRateConverter<float, float>;
template class SampleRateConverter<float, uint16_t>;
template class SampleRateConverter<float, int16_t>;
