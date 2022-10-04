/**
 * @file Renderer.hpp
 * 
 * @brief The renderer. It is a singleton class.
 **/

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>

/**
 * @brief Singleton renderer class.
 **/
class Renderer
{
public:

  Renderer( const Renderer&  ) = delete;
  Renderer(       Renderer&& ) = delete;
  
  Renderer& operator=( const Renderer& ) = delete;
  
  static Renderer& Get( void );
  
  SDL_Renderer* GetSDLRendererPtr( void );
  void          Render           ( void );

private:

   Renderer( void );
  ~Renderer( void );

  static constexpr int  FIRST_ONE = -1; /* Used by SDL_CreateRenderer(...) in Renderer's constructor */

  SDL_Renderer* m_Renderer; // The actual window renderer
  bool          m_WasInitSuccessful;
};


#endif // RENDERER_HPP