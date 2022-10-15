/**
 * @file InputManager.hpp
 *
 * @brief The Input Manager.
 **/

#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <SDL.h>

/**
 * @brief Singleton input manager class.
 **/
class InputManager
{
public:

  InputManager( const InputManager&  ) = delete;
  InputManager(       InputManager&& ) = delete;

  InputManager& operator=( const InputManager&  ) = delete;
  InputManager& operator=(       InputManager&& ) = delete;

  static InputManager& Get( void );

  void ManageInput      ( void );
  bool WasInitSuccessful( void );
  bool WasQuitRequested ( void );

private:

   InputManager( void );
  ~InputManager( void );

  bool      m_WasInitSuccessful;
  bool      m_WasQuitRequested;
  SDL_Event m_Event;
};

#endif // INPUTMANAGER_HPP