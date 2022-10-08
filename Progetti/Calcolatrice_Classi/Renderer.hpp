/**
 * @file Renderer.hpp
 *
 * @brief The renderer. It is a singleton class.
 **/

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>
#include <string>
#include "Texture.hpp"

/**
 * @brief Singleton renderer class.
 **/
class Renderer
{
public:

  Renderer( const Renderer&  ) = delete;
  Renderer(       Renderer&& ) = delete;

  Renderer& operator=( const Renderer&  ) = delete;
  Renderer& operator=(       Renderer&& ) = delete;

  static Renderer& Get( void );

  SDL_Renderer* GetSDLRendererPtr( void );
  void          Render           ( void );

private:

   Renderer( void );
  ~Renderer( void );

  void LoadMedia_Pvt      ( void );
  void CreateRenderer_Pvt ( void );

  static constexpr int  FIRST_ONE = -1; /* Used by SDL_CreateRenderer(...) in Renderer's constructor */

  SDL_Renderer*     m_Renderer = nullptr; // The actual window renderer
  bool              m_WasInitSuccessful = true;
  bool              m_MediaLoaded = false;
  Texture           m_Keys;
  Texture           m_KeysPressed;
  const std::string m_KeysPath        = "./Sprites/Keys.png";
  const std::string m_KeysPressedPath = "./Sprites/Keys_Pressed.png";
};


#endif // RENDERER_HPP