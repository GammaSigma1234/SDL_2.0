/***************************************************************************************************
* Includes
****************************************************************************************************/

#include "MainWindow.hpp"
#include "SDL_Initialiser.hpp"
#include "Supervisor.hpp"

#include <cstdio>
#include <SDL.h>


/***************************************************************************************************
* Private constants
****************************************************************************************************/


/***************************************************************************************************
* Methods
****************************************************************************************************/

/**
 * @brief Create main window
 **/
MainWindow::MainWindow(void)
{
  // if (SDL_Initialiser_Ref.isSDLInitialised()) // Utilizzare "SDL_Initialiser_Ref" provoca un crash all'avvio del programma
  if (SDL_Initialiser::Get().isSDLInitialised())
  {
    printf( "\nInitialising Main Window...\n" );

    m_Window = SDL_CreateWindow( "SDL Tutorial"          ,
                                  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  s_WINDOW_W             , s_WINDOW_H             ,
                                  SDL_WINDOW_SHOWN );

    if( m_Window == NULL )
    {
      printf( "\tWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
      Supervisor::Get().RaiseFault();
    }
    else
    {
      printf( "\tOK: window created.\n" );
    }
  }
  else
  {
    printf( "\nSDL has not been properly initialised! Cannot create main window.\n" );
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