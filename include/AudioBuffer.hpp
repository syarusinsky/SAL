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

#include <set>

#include "IBufferCallback.hpp"

namespace siike92
{
	class AudioBuffer;
}

class siike92::AudioBuffer
{
	public:
		AudioBuffer();
		AudioBuffer(const AudioBuffer& other);
		virtual ~AudioBuffer();

		AudioBuffer& operator= (const AudioBuffer& other);

		unsigned int getNumSamples() const;
		float getNextSample();
		const float* const getBuffer1() const;
		const float* const getBuffer2() const;
		const std::set<IBufferCallback*>& getCallbacks() const;
		void registerCallback(IBufferCallback* callback);
	private:
		float* m_Buffer1;
		float* m_Buffer2;
		unsigned int m_Pos;
		std::set<IBufferCallback*> m_Callbacks;

		bool m_CurrentReadBlock;

		float* getBuffer(bool writeBuffer);
};

#endif
