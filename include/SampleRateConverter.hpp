#ifndef SAMPLERATECONVERTER_HPP
#define SAMPLERATECONVERTER_HPP

/*******************************************************************************
 * A simple sample rate converter for converting plugin sample rates (source)
 * to the sample rates of SAL gear (target). Obviously this doesn't just have
 * to be used with SAL stuff. This component is treated as handling both input
 * (source-to-target) and output (target-to-source). The user will have to
 * manage feeding in more than one target buffer in the likely case of buffer
 * size mismatches. This should all be in the domain of the source's buffer size,
 * hence why convertFromSourceToTarget returns true if the source buffer is fully
 * converted and why convertFromTargetToSource returns true if the source buffer
 * is fully filled.
 *
 * Note that this sample rate converter uses zero-stuffing instead of zero-order
 * hold. This is because zero-order hold introduces distortion.
 *
 * For typical implementation, look at getNextAudioBlock in MainComponent.cpp in
 * the AkiDelay project.
*******************************************************************************/

#include "AntiAliasingFilter.hpp"

template <typename SType, typename TType>
class SampleRateConverter
{
	public:
		SampleRateConverter (const unsigned int initialSourceRate, const unsigned int initialTargetRate,
					const unsigned int initialSourceBufferSize, const unsigned int maxTargetBufferSize);
		~SampleRateConverter() {}

		bool convertFromSourceToTarget (const SType* const sourceBuffer, TType* const targetBuffer); // true if source buffer fully converted
		bool convertFromTargetToSource (const TType* const targetBuffer, SType* const sourceBuffer); // true if source buffer fully filled

		// these may need to be flipflopped, depending on if upsampling or downsampling
		void filterSourceToTargetDownsampling (SType* const buffer);
		void filterSourceToTargetUpsampling (TType* const buffer);
		void filterTargetToSourceDownsampling (TType* const buffer);
		void filterTargetToSourceUpsampling (SType* const buffer);

		void setSourceRate (const unsigned int sourceRate) { m_SourceRate = sourceRate; }
		void setTargeteRate (const unsigned int targetRate) { m_TargetRate = targetRate; }
		void setSourceBufferSize (const unsigned int sourceBufferSize) { m_SourceBufferSize = sourceBufferSize; }
		void setTargetBufferSize (const unsigned int maxTargetBufferSize); // actual target buffer size may be less
		void resetAAFilters();

		void resetAllIncrs()
		{
			m_SourceToTargetSourceIncr = 0.0f;
			m_SourceToTargetTargetIncr = 0.0f;
			m_TargetToSourceTargetIncr = 0.0f;
			m_TargetToSourceSourceIncr = 0.0f;
		}

		float getSourceToTargetSourceIncr() const { return m_SourceToTargetSourceIncr; }
		unsigned int getSourceToTargetTargetIncr() const { return m_SourceToTargetTargetIncr; }
		float getTargetToSourceTargetIncr() const { return m_TargetToSourceTargetIncr; }
		unsigned int getTargetToSourceSourceIncr() const { return m_TargetToSourceSourceIncr; }

		unsigned int getSourceRate() const { return m_SourceRate; }
		unsigned int getTargetRate() const { return m_TargetRate; }
		unsigned int getSourceBufferSize() const { return m_SourceBufferSize; }
		unsigned int getTargetBufferSize() const { return m_TargetBufferSize; }

		bool sourceToTargetIsUpsampling() const { return ( m_SourceRate < m_TargetRate ) ? true : false; }
		bool targetToSourceIsUpsampling() const { return ( m_TargetRate < m_SourceRate ) ? true : false; }

	private:
		unsigned int 	m_SourceRate;
		unsigned int 	m_TargetRate;
		unsigned int 	m_SourceBufferSize;
		unsigned int 	m_TargetBufferSize;
		float 		m_SourceToTargetSourceIncr; // fractional sample number of current source buffer during resampling
		float 		m_SourceToTargetTargetIncr; // fractional sample number of current target buffer sample during resampling
		float 		m_TargetToSourceTargetIncr; // fractional sample number of current target buffer during resampling
		float 	 	m_TargetToSourceSourceIncr; // fractional sample number of current source buffer sample during resampling

		AntiAliasingFilter<SType> m_SourceToTargetDownsamplingAAFilter;
		AntiAliasingFilter<TType> m_SourceToTargetUpsamplingAAFilter;
		AntiAliasingFilter<TType> m_TargetToSourceDownsamplingAAFilter;
		AntiAliasingFilter<SType> m_TargetToSourceUpsamplingAAFilter;

		constexpr TType convertSourceToTargetType (SType sourceType);
		constexpr SType convertTargetToSourceType (TType targetType);

		constexpr TType getTargetZeroPoint();
		constexpr SType getSourceZeroPoint();

		unsigned int getTargetBufferSizePerSourceBuffer() const
		{
			return ( static_cast<float>(m_TargetRate) / static_cast<float>(m_SourceRate) ) * static_cast<float>( m_SourceBufferSize );
		}
};

#endif // SAMPLERATECONVERTER_HPP
