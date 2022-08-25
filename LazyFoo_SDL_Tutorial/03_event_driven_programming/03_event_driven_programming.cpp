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
static constexpr int    WINDOW_W = 640;
static constexpr int    WINDOW_H = 480;

static constexpr size_t ONE_SECOND_S = 1;
static constexpr size_t WAIT_TIME_S  = 3;    // How long to wait (in seconds) in case of error messages
static constexpr size_t MS_IN_S      = 1000; // Milliseconds in a second


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void ErrorTimer(void); // Esperimento by GS - Abbandonata negli esempi successivi


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*  gWindow        = NULL; // The window we'll be rendering to
static SDL_Surface* gWindowSurface = NULL; // The surface contained by the window
static SDL_Surface* gXOut          = NULL; // The image we will load and show on the screen
static const char*  Image          = "x.bmp";


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

/**
 * @brief Starts up SDL and creates window
 *
 * @return true in case of success; false otherwise
 **/
static bool init(void)
{
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "\nSDL could not initialize! SDL_Error: %s", SDL_GetError() );
    success = false;
    ErrorTimer();
  }
  else
  {
    printf( "\nSDL initialised" );

    // Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "\nWindow could not be created! SDL_Error: %s", SDL_GetError() );
      success = false;
      ErrorTimer();
    }
    else
    {
      printf( "\nWindow created" );

      // Get window surface
      gWindowSurface = SDL_GetWindowSurface( gWindow );

      if( gWindowSurface == NULL )
      {
        printf( "\nSurface could not be created! SDL_Error: %s", SDL_GetError() );
        success = false;
        SDL_Delay(3000);
      }
      else
      {
        printf( "\nSurface created" );
      }

    } // Window created

  } // SDL initialised

  return success;
}


/**
 * @brief Loads media
 *
 * @return true in case of success; false otherwise
 **/
static bool loadMedia(void)
{
  // Loading success flag
  bool success = true;

  // Load splash image
  gXOut = SDL_LoadBMP( Image );

  if( gXOut == NULL )
  {
    printf( "\nUnable to load image %s! SDL Error: %s", Image, SDL_GetError() );
    success = false;
    ErrorTimer();
  }
  else
  {
    printf( "\nImage \"%s\" loaded", Image );
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
  bool HasProgramSucceeded = true;

  printf("\n*** Debugging console ***\n");
  printf("\nProgram started with %d additional arguments.", argc - 1); // Il primo argomento è il nome dell'eseguibile

  for (int i = 1; i != argc; ++i)
  {
    printf("\nArgument #%d: %s\n", i, args[i]);
  }

  // Start up SDL and create window
  if( !init() )
  {
    printf( "\nFailed to initialize!" );
    HasProgramSucceeded = false;
    ErrorTimer();
  }
  else
  {
    printf( "\nSDL initialised" );

    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!" );
      HasProgramSucceeded = false;
			ErrorTimer();
    }
    else
    {
      printf( "\nAll media have been loaded" );

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
        SDL_BlitSurface( gXOut, NULL, gWindowSurface, NULL );
        // printf( "\nSurface blitted" ); // Commentata perché continua a ripetere (è in un ciclo while!)

        // Update the surface
        SDL_UpdateWindowSurface( gWindow );
        // printf( "\nWindow's surface updated" ); // Commentata perché continua a ripetere (è in un ciclo while!)
      }

    } // All media loaded

  } // SDL initialised

  // Free resources and close SDL
  close();

  // Integrity check - Questi messaggi saranno leggibili solo grazie all'uso di SDL_Delay.
  // Nei prossimi esempi si userà una funzione di nome "PressEnter".
  if ( HasProgramSucceeded == true )
  {
    printf("\nProgram ended successfully! Exiting in a few seconds...\n");
    SDL_Delay(2000);
  }
  else
  {
    printf("\nThere was a problem during the execution of the program!\n");
    SDL_Delay(2000);
  }

  return 0;
}