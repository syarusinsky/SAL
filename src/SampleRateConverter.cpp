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
	m_TargetToSourceTargetIncr( 0.0f ),
	m_TargetToSourceSourceIncr( 0.0f ),
	m_SourceToTargetPhaseAccumulation( 0.0f ),
	m_PhaseAccumulated( false ),
	m_WorkingTargetBufferLastSample( getTargetZeroPoint() ),
	m_WorkingSourceBufferLastSample( getSourceZeroPoint() ),
	m_WorkingTargetBufferSize( m_TargetBufferSize ),
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
bool SampleRateConverter<SType, TType>::convertFromSourceToTarget (const SType* const sourceBuffer, TType* const targetBuffer)
{
	m_PhaseAccumulated = false;

	const float targetSamplesPerSourceSample = static_cast<float>( m_TargetRate ) / static_cast<float>( m_SourceRate );

	// to keep sampling consistent, we need the modulo value for the next source buffer
	m_SourceToTargetTargetIncr = std::fmod( m_SourceToTargetTargetIncr, m_WorkingTargetBufferSize );
	const float targetBufferSizeFraction = std::fmod( m_TargetBufferSize, std::floor(m_TargetBufferSize) );
	const bool fractionalTargetBufferIsLarger = targetBufferSizeFraction < 0.5f;
	m_SourceToTargetPhaseAccumulation += targetBufferSizeFraction;
	m_SourceToTargetSourceIncr = 0.0f;

	// if target buffer is fractional, we alternate target buffer sizes between the fraction
	const float phaseAccumulationModulo = std::fmod( m_SourceToTargetPhaseAccumulation, 1.0f );
	const bool needToUseFractionalTargetBufferSize = ( phaseAccumulationModulo + targetBufferSizeFraction ) >= 1.0f;
	m_WorkingTargetBufferSize = m_TargetBufferSize;
	if ( needToUseFractionalTargetBufferSize )
	{
		m_WorkingTargetBufferSize = (fractionalTargetBufferIsLarger) ? m_TargetBufferSize + 1.0f : m_TargetBufferSize - 1.0f;
	}
	m_WorkingTargetBufferSize = std::floor( m_WorkingTargetBufferSize );

	// we may need to carry over the last source buffer's sample if the target buffer size is fractional
	if ( (needToUseFractionalTargetBufferSize && fractionalTargetBufferIsLarger)
		|| (! needToUseFractionalTargetBufferSize && ! fractionalTargetBufferIsLarger) )
	{
		targetBuffer[ static_cast<unsigned int>(m_SourceToTargetTargetIncr) ] = m_WorkingSourceBufferLastSample;
		m_SourceToTargetTargetIncr += targetSamplesPerSourceSample;
	}

	// zero out target buffer (zero-stuffing if necessary)
	for ( float sample = m_SourceToTargetTargetIncr; sample < m_WorkingTargetBufferSize; sample += 1.0f )
	{
		targetBuffer[static_cast<unsigned int>(sample)] = getTargetZeroPoint();
	}

	for ( float& sample = m_SourceToTargetTargetIncr;
			sample < m_WorkingTargetBufferSize;
			sample += targetSamplesPerSourceSample )
	{
		targetBuffer[ static_cast<unsigned int>(sample) ]
			= convertSourceToTargetType( sourceBuffer[static_cast<unsigned int>(m_SourceToTargetSourceIncr)] );

		m_SourceToTargetSourceIncr += 1.0f;
	}

	m_WorkingSourceBufferLastSample = sourceBuffer[ static_cast<unsigned int>(m_SourceToTargetSourceIncr) ];

	// after a full fractional cycle, reset incrs to prevent floating point accumulation errors
	const float fullCycleVal = targetBufferSizeFraction
					* ( (fractionalTargetBufferIsLarger)
						? (1.0f / targetBufferSizeFraction)
						: (1.0f / (1.0f - targetBufferSizeFraction)) );
	// + targetBufferSizeFraction to account for floating point accumulation errors
	if ( m_SourceToTargetPhaseAccumulation + targetBufferSizeFraction >= fullCycleVal )
	{
		m_SourceToTargetTargetIncr = 0.0f;
		m_SourceToTargetSourceIncr = 0.0f;
		m_SourceToTargetPhaseAccumulation = 0.0f;

		// also prevent accumulation errors for target-to-source conversion
		m_PhaseAccumulated = true;
	}

	return true;
}

