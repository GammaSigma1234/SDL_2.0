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
  printf( "\nInitialising Main Window...\n" );

  /* Initialise SDL */

  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "\tSDL could not initialise! SDL Error: \"%s\"\n", SDL_GetError() );
    s_WasInitSuccessful = false;
  }
  else
  {
    printf( "\tOK: SDL initialised\n" );
  }

  // Set texture filtering to linear
  if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
  {
    printf( "\tWarning: Linear texture filtering not enabled!\n" );
    s_WasInitSuccessful = false;
  }
  else
  {
    printf( "\tOK: linear texture filtering enabled\n" );
  }

  /* Create window */

  s_Window = SDL_CreateWindow( "SDL Tutorial"          ,
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                s_WINDOW_W             , s_WINDOW_H             ,
                                SDL_WINDOW_SHOWN );

  if( s_Window == NULL )
  {
    printf( "\tWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
    s_WasInitSuccessful = false;
  }
  else
  {
    printf( "\tOK: window created.\n" );
  }
}


MainWindow::~MainWindow(void)
{;}


SDL_Window* MainWindow::Get(void)
{
  return s_Window; // Get static instance
}


int MainWindow::GetWindowWidth(void)
{
  return s_WINDOW_W;
}


int MainWindow::GetWindowHeight(void)
{
  return s_WINDOW_H;
}


void MainWindow::DestroyWindow(void)
{
  printf( "\nDestroying Main Window...\n" );
  
  SDL_DestroyWindow( MainWindow::Get() );
  s_Window = NULL;
}