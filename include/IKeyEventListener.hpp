#ifndef IKEYEVENTLISTENER_HPP
#define IKEYEVENTLISTENER_HPP

/*******************************************************************
 * An IKeyEventListener specifies a simple interface which
 * a subclass can use to be notified of key events.
*******************************************************************/

#include "IEventListener.hpp"

enum class KeyPressedEnum : unsigned int
{
	PRESSED,
	HELD,
	RELEASED
};

class KeyEvent : public IEvent
{
	public:
		KeyEvent (KeyPressedEnum pressed = KeyPressedEnum::RELEASED, unsigned int note = 0, unsigned int velocity = 0,
				unsigned int channel = 0);
		~KeyEvent() override;

		bool operator== (const KeyEvent& other) const;

		KeyPressedEnum pressed() const;
		unsigned int note() const;
		unsigned int velocity() const;

		bool isNoteAndType (const KeyEvent& other) const;
		bool isNoteAndType (const KeyEvent& other, const KeyPressedEnum& pressed) const;

	private:
		KeyPressedEnum m_Pressed;
		unsigned int m_Note;
		unsigned int m_Velocity;

};

class IKeyEventListener : public IEventListener
{
	public:
		virtual ~IKeyEventListener();

		virtual void onKeyEvent (const KeyEvent& keyEvent) = 0;

		void bindToKeyEventSystem();
		void unbindFromKeyEventSystem();

		static void PublishEvent (const KeyEvent& keyEvent);

	private:
		static EventDispatcher<IKeyEventListener, KeyEvent, &IKeyEventListener::onKeyEvent> m_EventDispatcher;
};

#endif // IKEYEVENTLISTENER_HPP
