/**
 * @file 02_getting_an_image_on_the_screen.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/02_getting_an_image_on_the_screen/index.php
 *
 * Lievemente modificato da GS.
 *
 * - Per avviare il nostro programma, dobbiamo creare una finestra.
 * - Per visualizzare qualcosa in quella finestra, dobbiamo poterne manipolare la superficie.
 * - Se vogliamo ad esempio visualizzare un immagine bitmap nella superficie della finestra,
 *   dobbiamo creare un'altra superficie, su cui caricheremo l'immagine, e "blittare" la
 *   superficie contenente l'immagine sulla superficie della finestra.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022) and may not be
 * redistributed without written permission.
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

// Screen dimension constants
static constexpr int SCREEN_WIDTH  = 640;
static constexpr int SCREEN_HEIGHT = 480;


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*  gWindow        = NULL; // The window we'll be rendering to
static SDL_Surface* gWindowSurface = NULL; // The surface contained by the window
static SDL_Surface* gHelloWorld    = NULL; // The image we will load and show on the screen
static const char*  Image          = ".\\hello_world.bmp"; // The path to the image to be loaded

/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

/**
 * @brief Starts up SDL and creates window
 *
 * @return true
 * @return false
 **/
static bool init(void)
{
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    success = false;
    SDL_Delay(3000);
  }
  else
  {
    // Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
      success = false;
      SDL_Delay(3000);
    }
    else
    {
      // Get window surface
      gWindowSurface = SDL_GetWindowSurface( gWindow );
    }
  }

  return success;
}


/**
 * @brief Loads media
 *
 * @return true
 * @return false
 **/
static bool loadMedia(void)
{
  // Loading success flag
  bool success = true;

  // Load splash image
  gHelloWorld = SDL_LoadBMP( Image );

  if( gHelloWorld == NULL )
  {
    printf( "Unable to load image %s! SDL Error: %s\n", Image, SDL_GetError() );
    success = false;
    SDL_Delay(3000);
  }

  return success;
}


/**
 * @brief Frees media and shuts down SDL
 **/
static void close(void)
{
  // Deallocate surface
  SDL_FreeSurface( gHelloWorld );
  gHelloWorld = NULL;

  // Destroy window
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  // Quit SDL subsystems
  SDL_Quit();
}


/***************************************************************************************************
* Main function
****************************************************************************************************/

int main( int argc, char* args[] )
{
  // Start up SDL and create window
  if( !init() )
  {
    printf( "Failed to initialize!\n" );
    SDL_Delay(3000);
  }
  else
  {
    // Load media
    if( !loadMedia() )
    {
      printf( "Failed to load media!\n" );
      SDL_Delay(3000);
    }
    else
    {
      // Apply the image
      SDL_BlitSurface( gHelloWorld, NULL, gWindowSurface, NULL );

      // Update the surface
      SDL_UpdateWindowSurface( gWindow );

      // Wait two seconds
      SDL_Delay( 2000 );
    }
  }

  // Free resources and close SDL
  close();

  return 0;
}