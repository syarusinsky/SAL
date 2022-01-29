#ifndef IBUFFERCALLBACK_HPP
#define IBUFFERCALLBACK_HPP

/****************************************************************
 * An IBufferCallback is used to write to the write buffer of
 * an AudioBuffer. See AudioBuffer for more details.
****************************************************************/

template <typename T=float, bool isStereo=false>
class IBufferCallback;

template <typename T>
class IBufferCallback<T, false>
{
	public:
		virtual ~IBufferCallback() {}
		virtual void call (T* writeBuffer) = 0;
};

template <typename T>
class IBufferCallback<T, true>
{
	public:
		virtual ~IBufferCallback() {}
		virtual void call (T* writeBufferL, T* writeBufferR) = 0;
};

#endif
