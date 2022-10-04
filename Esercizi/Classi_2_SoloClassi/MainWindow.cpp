/***************************************************************************************************
* Includes
****************************************************************************************************/

#include "MainWindow.hpp"
#include "Supervisor.hpp"

#include <cstdio>
#include <SDL.h>


/***************************************************************************************************
* Private constants
****************************************************************************************************/

static auto& Supervisor_Ref(Supervisor::Get());

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
    Supervisor_Ref.RaiseFault();
  }
  else
  {
    printf( "\tOK: SDL initialised\n" );
  }

  // Set texture filtering to linear
  if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
  {
    printf( "\tWarning: Linear texture filtering not enabled!\n" );
    Supervisor_Ref.RaiseFault();
  }
  else
  {
    printf( "\tOK: linear texture filtering enabled\n" );
  }

  /* Create window */

  m_Window = SDL_CreateWindow( "SDL Tutorial"          ,
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                s_WINDOW_W             , s_WINDOW_H             ,
                                SDL_WINDOW_SHOWN );

  if( m_Window == NULL )
  {
    printf( "\tWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
    Supervisor_Ref.RaiseFault();
  }
  else
  {
    printf( "\tOK: window created.\n" );
  }
}


/**
 * @brief Destuctor
 **/
MainWindow::~MainWindow(void)
{;}


/**
 * @brief Creates instance of singleton in static memory
 *
 * @return MainWindow&
 **/
MainWindow& MainWindow::Get(void)
{
  static MainWindow Instance;
  return Instance;
}


SDL_Window* MainWindow::GetSDLWindowPtr(void)
{
  return m_Window;
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

  SDL_DestroyWindow( m_Window );
  m_Window = NULL;
}