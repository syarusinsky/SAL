#ifndef IMODULATIONSOURCE_HPP
#define IMODULATIONSOURCE_HPP

/****************************************************************
 * An IModulationSource is intended to be a source of values
 * to be used in modulating a destination. The destination
 * is responsible for managing and responding to values it
 * receives from an IModulationSource.
****************************************************************/

class IModulationSource
{
	public:
		virtual ~IModulationSource(){}
		virtual float currentValue() = 0;
};

#endif
