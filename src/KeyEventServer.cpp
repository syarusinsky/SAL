#include "KeyEventServer.hpp"

#include "IKeyEventListener.hpp"

KeyEventServer::KeyEventServer()
{
}

KeyEventServer::~KeyEventServer()
{
}

void KeyEventServer::registerListener (IKeyEventListener* listener)
{
	if ( listener )
	{
		m_KeyEventListeners.insert( listener );
	}
}

void KeyEventServer::propagateKeyEvent (const KeyEvent& keyEvent)
{
	for ( IKeyEventListener* keyEventListener : m_KeyEventListeners )
	{
		if ( keyEventListener )
		{
			keyEventListener->onKeyEvent( keyEvent );
		}
	}
}
