#include "SampleRateConverter.hpp"

#include <stdint.h>
#include <cmath>

template <typename SType, typename TType>
SampleRateConverter<SType, TType>::SampleRateConverter (const unsigned int initialSourceRate, const unsigned int initialTargetRate,
							const unsigned int initialSourceBufferSize) :
	m_SourceRate( initialSourceRate ),
	m_TargetRate( initialTargetRate ),
	m_SourceBufferSize( initialSourceBufferSize ),
	m_TargetBufferSize( getTargetBufferSizePerSourceBuffer() ),
	m_SourceToTargetSourceIncr( 0.0f ),
	m_SourceToTargetTargetIncr( 0.0f ),
	m_TargetToSourceTargetIncr( -1.0f ), // since we begin delayed by one target sample
	m_TargetToSourceSourceIncr( 0.0f ),
	m_TargetToSourceEmittedSamples(),
	m_WorkingTargetBufferLastSample( getTargetZeroPoint() ),
	m_WorkingSourceBufferLastSample( getSourceZeroPoint() ),
	m_SourceToTargetDownsamplingAAFilter(
			// cutoff
			m_TargetRate / 2,
			// sample rate
			m_SourceRate,
			// filter order
			63 ),
	m_SourceToTargetUpsamplingAAFilter(
			// cutoff
			m_TargetRate / 2,
			// sample rate
			m_TargetRate,
			// filter order
			63 ),
	m_TargetToSourceDownsamplingAAFilter(
			// cutoff
			m_SourceRate / 2,
			// sample rate
			m_TargetRate,
			// filter order
			63 ),
	m_TargetToSourceUpsamplingAAFilter(
			// cutoff
			m_SourceRate / 2,
			// sample rate
			m_SourceRate,
			// filter order
			63 )
{
	// if downsampling from source to target, we'll be on the tail end of each "sample", so we set the source incr to be too
	if ( ! sourceToTargetIsUpsampling() )
	{
		const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );
		m_TargetToSourceSourceIncr = sourceSamplesPerTargetSample;
	}
}

template <typename SType, typename TType>
unsigned int SampleRateConverter<SType, TType>::convertFromSourceToTargetDownsampling (const SType* const sourceBuffer, TType* const targetBuffer)
{
	const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );

	unsigned int samplesConverted = 0;

	// to keep sampling consistent, we need the modulo value for the next source buffer
	m_SourceToTargetSourceIncr = std::fmod( m_SourceToTargetSourceIncr, m_SourceBufferSize );
	m_SourceToTargetTargetIncr = 0.0f;

	// we may need to linearly interpolate between the first sample of this buffer and the last sample of the previous buffer
	if ( m_SourceToTargetSourceIncr < 1.0f )
	{
		// linearly interpolate
		const float fraction = std::fmod( m_SourceToTargetSourceIncr, 1.0f );
		const float lowerAmt = 1.0f - fraction;
		const float upperAmt = 1.0f - lowerAmt;
		const float lowerSourceBufferVal = m_WorkingSourceBufferLastSample;
		const float upperSourceBufferVal = sourceBuffer[ static_cast<unsigned int>(std::floor(m_SourceToTargetSourceIncr)) ];
		const float lowerVal = lowerAmt * lowerSourceBufferVal;
		const float upperVal = upperAmt * upperSourceBufferVal;
		const SType sampleVal = static_cast<SType>( lowerVal + upperVal );

		targetBuffer[0] = convertSourceToTargetType( sampleVal );
		m_SourceToTargetTargetIncr += 1.0f;

		m_SourceToTargetSourceIncr += sourceSamplesPerTargetSample;

		samplesConverted++;
	}

	// buffer incr - 1.0f because we need valid indices and we're essentially delaying by one sample
	for ( float& sample = m_SourceToTargetTargetIncr; m_SourceToTargetSourceIncr < m_SourceBufferSize; sample += 1.0f )
	{

		// linearly interpolate
		const float fraction = std::fmod( m_SourceToTargetSourceIncr, 1.0f );
		const float lowerAmt = 1.0f - fraction;
		const float upperAmt = 1.0f - lowerAmt;
		const float lowerSourceBufferVal = sourceBuffer[ static_cast<unsigned int>(std::floor(m_SourceToTargetSourceIncr - 1.0f)) ];
		const float upperSourceBufferVal = sourceBuffer[ static_cast<unsigned int>(std::ceil(m_SourceToTargetSourceIncr - 1.0f)) ];
		const float lowerVal = lowerAmt * lowerSourceBufferVal;
		const float upperVal = upperAmt * upperSourceBufferVal;
		const SType sampleVal = static_cast<SType>( lowerVal + upperVal );

		targetBuffer[ static_cast<unsigned int>(sample) ] = convertSourceToTargetType( sampleVal );

		m_SourceToTargetSourceIncr += sourceSamplesPerTargetSample;

		samplesConverted++;
	}

	// save last sample value in case the next buffer needs it
	const unsigned int lastSampleIndex = m_SourceBufferSize - 1;
	m_WorkingSourceBufferLastSample = sourceBuffer[ lastSampleIndex ];

	return samplesConverted;
}

