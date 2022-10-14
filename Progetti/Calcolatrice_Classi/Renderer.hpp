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

  enum class ComponentsClips_Enum
  {
    DISPLAY = 0,
    PHOTOVOLTAIC_CELL,

    HOW_MANY
  };

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


  Renderer( const Renderer&  ) = delete; // Singleton
  Renderer(       Renderer&& ) = delete; // Singleton

  Renderer& operator=( const Renderer&  ) = delete; // Singleton
  Renderer& operator=(       Renderer&& ) = delete; // Singleton

  static Renderer& Get( void );

  SDL_Renderer*        GetSDLRendererPtr( void );
  Texture&             GetSpriteSheet   ( void );
  // std::vector<GraphicElement>& GetButtonVector  ( void );
  void                 Render           ( void );

private:

   Renderer( void );
  ~Renderer( void );

  void LoadMedia_Pvt      ( void );
  void CreateRenderer_Pvt ( void );
  void CreateShapes_Pvt   ( void );

  SDL_Renderer* m_Renderer          = nullptr; // The actual window renderer
  bool          m_WasInitSuccessful = true;
  bool          m_MediaLoaded       = false;

  Texture                       m_SpriteSheet;
  // std::vector<GraphicElement*>  m_GraphicElem_Vec;

public:
  Button m_Button0;
  Button m_Button1;
  Button m_Button2;
};


#endif // RENDERER_HPP