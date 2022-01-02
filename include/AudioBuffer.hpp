#ifndef AUDIOBUFFER_HPP
#define AUDIOBUFFER_HPP

/******************************************************************
 * A generic audio buffer which contains two arrays that allow
 * for IBufferCallbacks to write to a "write buffer" while outputs
 * read from a "read buffer". This ensures that reading and
 * writing are performed on separate memory blocks. This class
 * manages the switching between buffers as well as allows for
 * the registering of IBufferCallbacks, which write to the write
 * buffer.
******************************************************************/

#include "IBufferCallback.hpp"

#include "AudioConstants.hpp"
#include <set>
#include <stdint.h>

template <typename T>
class AudioBuffer
{
	public:
		AudioBuffer();
		AudioBuffer (const AudioBuffer& other);
		virtual ~AudioBuffer();

		AudioBuffer<T>& operator= (const AudioBuffer<T>& other);

		unsigned int getNumSamples() const;
		T getNextSample (T sampleValToReadBuf = 0); 	// if we're also reading data (from ADC for example)
								// we write those samples back into the 'read' buffer
		void pollToFillBuffers();
		const T* const getBuffer1() const;
		const T* const getBuffer2() const;

		const std::set<IBufferCallback<T>*>& getCallbacks() const;
		void registerCallback (IBufferCallback<T>* callback);
	private:
		T 		m_Buffer1[ABUFFER_SIZE];
		T 		m_Buffer2[ABUFFER_SIZE];
		T* 		m_CurrentBuffer;
		unsigned int 	m_Pos;
		std::set<IBufferCallback<T>*> m_Callbacks;

		bool m_NextReadBlockFilled;

		T* getBuffer(bool writeBuffer);
};

#endif
