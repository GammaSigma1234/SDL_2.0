#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>

class Renderer
{
public:

  static Renderer* Get(void);

  ~Renderer(void);

private:

  Renderer(/* args */);

  static constexpr int FIRST_ONE = -1;

  static Renderer Renderer_Singleton;

  SDL_Renderer* gRenderer = NULL; // The window renderer
};


#endif // RENDERER_HPP