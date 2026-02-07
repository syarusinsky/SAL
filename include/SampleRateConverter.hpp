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
 * For typical implementation, look at getNextAudioBlock in MainComponent.cpp in
 * the AkiDelay project.
*******************************************************************************/

template <typename SType, typename TType>
class SampleRateConverter
{
	public:
		SampleRateConverter (const unsigned int initialSourceRate, const unsigned int initialTargetRate,
					const unsigned int initialSourceBufferSize, const unsigned int initialTargetBufferSize);
		~SampleRateConverter() {}

		bool convertFromSourceToTarget (const SType* const sourceBuffer, TType* const targetBuffer); // true if source buffer fully converted
		bool convertFromTargetToSource (const TType* const targetBuffer, SType* const sourceBuffer); // true if source buffer fully filled

		void setSourceRate (const unsigned int sourceRate) { m_SourceRate = sourceRate; }
		void setTargeteRate (const unsigned int targetRate) { m_TargetRate = targetRate; }
		void setSourceBufferSize (const unsigned int sourceBufferSize) { m_SourceBufferSize = sourceBufferSize; }
		void setTargetBufferSize (const unsigned int targetBufferSize) { m_TargetBufferSize = targetBufferSize; }

		void resetAllIncrs()
		{
			m_SourceToTargetSourceIncr = 0.0f;
			m_SourceToTargetTargetIncr = 0.0f;
			m_TargetToSourceTargetIncr = 0.0f;
			m_TargetToSourceSourceIncr = 0.0f;
		}

		float getSourceToTargetSourceIncr() { return m_SourceToTargetSourceIncr; }
		unsigned int getSourceToTargetTargetIncr() { return m_SourceToTargetTargetIncr; }
		float getTargetToSourceTargetIncr() { return m_TargetToSourceTargetIncr; }
		unsigned int getTargetToSourceSourceIncr() { return m_TargetToSourceSourceIncr; }

		unsigned int getSourceRate() { return m_SourceRate; }
		unsigned int getTargetRate() { return m_TargetRate; }
		unsigned int getSourceBufferSize() { return m_SourceBufferSize; }
		unsigned int getTargetBufferSize() { return m_TargetBufferSize; }

	private:
		// TODO also need anti-aliasing filters for input and output
		unsigned int 	m_SourceRate;
		unsigned int 	m_TargetRate;
		unsigned int 	m_SourceBufferSize;
		unsigned int 	m_TargetBufferSize;
		float 		m_SourceToTargetSourceIncr; // fractional sample number of current source buffer during resampling
		float 		m_SourceToTargetTargetIncr; // fractional sample number of current target buffer sample during resampling
		float 		m_TargetToSourceTargetIncr; // fractional sample number of current target buffer during resampling
		float 	 	m_TargetToSourceSourceIncr; // fractional sample number of current source buffer sample during resampling

		constexpr TType convertSourceToTargetType (SType sourceType);
		constexpr SType convertTargetToSourceType (TType targetType);
};

#endif // SAMPLERATECONVERTER_HPP
