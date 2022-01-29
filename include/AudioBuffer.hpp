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

template <typename T=float, bool isStereo=false>
class AudioBuffer;

template <typename T>
class AudioBuffer<T, false>
{
	public:
		AudioBuffer();
		AudioBuffer (const AudioBuffer& other);
		virtual ~AudioBuffer();

		AudioBuffer<T, false>& operator= (const AudioBuffer<T, false>& other);

		unsigned int getNumSamples() const;
		T getNextSample (T sampleValToReadBuf = 0); 	// if we're also reading data (from ADC for example)
								// we write those samples back into the 'read' buffer
		void pollToFillBuffers();

		const T* const getBuffer1() const;
		const T* const getBuffer2() const;

		const std::set<IBufferCallback<T, false>*>& getCallbacks() const;
		void registerCallback (IBufferCallback<T, false>* callback);
	private:
		T 		m_Buffer1[ABUFFER_SIZE];
		T 		m_Buffer2[ABUFFER_SIZE];
		T* 		m_CurrentBuffer;
		unsigned int 	m_Pos;
		std::set<IBufferCallback<T, false>*> m_Callbacks;

		bool m_NextReadBlockFilled;

		T* getBuffer(bool writeBuffer);
};

template <typename T>
class AudioBuffer<T, true>
{
	public:
		AudioBuffer();
		AudioBuffer (const AudioBuffer& other);
		virtual ~AudioBuffer();

		AudioBuffer<T, true>& operator= (const AudioBuffer<T, true>& other);

		unsigned int getNumSamples() const;
		T getNextSampleL (T sampleValToReadBuf = 0);
		T getNextSampleR (T sampleValToReadBuf = 0);

		void pollToFillBuffers();

		const T* const getBufferL1() const;
		const T* const getBufferL2() const;
		const T* const getBufferR1() const;
		const T* const getBufferR2() const;

		const std::set<IBufferCallback<T, true>*>& getCallbacks() const;
		void registerCallback (IBufferCallback<T, true>* callback);
	private:
		T 		m_BufferL1[ABUFFER_SIZE];
		T 		m_BufferL2[ABUFFER_SIZE];
		T 		m_BufferR1[ABUFFER_SIZE];
		T 		m_BufferR2[ABUFFER_SIZE];
		T* 		m_CurrentBufferL;
		T* 		m_CurrentBufferR;
		unsigned int 	m_PosL;
		unsigned int 	m_PosR;
		std::set<IBufferCallback<T, true>*> m_Callbacks;

		bool m_NextReadBlockFilledL;
		bool m_NextReadBlockFilledR;

		T* getBufferL(bool writeBuffer);
		T* getBufferR(bool writeBuffer);
};

#endif
