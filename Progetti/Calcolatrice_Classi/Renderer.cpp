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

  CreateButtons_Pvt();
}


/**
 * @brief Destructor
 **/
Renderer::~Renderer(void)
{
  printf( "Renderer's destructor called\n" );
}


/**
 * @brief Creates instance of singleton in static memory
 *
 * @return Renderer&
 **/
Renderer& Renderer::Get(void)
{
  static Renderer Instance;
  return Instance;
}


SDL_Renderer* Renderer::GetSDLRendererPtr(void)
{
  return m_Renderer;
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


void Renderer::LoadMedia_Pvt( void )
{
  if( !m_Keys.loadFromFile( m_KeysPath, m_Renderer ) )
  {
    printf( "\nFailed to load \"%s\"!\n", m_KeysPath.c_str() );
    m_WasInitSuccessful = false;
  }
  else
  {
    printf( "\nTexture creation OK." );
  }
}


void Renderer::CreateButtons_Pvt(void)
{
  m_Button_1.setPosition(0, 600);
  m_Button_1.setClip(SDL_Rect{0, 400, Button::BUTTON_W_px, Button::BUTTON_H_px});

  m_Button_2.setPosition(100, 600);
  m_Button_2.setClip(SDL_Rect{100, 400, Button::BUTTON_W_px, Button::BUTTON_H_px});
}


Texture& Renderer::GetKeysTexture ( void )
{
  return m_Keys;
}

/**
 * @brief Performs all the rendering.
 **/
void Renderer::Render(void)
{
  SDL_SetRenderDrawColor( m_Renderer, WHITE_R, WHITE_G, WHITE_B, ALPHA_MAX );

  SDL_RenderClear( m_Renderer );

  m_Button_1.render();
  m_Button_2.render();

  SDL_RenderPresent( m_Renderer ); // Update screen
}