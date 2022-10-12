/**
 * @file Renderer.hpp
 *
 * @brief The renderer. It is a singleton class.
 **/

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>
#include <string>
#include <vector>
#include "Texture.hpp"
#include "Button.hpp"

/**
 * @brief Singleton renderer class.
 **/
class Renderer
{
public:

  enum class ButtonsClips_Enum
  {
    KEY_0 = 0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_PLUS,
    KEY_MINUS,
    KEY_DIVIDE,
    KEY_MULTIPLY,
    KEY_POINT,
    KEY_EQUALS,

    HOW_MANY
  };

  /**
   * @brief All the sprite sheets used by the renderer.
   **/
  enum class SpriteSheets_Enum
  {
    NORMAL_BUTTONS = 0,
    PRESSED_BUTTONS,

    HOW_MANY
  };

  Renderer( const Renderer&  ) = delete; // Singleton
  Renderer(       Renderer&& ) = delete; // Singleton

  Renderer& operator=( const Renderer&  ) = delete; // Singleton
  Renderer& operator=(       Renderer&& ) = delete; // Singleton

  static Renderer& Get( void );

  SDL_Renderer*        GetSDLRendererPtr( void );
  Texture&             GetSpriteSheet   ( SpriteSheets_Enum );
  std::vector<Button>& GetButtonVector  ( void );
  void                 Render           ( void );

private:

   Renderer( void );
  ~Renderer( void );

  void LoadMedia_Pvt      ( void );
  void CreateRenderer_Pvt ( void );
  void CreateShapes_Pvt   ( void );

  const int     FIRST_ONE = -1; /* Used by SDL_CreateRenderer(...) in Renderer's constructor */

  SDL_Renderer* m_Renderer          = nullptr; // The actual window renderer
  bool          m_WasInitSuccessful = true;
  bool          m_MediaLoaded       = false;

  std::vector<std::string> m_SpriteSheets_Paths;
  std::vector<Texture>     m_SpriteSheets_Vec;
  std::vector<Button>      m_Buttons_Vec;
};


#endif // RENDERER_HPP