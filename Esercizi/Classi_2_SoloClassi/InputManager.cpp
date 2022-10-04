/***************************************************************************************************
* Includes
****************************************************************************************************/

#include "InputManager.hpp"
#include "Supervisor.hpp" // TODO: test fault

#include <iostream>
#include <cstdio>

/***************************************************************************************************
* Private constants
****************************************************************************************************/

static auto& Manager_Ref( InputManager::Get() );


/***************************************************************************************************
* Methods
****************************************************************************************************/

InputManager::InputManager( void )
{
  std::cout << "\nOK: Input Manager initialised.\n";
}


InputManager::~InputManager( void )
{
  std::cout << "InputManager's destructor called\n";
}


/**
 * @brief Creates instance of singleton in static memory
 *
 * @return InputManager&
 **/
InputManager& InputManager::Get(void)
{
  static InputManager Instance;
  return Instance;
}


void InputManager::ManageInput( void )
{
  while( SDL_PollEvent( &Manager_Ref.m_Event ) != 0 )
  {
    if( m_Event.type == SDL_QUIT )
    {
      Manager_Ref.m_WasQuitRequested = true;
    }
    else if ( m_Event.type == SDL_KEYDOWN ) // User presses a key
    {
      switch( m_Event.key.keysym.sym )
      {
        case SDLK_UP: // TODO: test fault
          Supervisor::Get().RaiseFault();
          break;
        
        default:
          break;
      }
    }
    else
    { /* User did not request to quit the program. */ }
  }
}


bool InputManager::WasQuitRequested( void )
{
  return Manager_Ref.m_WasQuitRequested;
}