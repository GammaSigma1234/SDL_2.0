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

  static SDL_Renderer*  Get   (void);
  static void           Render(void);

  ~Renderer(void);

private:

  Renderer(/* args */);

  static constexpr int  FIRST_ONE = -1; /* Used by SDL_CreateRenderer(...) in the constructor */

  static Renderer       s_Renderer_Sgl;
  static SDL_Renderer*  s_Renderer; // The window renderer
  static bool           s_WasInitSuccessful;
};


#endif // RENDERER_HPP