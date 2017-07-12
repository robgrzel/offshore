#include <iostream>
#include "StateManager.h"
#include "GLSimpleState/GLSimpleState2d.h"
#include "GLSimpleState/GLSimpleState3d.h"


UIEvent::UIEvent( UIEventType type )
		: m_type( type ) {
	std::cout << "UIEvent\n";

}

UIEvent::~UIEvent( ) {
	std::cout << "UIEvent::~UIEvent\n";

}

bool& UIEvent::rbutton( int index ) {
	std::cout << "UIEvent::rbutton\n";

	index = index % 3;
	return m_buttons[index];

}

int& UIEvent::rkey( ) {
	std::cout << "UIEvent::rkey\n";

	return m_key;
}

int& UIEvent::rx( ) {
	std::cout << "UIEvent::rx\n";

	return m_x;
}

int& UIEvent::ry( ) {
	std::cout << "UIEvent::ry\n";

	return m_y;
}

BaseState::BaseState( ) {
	std::cout << "BaseState::BaseState\n";

}

BaseState::~BaseState( ) {
	std::cout << "BaseState::~BaseState\n";

}

StateManager::StateManager( ) :

		m_activeState( nullptr ) {
	std::cout << "StateManager::StateManager( STATE_TYPES stateType )\n";
	addStates();

}


StateManager::~StateManager( ) {
	std::cout << "StateManager::~StateManager\n";

}

StateManager& StateManager::instance( ) {
	std::cout << "StateManager& StateManager::instance( )\n";

	static StateManager stMngr;
	return stMngr;
}

void StateManager::switch_active_state_2d( ) {
	std::cout << "StateManager& StateManager::switch_active_state_2d( )\n";

	m_activeStateName = GL_SIMPLE_STATE_2D;
	m_activeState = m_states[m_activeStateName];
}

void StateManager::switch_active_state_3d( ) {
	std::cout << "StateManager& StateManager::switch_active_state_3d()\n";

	m_activeStateName = GL_SIMPLE_STATE_3D;
	m_activeState = m_states[m_activeStateName];
}



void StateManager::setActiveState( const std::string& stateName, void* initData ) {
	std::cout << "StateManager::setActiveState( const std::string& stateName, void* initData )\n";
	BaseState* oldState = m_activeState;
	m_activeStateName = stateName;
	m_activeState     = m_states[m_activeStateName];
	if (oldState != nullptr) {
		oldState->clear();
	}
	if (m_activeState != nullptr) {
		m_activeState->init( initData );
	}
}

BaseState* StateManager::activeState( ) {
	std::cout << "StateManager::activeState\n";
	return m_activeState;
}

void StateManager::updateActiveState( void* data ) {
	std::cout << "StateManager::updateActiveState( void* data )\n";
	if (m_activeState == nullptr) {
		return;
	}

	if (!m_activeState->initialized()) {
		m_activeState->init();
	}
	m_activeState->update( data );
}

void StateManager::renderActiveState( ) {
	std::cout << "StateManager::renderActiveState\n";
	if (m_activeState == nullptr) {
		return;
	}

	m_activeState->render();
}

void StateManager::addStates( ) {
	std::cout << "StateManager::addStates2d\n";
	m_states[GL_SIMPLE_STATE_2D] = new GLSimpleState2d();
	m_states[GL_SIMPLE_STATE_3D] = new GLSimpleState3d();

}

void StateManager::sendEvent( UIEvent* event ) {
	std::cout << "StateManager::sendEvent( UIEvent* event )\n";
	if (m_activeState == nullptr) {
		return;
	}

	m_activeState->event( event );
}
