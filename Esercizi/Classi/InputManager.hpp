/**
 * @file InputManager.hpp
 * 
 * @brief The Input Manager.
 **/

#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <SDL.h>

class InputManager
{
public:

  ~InputManager( void );

  static void ManageInput  ( void );
  static bool QuitRequested( void );

private:

  InputManager( void );

  static InputManager s_InputManager_Sgl;
  static bool         s_WasInitSuccessful;
  static bool         s_WasQuitRequested;
  static SDL_Event    s_Event;
};

#endif // INPUTMANAGER_HPP