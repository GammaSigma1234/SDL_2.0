/***************************************************************************************************
* Includes
****************************************************************************************************/

#include <cstdio>
#include "MainWindow.hpp"
#include "Supervisor.hpp"
#include "Renderer.hpp"
#include "..\..\Colours_Lib\colours.hpp"


/***************************************************************************************************
* Private constants
****************************************************************************************************/


/***************************************************************************************************
* Methods
****************************************************************************************************/

/**
 * @brief Constructor
 **/
Renderer::Renderer( void )
{
  printf( "\nInitialising Renderer...\n");

  CreateRenderer_Pvt();

  LoadMedia_Pvt();

  CreateShapes_Pvt();
}


/**
 * @brief Destructor
 **/
Renderer::~Renderer(void)
{
  printf( "Renderer's destructor called\n" );
}


void Renderer::CreateRenderer_Pvt(void)
{
  m_Renderer = SDL_CreateRenderer( MainWindow::Get().GetSDLWindowPtr(), FIRST_ONE, SDL_RENDERER_ACCELERATED );

  if( m_Renderer == NULL )
  {
    printf( "\tRenderer could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
    printf( "\tMainWindow::Get() = %p\n", static_cast<void*>( MainWindow::Get().GetSDLWindowPtr() ) );
    Supervisor::Get().RaiseFault();
  }
  else
  {
    printf( "\tOK: Renderer created. Address: %p\n", static_cast<void*>( m_Renderer ) );
  }
}


/**
 * @brief Loads all media needed for this project.
 **/
void Renderer::LoadMedia_Pvt( void )
{
  size_t SpriteSheetsNum(static_cast<size_t>(SpriteSheets_Enum::HOW_MANY));

  m_SpriteSheets_Paths.push_back("./Sprites/Keys_Normal_400x600.png");
  m_SpriteSheets_Paths.push_back("./Sprites/Keys_Pressed_400x600.png");

  if ( m_SpriteSheets_Paths.size() == SpriteSheetsNum )
  {
    printf("\nSprite sheets enumeration OK.");
  }
  else
  {
    printf("\n*** WARNING ***\n\tNumber of specified sprite sheets: %llu. Expected: %llu.", m_SpriteSheets_Paths.size(), SpriteSheetsNum);
  }

  m_SpriteSheets_Vec.resize(SpriteSheetsNum);

  for (size_t i = 0; i != SpriteSheetsNum; ++i)
  {
    m_SpriteSheets_Vec[i].loadFromFile( m_SpriteSheets_Paths[i], m_Renderer );

    if( m_SpriteSheets_Vec[i].isValid() )
    {
      printf( "\nTexture creation OK." );
    }
    else
    {
      printf( "\nFailed to load \"%s\"!\n", m_SpriteSheets_Paths[i].c_str() );
      m_WasInitSuccessful = false;
    }
  }
}


void Renderer::CreateShapes_Pvt(void)
{
  m_Buttons_Vec.resize(static_cast<size_t>(ButtonsClips_Enum::HOW_MANY));

  int Row_0(0);
  int Row_1(100);
  int Row_2(200);
  int Row_3(300);
  int Row_4(400);
  int Row_5(500);

  int Column_0(0);
  int Column_1(100);
  int Column_2(200);
  int Column_3(300);

  size_t Key_0(static_cast<size_t>(ButtonsClips_Enum::KEY_0));
  m_Buttons_Vec[Key_0].setPosition(0, 600);
  m_Buttons_Vec[Key_0].setClip(SDL_Rect{Column_0, Row_5, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_Point(static_cast<size_t>(ButtonsClips_Enum::KEY_POINT));
  m_Buttons_Vec[Key_Point].setPosition(100, 600);
  m_Buttons_Vec[Key_Point].setClip(SDL_Rect{Column_1, Row_5, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_Equals(static_cast<size_t>(ButtonsClips_Enum::KEY_EQUALS));
  m_Buttons_Vec[Key_Equals].setPosition(200, 600);
  m_Buttons_Vec[Key_Equals].setClip(SDL_Rect{Column_2, Row_5, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_1(static_cast<size_t>(ButtonsClips_Enum::KEY_1));
  m_Buttons_Vec[Key_1].setPosition(0, 500);
  m_Buttons_Vec[Key_1].setClip(SDL_Rect{Column_0, Row_4, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_2(static_cast<size_t>(ButtonsClips_Enum::KEY_2));
  m_Buttons_Vec[Key_2].setPosition(100, 500);
  m_Buttons_Vec[Key_2].setClip(SDL_Rect{Column_1, Row_4, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_3(static_cast<size_t>(ButtonsClips_Enum::KEY_3));
  m_Buttons_Vec[Key_3].setPosition(200, 500);
  m_Buttons_Vec[Key_3].setClip(SDL_Rect{Column_2, Row_4, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_4(static_cast<size_t>(ButtonsClips_Enum::KEY_4));
  m_Buttons_Vec[Key_4].setPosition(0, 400);
  m_Buttons_Vec[Key_4].setClip(SDL_Rect{Column_0, Row_3, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_5(static_cast<size_t>(ButtonsClips_Enum::KEY_5));
  m_Buttons_Vec[Key_5].setPosition(100, 400);
  m_Buttons_Vec[Key_5].setClip(SDL_Rect{Column_1, Row_3, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_6(static_cast<size_t>(ButtonsClips_Enum::KEY_6));
  m_Buttons_Vec[Key_6].setPosition(200, 400);
  m_Buttons_Vec[Key_6].setClip(SDL_Rect{Column_2, Row_3, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_7(static_cast<size_t>(ButtonsClips_Enum::KEY_7));
  m_Buttons_Vec[Key_7].setPosition(0, 300);
  m_Buttons_Vec[Key_7].setClip(SDL_Rect{Column_0, Row_2, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_8(static_cast<size_t>(ButtonsClips_Enum::KEY_8));
  m_Buttons_Vec[Key_8].setPosition(100, 300);
  m_Buttons_Vec[Key_8].setClip(SDL_Rect{Column_1, Row_2, Button::BUTTON_W_px, Button::BUTTON_H_px});

  size_t Key_9(static_cast<size_t>(ButtonsClips_Enum::KEY_9));
  m_Buttons_Vec[Key_9].setPosition(200, 300);
  m_Buttons_Vec[Key_9].setClip(SDL_Rect{Column_2, Row_2, Button::BUTTON_W_px, Button::BUTTON_H_px});
}

Texture& Renderer::GetSpriteSheet( SpriteSheets_Enum ss )
{
  size_t SpriteSheets_Index;

  switch ( ss )
  {
    default:
    case SpriteSheets_Enum::NORMAL_BUTTONS:
      SpriteSheets_Index = static_cast<size_t>(SpriteSheets_Enum::NORMAL_BUTTONS);
      return m_SpriteSheets_Vec[SpriteSheets_Index];
      break;

    case SpriteSheets_Enum::PRESSED_BUTTONS:
      SpriteSheets_Index = static_cast<size_t>(SpriteSheets_Enum::PRESSED_BUTTONS);
      return m_SpriteSheets_Vec[SpriteSheets_Index];
      break;
  }
}


std::vector<Button>& Renderer::GetButtonVector( void )
{
  return m_Buttons_Vec;
}


/**
 * @brief Performs all the rendering.
 **/
void Renderer::Render(void)
{
  SDL_SetRenderDrawColor( m_Renderer, WHITE_R, WHITE_G, WHITE_B, ALPHA_MAX );

  SDL_RenderClear( m_Renderer );

  for (auto& Button : m_Buttons_Vec)
  {
    Button.render();
  }

  SDL_RenderPresent( m_Renderer ); // Update screen
}


/**
 * @brief Use this function to invoke the Renderer when needed.
 *
 * @return Renderer&
 **/
Renderer& Renderer::Get(void)
{
  static Renderer Instance; // Creates instance of singleton in static memory.
  return Instance;
}


SDL_Renderer* Renderer::GetSDLRendererPtr(void)
{
  return m_Renderer;
}