template <typename SType, typename TType>
unsigned int SampleRateConverter<SType, TType>::convertFromTargetToSourceUpsampling (const TType* const targetBuffer,
											const unsigned int targetBufferSize,
											SType* const sourceBuffer)
{
	const float targetSamplesPerSourceSample = static_cast<float>( m_TargetRate ) / static_cast<float>( m_SourceRate );

	unsigned int samplesConverted = 0;

	// samples that are generated, but to be used in the next source buffer since they don't fit in this one
	unsigned int samplesEmitted = 0;

	m_TargetToSourceSourceIncr = 0.0f;

	// we may need to fill this buffer with samples generated from the last buffer if the buffer size is fractional
	for ( const SType sampleVal : m_TargetToSourceEmittedSamples )
	{
		sourceBuffer[ static_cast<unsigned int>(m_TargetToSourceSourceIncr) ] = sampleVal;

		m_TargetToSourceSourceIncr += 1.0f;
		m_TargetToSourceTargetIncr += targetSamplesPerSourceSample;
	}

	// clear emitted samples since they've been used and we may emit new samples for this buffer
	m_TargetToSourceEmittedSamples.clear();

	// we may need to linearly interpolate between the first sample of this buffer and the last sample of the previous buffer
	for ( float& sample = m_TargetToSourceTargetIncr; sample < 0.0f; sample += targetSamplesPerSourceSample )
	{
		// this value should be between 0 and 1
		const float tempTargetToSourceTargetIncr = sample + 1.0f;

		// linearly interpolate
		const float fraction = std::fmod( tempTargetToSourceTargetIncr, 1.0f );
		const float lowerAmt = 1.0f - fraction;
		const float upperAmt = 1.0f - lowerAmt;
		const float lowerTargetBufferVal = m_WorkingTargetBufferLastSample;
		const float upperTargetBufferVal = targetBuffer[ static_cast<unsigned int>(std::floor(tempTargetToSourceTargetIncr)) ];
		const float lowerVal = lowerAmt * lowerTargetBufferVal;
		const float upperVal = upperAmt * upperTargetBufferVal;
		const TType sampleVal = static_cast<TType>( lowerVal + upperVal );

		sourceBuffer[ static_cast<unsigned int>(m_TargetToSourceSourceIncr) ] = convertTargetToSourceType( sampleVal );

		m_TargetToSourceSourceIncr += 1.0f;

		samplesConverted++;
	}

	for ( float& sample = m_TargetToSourceTargetIncr;
			sample < static_cast<float>(targetBufferSize - 1);
			sample += targetSamplesPerSourceSample )
	{
		// linearly interpolate
		const float fraction = std::fmod( m_TargetToSourceTargetIncr, 1.0f );
		const float lowerAmt = 1.0f - fraction;
		const float upperAmt = 1.0f - lowerAmt;
		const float lowerTargetBufferVal = targetBuffer[ static_cast<unsigned int>(std::floor(m_TargetToSourceTargetIncr)) ];
		const float upperTargetBufferVal = targetBuffer[ static_cast<unsigned int>(std::ceil(m_TargetToSourceTargetIncr)) ];
		const float lowerVal = lowerAmt * lowerTargetBufferVal;
		const float upperVal = upperAmt * upperTargetBufferVal;
		const TType sampleVal = static_cast<TType>( lowerVal + upperVal );

		// we may be emitting samples that don't fit in this source buffer if the target buffer size is fractional
		if ( m_TargetToSourceSourceIncr > static_cast<float>(m_SourceBufferSize - 1) )
		{
			m_TargetToSourceEmittedSamples.push_back( convertTargetToSourceType(sampleVal) );
			samplesEmitted++;
		}
		else
		{
			sourceBuffer[ static_cast<unsigned int>(m_TargetToSourceSourceIncr) ] = convertTargetToSourceType( sampleVal );
		}

		m_TargetToSourceSourceIncr += 1.0f;

		samplesConverted++;
	}

	// to keep sampling consistent, we need the modulo value for the next target buffer
	// we subtract by once since we want to use the previous buffer's last sample, then by samplesEmitted * targetSamplesPerSourceSample
	// because we will also be adding the emitted samples to the next buffer
	m_TargetToSourceTargetIncr = std::fmod( m_TargetToSourceTargetIncr, static_cast<float>(targetBufferSize - 1) )
					- 1.0f - ( static_cast<float>(samplesEmitted) * targetSamplesPerSourceSample );

	// save last sample value in case the next buffer needs it
	const unsigned int lastSampleIndex = targetBufferSize - 1;
	m_WorkingTargetBufferLastSample = targetBuffer[ lastSampleIndex ];

	return samplesConverted;
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::filterSourceToTargetDownsampling (SType* const buffer)
{
	m_SourceToTargetDownsamplingAAFilter.call( buffer, m_SourceBufferSize );
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::filterSourceToTargetUpsampling (TType* const buffer, const unsigned int bufferSize)
{
	m_SourceToTargetUpsamplingAAFilter.call( buffer, bufferSize );
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::filterTargetToSourceDownsampling (TType* const buffer, const unsigned int bufferSize)
{
	m_TargetToSourceDownsamplingAAFilter.call( buffer, bufferSize );
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::filterTargetToSourceUpsampling (SType* const buffer)
{
	m_TargetToSourceUpsamplingAAFilter.call( buffer, m_SourceBufferSize );
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::setSourceRate (const unsigned int sourceRate)
{
	m_SourceRate = sourceRate;

	m_TargetBufferSize = getTargetBufferSizePerSourceBuffer();
	m_WorkingTargetBufferLastSample = getTargetZeroPoint();
	m_WorkingSourceBufferLastSample = getSourceZeroPoint();
	m_SourceToTargetTargetIncr = 0.0f;
	m_SourceToTargetSourceIncr = 0.0f;
	m_TargetToSourceTargetIncr = -1.0f; // since we begin delayed by one target sample

	m_TargetToSourceSourceIncr = 0.0f;
	// if downsampling from source to target, we'll be on the tail end of each "sample", so we set the source incr to be too
	if ( ! sourceToTargetIsUpsampling() )
	{
		const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );
		m_TargetToSourceSourceIncr = sourceSamplesPerTargetSample;
	}

	m_TargetToSourceEmittedSamples.clear();
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::setTargetRate (const unsigned int targetRate)
{
	m_TargetRate = targetRate;

	m_TargetBufferSize = getTargetBufferSizePerSourceBuffer();
	m_WorkingTargetBufferLastSample = getTargetZeroPoint();
	m_WorkingSourceBufferLastSample = getSourceZeroPoint();
	m_SourceToTargetTargetIncr = 0.0f;
	m_SourceToTargetSourceIncr = 0.0f;
	m_TargetToSourceTargetIncr = -1.0f; // since we begin delayed by one target sample

	m_TargetToSourceSourceIncr = 0.0f;
	// if downsampling from source to target, we'll be on the tail end of each "sample", so we set the source incr to be too
	if ( ! sourceToTargetIsUpsampling() )
	{
		const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );
		m_TargetToSourceSourceIncr = sourceSamplesPerTargetSample;
	}

	m_TargetToSourceEmittedSamples.clear();
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::setSourceBufferSize (const unsigned int bufferSize)
{
	m_SourceBufferSize = bufferSize;

	m_TargetBufferSize = getTargetBufferSizePerSourceBuffer();
	m_WorkingTargetBufferLastSample = getTargetZeroPoint();
	m_WorkingSourceBufferLastSample = getSourceZeroPoint();
	m_SourceToTargetTargetIncr = 0.0f;
	m_SourceToTargetSourceIncr = 0.0f;
	m_TargetToSourceTargetIncr = -1.0f; // since we begin delayed by one target sample

	m_TargetToSourceSourceIncr = 0.0f;
	// if downsampling from source to target, we'll be on the tail end of each "sample", so we set the source incr to be too
	if ( ! sourceToTargetIsUpsampling() )
	{
		const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );
		m_TargetToSourceSourceIncr = sourceSamplesPerTargetSample;
	}

	m_TargetToSourceEmittedSamples.clear();
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
			63 );
	m_SourceToTargetUpsamplingAAFilter.changeValues(
			// cutoff
			m_TargetRate / 2,
			// sample rate
			m_TargetRate,
			// filter order
			63 );
	m_TargetToSourceDownsamplingAAFilter.changeValues(
			// cutoff
			m_SourceRate / 2,
			// sample rate
			m_TargetRate,
			// filter order
			63 );
	m_TargetToSourceUpsamplingAAFilter.changeValues(
			// cutoff
			m_SourceRate / 2,
			// sample rate
			m_SourceRate,
			// filter order
			63 );
}

template <typename SType, typename TType>
float SampleRateConverter<SType, TType>::getTargetBufferSizePerSourceBuffer() const
{
	return ( static_cast<float>(m_TargetRate) / static_cast<float>(m_SourceRate) ) * static_cast<float>(m_SourceBufferSize);
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
