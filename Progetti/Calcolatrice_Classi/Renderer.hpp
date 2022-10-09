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

  Renderer( const Renderer&  ) = delete;
  Renderer(       Renderer&& ) = delete;

  Renderer& operator=( const Renderer&  ) = delete;
  Renderer& operator=(       Renderer&& ) = delete;

  static Renderer& Get( void );

  SDL_Renderer*        GetSDLRendererPtr           ( void );
  Texture&             GetNormalButtonsSpriteSheet ( void );
  Texture&             GetPressedButtonsSpriteSheet( void );
  std::vector<Button>& GetButtonVector             ( void );
  void                 Render                      ( void );

  enum ButtonsClips_Enum
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

private:

   Renderer( void );
  ~Renderer( void );

  void LoadMedia_Pvt      ( void );
  void CreateRenderer_Pvt ( void );
  void CreateButtons_Pvt  ( void );

  static constexpr int  FIRST_ONE = -1; /* Used by SDL_CreateRenderer(...) in Renderer's constructor */

  SDL_Renderer*     m_Renderer          = nullptr; // The actual window renderer
  bool              m_WasInitSuccessful = true;
  bool              m_MediaLoaded       = false;
  
  Texture           m_NormalButtonsSpriteSheet;
  Texture           m_PressedButtonsSpriteSheet;
  const std::string m_NormalButtonsSpriteSheetPath  = "./Sprites/Keys_Normal_400x600.png";
  const std::string m_PressedButtonsSpriteSheetPath = "./Sprites/Keys_Pressed_400x600.png";

  std::vector<Button> m_Buttons_Vec;
};


#endif // RENDERER_HPP