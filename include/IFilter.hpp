#ifndef IFILTER_HPP
#define IFILTER_HPP

/*****************************************************************
 * An IFilter describes a generic filter which is capable of
 * processing samples, setting its coefficients, and setting its
 * resonance.
*****************************************************************/

class IFilter
{
	public:
		virtual ~IFilter() {}

		virtual float processSample (float sample) = 0;
		virtual void setCoefficients (float frequency) = 0;
		virtual void setResonance (float resonance) = 0;
		virtual float getResonance() = 0;
};

#endif // IFILTER_HPP
