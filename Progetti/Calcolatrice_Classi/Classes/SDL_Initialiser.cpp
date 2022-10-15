#include "SDL_Initialiser.hpp"
#include "Supervisor.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>

/***************************************************************************************************
* Private constants
****************************************************************************************************/


/***************************************************************************************************
* Methods
****************************************************************************************************/

SDL_Initialiser::SDL_Initialiser(void)
  : m_WasInitSuccessful(true)
{
  printf("\nInitialising SDL subsystems...\n");
  /* Initialise SDL */

  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "\tWarning: SDL could not initialise! SDL Error: \"%s\"\n", SDL_GetError() );
    m_WasInitSuccessful = false;
  }
  else
  {
    printf( "\tOK: SDL initialised.\n" );
  }

  // Set texture filtering to linear
  if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
  {
    printf( "\tWarning: Linear texture filtering not enabled!\n" );
    m_WasInitSuccessful = false;
  }
  else
  {
    printf( "\tOK: linear texture filtering enabled.\n" );
  }

  // Initialize PNG loading
  constexpr int imgFlags( static_cast<int>(IMG_INIT_PNG) );

  if( !( IMG_Init( imgFlags ) & imgFlags ) )
  {
    printf( "\tWarning: SDL_image could not initialise! SDL_Image Error: %s", IMG_GetError() );
    m_WasInitSuccessful = false;
  }
  else
  {
    printf( "\tOK: SDL_image initialised.\n" );
  }
}


SDL_Initialiser::~SDL_Initialiser(void)
{;}


SDL_Initialiser& SDL_Initialiser::Get(void)
{
  static SDL_Initialiser Instance;
  return Instance;
}


bool SDL_Initialiser::isSDLInitialised(void)
{
  // printf("\nm_isSDLInitialised: %d", m_WasInitSuccessful); // TODO: debug
  return m_WasInitSuccessful;
}


void SDL_Initialiser::QuitSDL(void)
{
  printf("\nQuitting SDL subsystems...");
  SDL_Quit();
}