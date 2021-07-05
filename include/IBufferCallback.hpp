#ifndef IBUFFERCALLBACK_HPP
#define IBUFFERCALLBACK_HPP

/****************************************************************
 * An IBufferCallback is used to write to the write buffer of
 * an AudioBuffer. See AudioBuffer for more details.
****************************************************************/

template <typename T=float>
class IBufferCallback
{
	public:
		virtual ~IBufferCallback() {}
		virtual void call (T* writeBuffer) = 0;
};

#endif