template <typename SType, typename TType>
bool SampleRateConverter<SType, TType>::convertFromTargetToSource (const TType* const targetBuffer, SType* const sourceBuffer)
{
	const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );

	// to keep sampling consistent, we need the modulo value for the next source buffer
	m_TargetToSourceSourceIncr = std::fmod( m_TargetToSourceSourceIncr, static_cast<float>(m_SourceBufferSize) );
	m_TargetToSourceTargetIncr = 0;

	// zero out source buffer from m_TargetToSourceSourceIncr to m_SourceBufferSize (zero-stuffing if necessary)
	for ( unsigned int sample = 0; sample < m_SourceBufferSize; sample++ )
	{
		sourceBuffer[sample] = getSourceZeroPoint();
	}

	for ( float& sample = m_TargetToSourceSourceIncr;
			sample < m_SourceBufferSize;
			sample += sourceSamplesPerTargetSample )
	{
		sourceBuffer[ static_cast<unsigned int>(sample) ]
			= convertTargetToSourceType( targetBuffer[static_cast<unsigned int>(m_TargetToSourceTargetIncr)] );

		m_TargetToSourceTargetIncr += 1.0f;
	}

	// prevent floating point accumulation errors
	if ( m_PhaseAccumulated )
	{
		// if downsampling from source to target, we'll be on the tail end of each "sample", so we set the source incr to be too
		if ( ! sourceToTargetIsUpsampling() )
		{
			const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );
			m_TargetToSourceSourceIncr = sourceSamplesPerTargetSample;
		}
		else
		{
			m_TargetToSourceSourceIncr = 0.0f;
		}
	}

	return true;
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
	m_WorkingTargetBufferSize = m_TargetBufferSize;
	m_SourceToTargetTargetIncr = 0.0f;
	m_SourceToTargetSourceIncr = 0.0f;
	m_TargetToSourceTargetIncr = 0.0f;
	m_SourceToTargetPhaseAccumulation = 0.0f;
	m_PhaseAccumulated = false;

	// if downsampling from source to target, we'll be on the tail end of each "sample", so we set the source incr to be too
	if ( ! sourceToTargetIsUpsampling() )
	{
		const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );
		m_TargetToSourceSourceIncr = sourceSamplesPerTargetSample;
	}
	else
	{
		m_TargetToSourceSourceIncr = 0.0f;
	}
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::setTargetRate (const unsigned int targetRate)
{
	m_TargetRate = targetRate;

	m_TargetBufferSize = getTargetBufferSizePerSourceBuffer();
	m_WorkingTargetBufferLastSample = getTargetZeroPoint();
	m_WorkingSourceBufferLastSample = getSourceZeroPoint();
	m_WorkingTargetBufferSize = m_TargetBufferSize;
	m_SourceToTargetTargetIncr = 0.0f;
	m_SourceToTargetSourceIncr = 0.0f;
	m_TargetToSourceTargetIncr = 0.0f;
	m_SourceToTargetPhaseAccumulation = 0.0f;
	m_PhaseAccumulated = false;

	// if downsampling from source to target, we'll be on the tail end of each "sample", so we set the source incr to be too
	if ( ! sourceToTargetIsUpsampling() )
	{
		const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );
		m_TargetToSourceSourceIncr = sourceSamplesPerTargetSample;
	}
	else
	{
		m_TargetToSourceSourceIncr = 0.0f;
	}
}

template <typename SType, typename TType>
void SampleRateConverter<SType, TType>::setSourceBufferSize (const unsigned int bufferSize)
{
	m_SourceBufferSize = bufferSize;

	m_TargetBufferSize = getTargetBufferSizePerSourceBuffer();
	m_WorkingTargetBufferLastSample = getTargetZeroPoint();
	m_WorkingSourceBufferLastSample = getSourceZeroPoint();
	m_WorkingTargetBufferSize = m_TargetBufferSize;
	m_SourceToTargetTargetIncr = 0.0f;
	m_SourceToTargetSourceIncr = 0.0f;
	m_TargetToSourceTargetIncr = 0.0f;
	m_SourceToTargetPhaseAccumulation = 0.0f;
	m_PhaseAccumulated = false;

	// if downsampling from source to target, we'll be on the tail end of each "sample", so we set the source incr to be too
	if ( ! sourceToTargetIsUpsampling() )
	{
		const float sourceSamplesPerTargetSample = static_cast<float>( m_SourceRate ) / static_cast<float>( m_TargetRate );
		m_TargetToSourceSourceIncr = sourceSamplesPerTargetSample;
	}
	else
	{
		m_TargetToSourceSourceIncr = 0.0f;
	}
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
	// ceil, since the target buffer needs to accomodate any fractional overflow
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
