#include <cstdio>
#include "MainWindow.hpp"
#include "Renderer.hpp"
#include "..\..\Colours_Lib\colours.hpp"

/***************************************************************************************************
* Static members
****************************************************************************************************/

SDL_Renderer* Renderer::s_Renderer;
bool          Renderer::s_WasInitSuccessful;


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
  printf( "\nInitialising renderer...\n");
  s_Renderer = SDL_CreateRenderer( MainWindow::Get(), FIRST_ONE, SDL_RENDERER_ACCELERATED );

  if( s_Renderer == NULL )
  {
    printf( "\tRenderer could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
    printf( "\tMainWindow::Get() = %p\n", static_cast<void*>( MainWindow::Get() ) );
  }
  else
  {
    printf( "\tOK: renderer created\n" );
  }
}


/**
 * @brief Destructor
 **/
Renderer::~Renderer(void)
{
  printf( "Renderer's destructor called\n" );
}


SDL_Renderer* Renderer::Get(void)
{
  return s_Renderer;
}


/**
 * @brief Performs all the rendering.
 **/
void Renderer::Render(void)
{
  const int WINDOW_W = MainWindow::GetWindowWidth();
  const int WINDOW_H = MainWindow::GetWindowHeight();

  // We're setting the clearing color to white at every frame as opposed to setting it once in
  // the initialization function
  SDL_SetRenderDrawColor( Renderer::Get(), WHITE_R, WHITE_G, WHITE_B, ALPHA_MAX );

  // Clear screen
  SDL_RenderClear( Renderer::Get() );

  // Render red filled quad
  SDL_Rect fillRect = { WINDOW_W / 4, WINDOW_H / 4, WINDOW_W / 2, WINDOW_H / 2 };
  SDL_SetRenderDrawColor( Renderer::Get(), RED_R, RED_G, RED_B, ALPHA_MAX );
  SDL_RenderFillRect( Renderer::Get(), &fillRect );

  // Render green outlined quad
  SDL_Rect outlineRect = { WINDOW_W / 6, WINDOW_H / 6, WINDOW_W * 2 / 3, WINDOW_H * 2 / 3 };
  SDL_SetRenderDrawColor( Renderer::Get(), GREEN_R, GREEN_G, GREEN_B, ALPHA_MAX );
  SDL_RenderDrawRect( Renderer::Get(), &outlineRect );

  // Draw blue horizontal line
  SDL_SetRenderDrawColor( Renderer::Get(), BLUE_R, BLUE_G, BLUE_B, ALPHA_MAX );
  SDL_RenderDrawLine( Renderer::Get(), 0, WINDOW_H / 2, WINDOW_W, WINDOW_H / 2 );

  // Draw vertical line of yellow dots
  SDL_SetRenderDrawColor( Renderer::Get(), YELLOW_R, YELLOW_G, YELLOW_B, ALPHA_MAX );
  for( int i = 0; i != WINDOW_H; i += 4 )
  {
    SDL_RenderDrawPoint( Renderer::Get(), WINDOW_W / 2, i );
  }

  // Update screen
  SDL_RenderPresent( Renderer::Get() );
}