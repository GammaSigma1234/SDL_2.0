#include <cstdio>
#include "MainWindow.hpp"
#include "Renderer.hpp"


Renderer Renderer::Renderer_Singleton;


Renderer* Renderer::Get(void)
{
  return &Renderer_Singleton;
}


Renderer::Renderer(/* args */)
{
  gRenderer = SDL_CreateRenderer( MainWindow::Get(), FIRST_ONE, SDL_RENDERER_ACCELERATED );

  if( gRenderer == NULL )
  {
    printf( "\nRenderer could not be created! SDL Error: %s", SDL_GetError() );
    printf( "\nMainWindow::Get() = %p", MainWindow::Get() );
  }
  else
  {
    printf( "\nOK: renderer created" );
  }
}

Renderer::~Renderer(void)
{;}
