/**
 * @file Renderer.hpp
 * 
 * @brief The renderer.
 **/

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>

class Renderer
{
public:

  ~Renderer( void );
  
  static SDL_Renderer* Get   ( void );
  static void          Render( void );

private:

  Renderer( void );

  static constexpr int  FIRST_ONE = -1; /* Used by SDL_CreateRenderer(...) in Renderer's constructor */

  static Renderer       s_Renderer_Sgl;
  static SDL_Renderer*  s_Renderer; // The actual window renderer
  static bool           s_WasInitSuccessful;
};


#endif // RENDERER_HPP