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

  SDL_Renderer* GetSDLRendererPtr( void );
  Texture&      GetKeysTexture   ( void );
  void          Render           ( void );

private:

   Renderer( void );
  ~Renderer( void );

  void LoadMedia_Pvt      ( void );
  void CreateRenderer_Pvt ( void );
  void CreateButtons_Pvt  ( void );

  enum m_KeysClips_Enum
  {
    KEY_1,
    KEY_2,
    KEY_3,
    
    HOW_MANY
  };

  static constexpr int  FIRST_ONE = -1; /* Used by SDL_CreateRenderer(...) in Renderer's constructor */

  SDL_Renderer*     m_Renderer          = nullptr; // The actual window renderer
  bool              m_WasInitSuccessful = true;
  bool              m_MediaLoaded       = false;
  
  Texture           m_Keys;
  Texture           m_KeysPressed;
  const std::string m_KeysPath        = "./Sprites/Keys_400x600.png";
  const std::string m_KeysPressedPath = "./Sprites/Keys_Pressed.png";

  // std::vector<Button> m_Buttons;
  Button m_Button_1;
  Button m_Button_2;
};


#endif // RENDERER_HPP