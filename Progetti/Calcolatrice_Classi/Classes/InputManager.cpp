/***************************************************************************************************
* Includes
****************************************************************************************************/

#include "InputManager.hpp"
#include "Supervisor.hpp" // TODO: test fault
#include "Renderer.hpp"

#include <iostream>
#include <cstdio>

/***************************************************************************************************
* Private constants
****************************************************************************************************/


/***************************************************************************************************
* Methods
****************************************************************************************************/

InputManager::InputManager( void )
{
  std::cout << "\nInitialising Input Manager...\n";
  std::cout << "\tOK: Input Manager initialised.\n";
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
  while( SDL_PollEvent( &m_Event ) != 0 )
  {
    if ( m_Event.type == SDL_QUIT )
    {
      m_WasQuitRequested = true;
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

    /* Handle button events */

    size_t NumOfButtons(static_cast<size_t>(Renderer::ButtonsClips_Enum::HOW_MANY));

    for ( size_t i = 0; i != NumOfButtons; ++i )
    {
      Renderer::Get().GetButtonVector()[ i ].handleMouseEvent( &m_Event );
    }
  }
}


bool InputManager::WasQuitRequested( void )
{
  return m_WasQuitRequested;
}