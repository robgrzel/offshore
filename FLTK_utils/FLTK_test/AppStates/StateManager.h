#pragma once

#include <map>
#include <string>

#define STATEMNGR (StateManager::instance())


class UIEvent {
	public:
		enum UIEventType {
			MOUSE_DOWN,
			MOUSE_MOVE,
			MOUSE_UP,
			MOUSE_WHEEL,
			MOUSE_LEFT,
			MOUSE_RIGHT,
			KEY_DOWN,
			KEY_UP
		};

		UIEvent( UIEventType type );

		~UIEvent( );

		UIEventType type( ) {
			return m_type;
		}

		bool& rbutton( int index );

		int& rkey( );

		int& rx( );

		int& ry( );

	private:
		bool m_buttons[3];
		int  m_key;

		int m_x;
		int m_y;

		UIEventType m_type;
};

class BaseState {
		friend class StateManager;

	public:
		virtual void init( void* data = nullptr ) = 0;

		virtual void clear( ) = 0;

		virtual void update( void* data = nullptr ) = 0;

		virtual void render( ) = 0;

		virtual bool initialized( ) = 0;

		virtual bool event( UIEvent* event ) = 0;

	protected:
		BaseState( );

		virtual ~BaseState( );
};

class StateManager {
	public:
		enum STATE_TYPES {
			state2d,
			state3d,
		};

		static StateManager& instance();

		void setActiveState( const std::string& stateName, void* initData = NULL );

		void switch_active_state_2d();

		void switch_active_state_3d();

		BaseState* activeState( );

		void updateActiveState( void* data = nullptr );

		void renderActiveState( );

		void sendEvent( UIEvent* event );

	protected:
		StateManager( );

		~StateManager( );

		void addStates( );

	private:
		STATE_TYPES stateType;
		std::string m_activeStateName;
		BaseState* m_activeState;
		std::map<std::string, BaseState*> m_states;
};

