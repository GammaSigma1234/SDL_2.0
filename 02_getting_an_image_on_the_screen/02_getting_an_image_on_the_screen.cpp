/**
 * @file 02_getting_an_image_on_the_screen.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/02_getting_an_image_on_the_screen/index.php
 *
 * Lievemente modificato da GS.
 *
 * - Per avviare il nostro programma, dobbiamo creare una finestra mediante SDL_CreateWindow.
 * - Per visualizzare qualcosa in quella finestra, dobbiamo poterne manipolare la superficie
 *   mediante SDL_GetWindowSurface.
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
 * @return true in case of success; false otherwise
 **/
static bool init(void)
{
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "\nSDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    success = false;
    SDL_Delay(3000);
  }
  else
  {
    printf( "\nSDL initialised" );
    SDL_Delay(1000);

    // Create window
    gWindow = SDL_CreateWindow( "\nSDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "\nWindow could not be created! SDL_Error: %s\n", SDL_GetError() );
      success = false;
      SDL_Delay(3000);
    }
    else
    {
      printf( "\nWindow created" );
      SDL_Delay(1000);

      // Get window surface
      gWindowSurface = SDL_GetWindowSurface( gWindow );

      if( gWindowSurface == NULL )
      {
        printf( "\nSurface could not be created! SDL_Error: %s\n", SDL_GetError() );
        success = false;
        SDL_Delay(3000);
      }
      else
      {
        printf( "\nSurface created" );
        SDL_Delay(1000);
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
  gHelloWorld = SDL_LoadBMP( Image );

  if( gHelloWorld == NULL )
  {
    printf( "\nUnable to load image %s! SDL Error: %s\n", Image, SDL_GetError() );
    success = false;
    SDL_Delay(3000);
  }
  else
  {
    printf( "\nImage loaded" );
    SDL_Delay(1000);
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
  printf( "\n***** Debugging console *****\n" );

  bool HasProgramSucceeded = true;

  // Start up SDL and create window
  if( !init() )
  {
    printf( "\nFailed to initialize!" );
    SDL_Delay(3000);
  }
  else
  {
    printf( "\nSDL initialised" );
    SDL_Delay(1000);

    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!" );
      SDL_Delay(3000);
    }
    else
    {
      printf( "\nMedia loaded" );
      SDL_Delay(1000);

      // Apply the image
      SDL_BlitSurface( gHelloWorld, NULL, gWindowSurface, NULL );
      printf( "\nSurface blitted" );
      SDL_Delay(1000);

      // Update the surface
      SDL_UpdateWindowSurface( gWindow );
      printf( "\nWindow's surface updated" );

      // Wait two seconds
      SDL_Delay( 2000 );
    }
  }

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

  // Free resources and close SDL
  close();

  return 0;
}