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

  m_Renderer = SDL_CreateRenderer( MainWindow::Get().GetSDLWindowPtr(), FIRST_ONE, SDL_RENDERER_ACCELERATED );

  if( m_Renderer == NULL )
  {
    printf( "\tRenderer could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
    printf( "\tMainWindow::Get() = %p\n", static_cast<void*>( MainWindow::Get().GetSDLWindowPtr() ) );
    Supervisor::Get().RaiseFault();
  }
  else
  {
    printf( "\tOK: Renderer created\n" );
  }
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


/**
 * @brief Performs all the rendering.
 **/
void Renderer::Render(void)
{
  const int WINDOW_W = MainWindow::Get().GetWindowWidth();
  const int WINDOW_H = MainWindow::Get().GetWindowHeight();

  // We're setting the clearing color to white at every frame as opposed to setting it once in
  // the initialization function
  SDL_SetRenderDrawColor( m_Renderer, WHITE_R, WHITE_G, WHITE_B, ALPHA_MAX );

  // Clear screen
  SDL_RenderClear( m_Renderer );

  // Render red filled quad
  SDL_Rect fillRect = { WINDOW_W / 4, WINDOW_H / 4, WINDOW_W / 2, WINDOW_H / 2 };
  SDL_SetRenderDrawColor( m_Renderer, RED_R, RED_G, RED_B, ALPHA_MAX );
  SDL_RenderFillRect( m_Renderer, &fillRect );

  // Render green outlined quad
  SDL_Rect outlineRect = { WINDOW_W / 6, WINDOW_H / 6, WINDOW_W * 2 / 3, WINDOW_H * 2 / 3 };
  SDL_SetRenderDrawColor( m_Renderer, GREEN_R, GREEN_G, GREEN_B, ALPHA_MAX );
  SDL_RenderDrawRect( m_Renderer, &outlineRect );

  // Draw blue horizontal line
  SDL_SetRenderDrawColor( m_Renderer, BLUE_R, BLUE_G, BLUE_B, ALPHA_MAX );
  SDL_RenderDrawLine( m_Renderer, 0, WINDOW_H / 2, WINDOW_W, WINDOW_H / 2 );

  // Draw vertical line of yellow dots
  SDL_SetRenderDrawColor( m_Renderer, YELLOW_R, YELLOW_G, YELLOW_B, ALPHA_MAX );
  for( int i = 0; i != WINDOW_H; i += 4 )
  {
    SDL_RenderDrawPoint( m_Renderer, WINDOW_W / 2, i );
  }

  // Update screen
  SDL_RenderPresent( m_Renderer );
}