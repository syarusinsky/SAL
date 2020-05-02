#ifndef IBUFFERCALLBACK_HPP
#define IBUFFERCALLBACK_HPP

/****************************************************************
 * An IBufferCallback is used to write to the write buffer of
 * an AudioBuffer. See AudioBuffer for more details.
****************************************************************/

namespace siike92
{
	class IBufferCallback;
}

class siike92::IBufferCallback
{
	public:
		virtual ~IBufferCallback(){}
		virtual void call(float* writeBuffer) = 0;
};

#endif
