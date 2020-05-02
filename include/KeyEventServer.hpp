#ifndef KEYEVENTSERVER_HPP
#define KEYEVENTSERVER_HPP

/********************************************************************
 * The KeyEventServer is a class that propagates KeyEvents to
 * multiple IKeyEventListeners. This is useful in synthesizer voices
 * for example, as it allows for easier management of the many
 * IKeyEventListeners that a synthesizer voice may have.
 *
 * IKeyEventListeners can choose which properties to respond to
 * and which to ignore.
*********************************************************************/

#include <set>

#include "IKeyEventListener.hpp"

class KeyEventServer
{
	public:
		KeyEventServer();
		~KeyEventServer();

		void registerListener(IKeyEventListener* listener);
		void propagateKeyEvent(const KeyEvent& keyEvent);
	private:
		std::set<IKeyEventListener*> m_KeyEventListeners;
};

#endif // KEYEVENTSERVER_HPP
