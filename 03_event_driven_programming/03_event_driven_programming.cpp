/**
 * @file 03_event_driven_programming.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/03_event_driven_programming/index.php
 *
 * Nel main loop while( !quit ) continuiamo a fare l'update dell'immagine da visualizzare, cioè il
 * sistema sta continuamente ridisegnando la superficie della finestra. Essendo una procedura molto
 * veloce, l'occhio umano non se ne accorge. Negli esempi precedenti non c'era un ciclo while, bensì
 * l'immagine veniva disegnata una volta, e poi congelata per alcuni secondi mediante
 * SDL_Delay(...).
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
static constexpr int    SCREEN_WIDTH  = 640;
static constexpr int    SCREEN_HEIGHT = 480;

static constexpr size_t ONE_SECOND_S  = 1;
static constexpr size_t WAIT_TIME_S   = 3;    // How long to wait (in seconds) in case of error messages
static constexpr size_t MS_IN_S       = 1000; // Milliseconds in a second


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void ErrorTimer(void);


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*  gWindow        = NULL; // The window we'll be rendering to
static SDL_Surface* gScreenSurface = NULL; // The surface contained by the window
static SDL_Surface* gXOut          = NULL; // The image we will load and show on the screen
static const char*  Image          = "x.bmp";


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
    ErrorTimer();
  }
  else
  {
    // Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
      success = false;
      ErrorTimer();
    }
    else
    {
      // Get window surface
      gScreenSurface = SDL_GetWindowSurface( gWindow );
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
  gXOut = SDL_LoadBMP( Image );
  if( gXOut == NULL )
  {
    printf( "Unable to load image %s! SDL Error: %s\n", Image, SDL_GetError() );
    success = false;
    ErrorTimer();
  }

  return success;
}


/**
 * @brief Frees media and shuts down SDL
 **/
static void close(void)
{
  // Deallocate surface
  SDL_FreeSurface( gXOut );
  gXOut = NULL;

  // Destroy window
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  // Quit SDL subsystems
  SDL_Quit();
}


/**
 * @brief Used to keep the console open for a short time when an error message is displayed, to
 * allow the user to read it before the console automatically closes.
 **/
static void ErrorTimer(void)
{
  for (size_t i = 0; i != WAIT_TIME_S; ++i)
  {
    printf(".");
    SDL_Delay(ONE_SECOND_S * MS_IN_S);
  }

  printf("\n");
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
    ErrorTimer();
  }
  else
  {
    // Load media
    if( !loadMedia() )
    {
      printf( "Failed to load media!\n" );
			ErrorTimer();
    }
    else
    {
      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

      // While application is running (main loop)
      while( !quit )
      {
        // Handle events on queue (event loop)
        // If events queue is empty, returns 0
        while( SDL_PollEvent( &e ) != 0 )
        {
          // User requests quit
          if( e.type == SDL_QUIT )
          {
            quit = true;
          }
					else
					{;} // Continue running program
        }

        // Apply the image
        SDL_BlitSurface( gXOut, NULL, gScreenSurface, NULL );

        // Update the surface
        SDL_UpdateWindowSurface( gWindow );
      }
    }
  }

  // Free resources and close SDL
  close();

  return 0;
}