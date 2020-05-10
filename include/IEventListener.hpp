#ifndef IEVENTLISTENER_HPP
#define IEVENTLISTENER_HPP

/*****************************************************************
 * The IEventListener and IEvent classes are the heart of the
 * event system. Any class that inherits from an IEventListener
 * will be capable of binding itself to that event's event
 * dispatcher, meaning that all events of that type that are
 * published will be passed to that subclass' on*Event function.
 * The events themselves will be published by calling the
 * IEventListener subclass' PublishEvent function.
 *
 * To create your own event type and event listener type, the
 * event listener must subclass from IEventListener and the
 * event type must subclass from IEvent. The IEventListener
 * subclass must also hold a static EventDispatcher, provide
 * bindTo*EventSystem and unbindFrom*EventSystem functions to
 * bind to/unbind from that EventDispatcher, provide a static
 * PublishEvent function to dispatch events through the
 * EventDispatcher, and provide its own on*Event pure virtual
 * function. In addition, the IEventListener subclass should also
 * call unbindFromEventSystem in its destructor to assure events
 * are not passed to an invalid pointer. Check the IKeyEventListener
 * class for a typical example.
 *
 * An IEventListener class should include the IEvent subclass and
 * the IEventListener subclass in the hpp file, and the
 * instantiation of the EventDispatcher in the cpp file.
*****************************************************************/

#include <functional>
#include <set>

class IEvent
{
	public:
		IEvent (unsigned int channel) : m_Channel (channel) {}
		virtual ~IEvent() {}

		unsigned int getChannel() const { return m_Channel; }
		void setChannel (unsigned int channel) { m_Channel = channel; }

	protected:
		unsigned int m_Channel;
};

class IEventListener
{
	public:
		virtual ~IEventListener() {}
};

template <typename ListenerType, typename EventType, void (ListenerType::*ListenerFunction)(const EventType&)>
class EventDispatcher
{
	public:
		EventDispatcher()
		{
			// ensure correct types
			static_assert(std::is_base_of<IEventListener, ListenerType>::value,
					"EventDispatcher Template Argument 1: Need to use IEventListener");
			static_assert(std::is_base_of<IEvent, EventType>::value,
					"EventDispatcher Template Argument 2: Need to use IEvent");

			m_Func = ListenerFunction;
		}

		~EventDispatcher() {}

		void bind (ListenerType* listener)
		{
			m_Listeners.insert( listener );
		}

		void unbind (ListenerType* listener)
		{
			m_Listeners.erase( listener );
		}

		void dispatch (const EventType& event)
		{
			for ( ListenerType* eventListener : m_Listeners )
			{
				m_Func( eventListener, event );
			}
		}

	private:
		std::function<void(ListenerType*, const EventType&)> 	m_Func;
		std::set<ListenerType*> 				m_Listeners;
};

#endif // IEVENTLISTENER_HPP
