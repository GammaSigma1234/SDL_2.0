#include "MainWindow.hpp"

#include <cstdio>
#include <SDL.h>

/***************************************************************************************************
* Static members
****************************************************************************************************/

SDL_Window* MainWindow::s_Window;
bool        MainWindow::s_WasInitSuccessful;


/***************************************************************************************************
* Methods
****************************************************************************************************/

/**
 * @brief Constructor
 **/
MainWindow::MainWindow(void)
{
  /* Initialize SDL */

  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "\nSDL could not initialize! SDL Error: \"%s\"\n", SDL_GetError() );
    s_WasInitSuccessful = false;
  }
  else
  {
    printf( "\nOK: SDL initialised" );
  }

  // Set texture filtering to linear
  if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
  {
    printf( "\nWarning: Linear texture filtering not enabled!" );
    s_WasInitSuccessful = false;
  }
  else
  {
    printf( "\nOK: linear texture filtering enabled" );
  }

  /* Create window */

  s_Window = SDL_CreateWindow( "SDL Tutorial"          ,
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                s_WINDOW_W             , s_WINDOW_H             ,
                                SDL_WINDOW_SHOWN );

  if( s_Window == NULL )
  {
    printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
    s_WasInitSuccessful = false;
  }
  else
  {
    printf( "\nOK: window created" );
  }
}


SDL_Window* MainWindow::Get(void)
{
  return s_Window; // Get static instance
}


void MainWindow::Debug(void)
{
  printf("\nCiao");
}