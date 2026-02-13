#ifndef SAMPLERATECONVERTER_HPP
#define SAMPLERATECONVERTER_HPP

/*******************************************************************************
 * A simple sample rate converter for converting plugin sample rates (source)
 * to the sample rates of SAL gear (target). Obviously this doesn't just have
 * to be used with SAL stuff. This component is treated as handling both input
 * (source-to-target) and output (target-to-source).
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
					const unsigned int initialSourceBufferSize);
		~SampleRateConverter() {}

		bool convertFromSourceToTarget (const SType* const sourceBuffer, TType* const targetBuffer); // true if source buffer fully converted
		bool convertFromTargetToSource (const TType* const targetBuffer, SType* const sourceBuffer); // true if source buffer fully filled

		void filterSourceToTargetDownsampling (SType* const buffer);
		void filterSourceToTargetUpsampling (TType* const buffer, const unsigned int bufferSize); // since target buffer can be fractional
		void filterTargetToSourceDownsampling (TType* const buffer, const unsigned int bufferSize); // since target buffer can be fractional
		void filterTargetToSourceUpsampling (SType* const buffer);

		void setSourceRate (const unsigned int sourceRate);
		void setTargetRate (const unsigned int targetRate);
		void setSourceBufferSize (const unsigned int sourceBufferSize);
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
		float getTargetBufferSize() const { return m_TargetBufferSize; }

		bool sourceToTargetIsUpsampling() const { return ( m_SourceRate < m_TargetRate ) ? true : false; }
		bool targetToSourceIsUpsampling() const { return ( m_TargetRate < m_SourceRate ) ? true : false; }

	private:
		unsigned int 	m_SourceRate;
		unsigned int 	m_TargetRate;
		unsigned int 	m_SourceBufferSize;
		float 		m_TargetBufferSize; // this value is a float, since conversion from source rate to target rate can be fractional
		float 		m_SourceToTargetSourceIncr; // fractional sample number of current source buffer during resampling
		float 		m_SourceToTargetTargetIncr; // fractional sample number of current target buffer sample during resampling
		float 		m_TargetToSourceTargetIncr; // fractional sample number of current target buffer during resampling
		float 	 	m_TargetToSourceSourceIncr; // fractional sample number of current source buffer sample during resampling
		float 		m_SourceToTargetPhaseAccumulation; // for calculating fixed size buffers with fractional target buffer sizes

		TType 		m_WorkingTargetBufferLastSample;
		SType 		m_WorkingSourceBufferLastSample;
		float 		m_WorkingTargetBufferSize; 	// this is to keep track of fractional buffer sizes between source-to-target and
								// target-to-source conversions

		AntiAliasingFilter<SType> m_SourceToTargetDownsamplingAAFilter;
		AntiAliasingFilter<TType> m_SourceToTargetUpsamplingAAFilter;
		AntiAliasingFilter<TType> m_TargetToSourceDownsamplingAAFilter;
		AntiAliasingFilter<SType> m_TargetToSourceUpsamplingAAFilter;

		constexpr TType convertSourceToTargetType (SType sourceType);
		constexpr SType convertTargetToSourceType (TType targetType);

		constexpr TType getTargetZeroPoint();
		constexpr SType getSourceZeroPoint();

		float getTargetBufferSizePerSourceBuffer() const;
};

#endif // SAMPLERATECONVERTER_HPP
