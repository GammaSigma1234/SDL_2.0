#include "MainWindow.hpp"

#include <cstdio>
#include <SDL.h>


MainWindow MainWindow::MainWindow_Singleton; // Static instance
SDL_Window* MainWindow::m_Window;

MainWindow::MainWindow(void)
{
  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "\nSDL could not initialize! SDL Error: \"%s\"\n", SDL_GetError() );
    m_SuccessfulInit = false;
  }
  else
  {
    printf( "\nOK: SDL initialised" );

    // Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
      printf( "\nWarning: Linear texture filtering not enabled!" );
      m_SuccessfulInit = false;
		}
    else
    {
      printf( "\nOK: linear texture filtering enabled" );
    }

    // Create window
    m_Window = SDL_CreateWindow( "SDL Tutorial"        ,
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                s_WINDOW_W             , s_WINDOW_H             ,
                                SDL_WINDOW_SHOWN );

    if( m_Window == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
      m_SuccessfulInit = false;
    }
    else
    {
      printf( "\nOK: window created" );
    }
  }
}


SDL_Window* MainWindow::Get(void)
{
  return m_Window; // Get static instance
}


void MainWindow::Debug(void)
{
  printf("\nCiao");
}