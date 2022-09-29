/***************************************************************************************************
* Includes
****************************************************************************************************/

#include "InputManager.hpp"

#include <iostream>
#include <cstdio>


/***************************************************************************************************
* Static members
****************************************************************************************************/

bool      InputManager::s_WasInitSuccessful (true);
bool      InputManager::s_WasQuitRequested  (false);
SDL_Event InputManager::s_Event;


/***************************************************************************************************
* Private constants
****************************************************************************************************/


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


void InputManager::ManageInput( void )
{
  while( SDL_PollEvent( &s_Event ) != 0 )
  {
    if( s_Event.type == SDL_QUIT )
    {
      s_WasQuitRequested = true;
    }
    else
    {
      // s_WasQuitRequested = false;
    }
  }
}


bool InputManager::QuitRequested( void )
{
  return s_WasQuitRequested;
}