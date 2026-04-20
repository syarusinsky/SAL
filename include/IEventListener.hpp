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
 *
 * The VST_BUILD stuff is for use with VST plugins. Since VST
 * plugins execute their UIs on a single thread, there's an issue
 * with the fact that EventDispatcher is a static member variable
 * and all instances of the plugin will get the event. To fix this
 * the JUCE PluginProcessor and PluginProcessorEditor objects must
 * hold unique ids which will be used to send events only to the
 * event listeners they instantiated. For an example, look at
 * Aki-delay and look for the dispatchEventsForIds function, which
 * is called at the end of any function that might publish events.
*****************************************************************/

#include <functional>
#include <set>

#ifdef VST_BUILD
#include <deque>
#include <mutex>
#include <unordered_map>
#endif

class IEvent
{
	public:
		IEvent (unsigned int channel) : m_Channel( channel ) {}
		virtual ~IEvent() {}

		unsigned int getChannel() const { return m_Channel; }
		virtual void setChannel (unsigned int channel) { m_Channel = channel; }

	protected:
		unsigned int m_Channel;
};

class IEventListener
{
	public:
#ifdef VST_BUILD
		IEventListener() : m_ThisJuceProcessorId( m_GlobalJuceProcessorId ) {}
#endif
		virtual ~IEventListener() {}
#ifdef VST_BUILD
		// this should be called at the end of the plugin processor's constructor and at the end of the
		// plugin processor editor's construcot
		static unsigned int getGlobalJuceProcessorId()
		{
			unsigned int globalJuceProcessorId = m_GlobalJuceProcessorId;
			m_GlobalJuceProcessorId++; // increment for the next plugin processor

			return globalJuceProcessorId;
		}

		unsigned int getThisJuceProcessorId()
		{
			return m_ThisJuceProcessorId;
		}

	private:
		unsigned int m_ThisJuceProcessorId;
		// since juce instantiates each plugin processor sequentially, we use this global id
		// to assign an id to each instance of event listener per plugin processor and per plugin
		// editor
		static inline unsigned int m_GlobalJuceProcessorId = 0;
#endif
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
#ifdef VST_BUILD
			std::lock_guard<std::mutex> lock( m_JuceEventQueueMutex );
			m_JuceListenersMap.emplace( listener->getThisJuceProcessorId(), listener );
#else
			m_Listeners.insert( listener );
#endif
		}

		void unbind (ListenerType* listener)
		{
#ifdef VST_BUILD
			std::lock_guard<std::mutex> lock( m_JuceEventQueueMutex );
			auto listenersRange = m_JuceListenersMap.equal_range( listener->getThisJuceProcessorId() );

			for ( auto listenersIt = listenersRange.first; listenersIt != listenersRange.second; listenersIt++ )
			{
				auto* eventListener = listenersIt->second;
				if ( eventListener == listener )
				{
					m_JuceListenersMap.erase( listenersIt );
				}
			}
#else
			m_Listeners.erase( listener );
#endif
		}

		void dispatch (const EventType& event)
		{
#ifdef VST_BUILD
			// just queueing up the event for the juce dispatch function,
			// which should be called at the end of any ui input processing and at
			// the end of the processBlock
			std::lock_guard<std::mutex> lock( m_JuceEventQueueMutex );
			m_JuceEventQueue.push_back( event );
#else
			for ( ListenerType* eventListener : m_Listeners )
			{
				m_Func( eventListener, event );
			}
#endif
		}

#ifdef VST_BUILD
		// this function may need to be called more than one time if the handling of this function for a different event listener dispatches
		// new events to this event listener's event queue
		static void juceDispatchQueuedEvents (const unsigned int processorId, const unsigned int processorEditorId)
		{
			std::lock_guard<std::mutex> lock( m_JuceEventQueueMutex );
			auto eventIt = m_JuceEventQueue.begin();

			while ( eventIt != m_JuceEventQueue.end() )
			{
				bool handled = false;
				auto listenersRange = m_JuceListenersMap.equal_range( processorId );

				for ( auto listenersIt = listenersRange.first; listenersIt != listenersRange.second; listenersIt++ )
				{
					auto* eventListener = listenersIt->second;
					const unsigned int eventListenerId = eventListener->getThisJuceProcessorId();
					if ( eventListenerId == processorId )
					{
						(eventListener->*ListenerFunction)( *eventIt );
						handled = true;
					}
				}

				listenersRange = m_JuceListenersMap.equal_range( processorEditorId );

				for ( auto listenersIt = listenersRange.first; listenersIt != listenersRange.second; listenersIt++ )
				{
					auto* eventListener = listenersIt->second;
					const unsigned int eventListenerId = eventListener->getThisJuceProcessorId();
					if ( eventListenerId == processorEditorId )
					{
						(eventListener->*ListenerFunction)( *eventIt );
						handled = true;
					}
				}

				if ( handled )
				{
					eventIt = m_JuceEventQueue.erase( eventIt );
				}
				else
				{
					eventIt++;
				}
			}
		}
#endif

	private:
		std::function<void(ListenerType*, const EventType&)> 			m_Func;
#ifdef VST_BUILD
		static inline std::deque<EventType> 					m_JuceEventQueue;
		static inline std::mutex 						m_JuceEventQueueMutex;
		static inline std::unordered_multimap<unsigned int, ListenerType*> 	m_JuceListenersMap;
#else
		std::set<ListenerType*> 						m_Listeners;
#endif
};

#endif // IEVENTLISTENER_HPP
