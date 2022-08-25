/**
 * @file 06_extension_libraries_and_loading_other_image_formats.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/06_extension_libraries_and_loading_other_image_formats/index.php
 *
 * @brief SDL extension libraries allow you do things like load image files besides BMP, render TTF
 * fonts, and play music. You can set up SDL_image to load PNG files, which can save you a lot of
 * disk space. SDL itself is an extension library, since it adds game and media functionality that
 * doesn't come standard with your C++ compiler.
 *
 * Now that we're using SDL_image, we need to initialize it. Here we want to initialize
 * SDL_image with PNG loading, so we pass in the PNG loading flags into IMG_Init. IMG_Init returns
 * the flags that loaded successfully. If the flags that are returned do not contain the flags we
 * requested, that means there's an error.
 *
 * Our image loading function is pretty much the same as before, only now it uses IMG_Load as
 * opposed to SDL_LoadBMP. When there's an error with SDL_image, you get error string with
 * IMG_GetError as opposed to SDL_GetError.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

// Screen dimension constants
static constexpr int     WINDOW_W = 640;
static constexpr int     WINDOW_H = 480;
static const std::string Img_Path("loaded.png");


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void PressEnter(void);

static SDL_Surface* loadSurface( std::string path );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*  gWindow        = NULL; // The window we'll be rendering to
static SDL_Surface* gWindowSurface = NULL; // The surface contained by the window
static SDL_Surface* gPNGSurface    = NULL; // Current displayed PNG image


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

/**
 * @brief Starts up SDL and creates window
 *
 * @return bool
 **/
bool init(void)
{
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    success = false;
    PressEnter();
  }
  else
  {
    printf( "\nSDL initialised" );

    // Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
      success = false;
      PressEnter();
    }
    else
    {
      printf( "\nWindow created" );

      // Initialize PNG loading
      int imgFlags = IMG_INIT_PNG;

      if( !( IMG_Init( imgFlags ) & imgFlags ) )
      {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        success = false;
        PressEnter();
      }
      else
      {
        printf( "\nSDL_image initialised" );

        // Get window surface
        gWindowSurface = SDL_GetWindowSurface( gWindow );

        if( gWindowSurface == nullptr )
        {
          printf( "SDL_GetWindowSurface could not initialize! SDL Error: %s\n", SDL_GetError() );
          success = false;
          PressEnter();
        }
        else
        {
          // All OK
          printf( "\nWindow surface initialised" );
        }
      }
    }
  }

  return success;
}


/**
 * @brief Loads all necessary media for this project.
 *
 * @return true if loading was successful; false otherwise
 **/
static bool loadMedia(void)
{
  // Loading success flag
  bool success = true;

  // Load PNG surface
  gPNGSurface = loadSurface( Img_Path );

  if( gPNGSurface == NULL )
  {
    printf( "Failed to load PNG image \"%s\"!\n", Img_Path );
    success = false;
    PressEnter();
  }
  else
  {
    printf( "\n\"%s\" loaded", Img_Path.c_str() );
  }

  return success;
}


/**
 * @brief Frees media and shuts down SDL
 **/
static void close(void)
{
  // Free loaded image
  SDL_FreeSurface( gPNGSurface );
  gPNGSurface = NULL;

  // Destroy window
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}


/**
 * @brief Loads individual image. Optimised version (converts image to 32 bits).
 *
 * @param path The path to the image
 * @return SDL_Surface* Pointer to the loaded image
 **/
static SDL_Surface* loadSurface( std::string path )
{
  // The final optimized image
  SDL_Surface* optimizedSurface = NULL;

  // Load image at specified path. Uses IMG_Load instead of SDL_LoadBMP
  SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

  if( loadedSurface == NULL )
  {
    printf( "\nUnable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    PressEnter();
  }
  else
  {
    // Convert surface to screen format
    optimizedSurface = SDL_ConvertSurface( loadedSurface, gWindowSurface->format, 0 );

    if( optimizedSurface == NULL )
    {
      printf( "\nUnable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
      PressEnter();
    }
    else
    {
      printf("\nSurface converted to screen format");
    }

    // Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
  }

  return optimizedSurface;
}


static void PressEnter(void)
{
  int UserChoice = '\0';

  printf("\nPress ENTER to exit...");

  while ( ( UserChoice = getchar() ) != '\n' )
  {;}
}


/***************************************************************************************************
* Main function
****************************************************************************************************/

int main( int argc, char* args[] )
{
  bool HasProgramSucceeded = true;

  printf("\n*** Debugging console ***\n");

  // Start up SDL and create window
  if( !init() )
  {
    printf( "Failed to initialize!\n" );
    PressEnter();
  }
  else
  {
    // Load media
    if( !loadMedia() )
    {
      printf( "Failed to load media!\n" );
      PressEnter();
    }
    else
    {
      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

      // While application is running
      while( !quit )
      {
        // Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
          // User requests quit
          if( e.type == SDL_QUIT )
          {
            quit = true;
          }
        }

        // Apply the PNG image
        SDL_BlitSurface( gPNGSurface, NULL, gWindowSurface, NULL );

        // Update the surface
        SDL_UpdateWindowSurface( gWindow );
      }
    }
  }

  // Free resources and close SDL
  close();

  // Integrity check
  if ( HasProgramSucceeded == true )
  {
    printf("\nProgram ended successfully!\n");
  }
  else
  {
    printf("\nThere was a problem during the execution of the program!\n");
  }

  PressEnter();

  return 0;
}