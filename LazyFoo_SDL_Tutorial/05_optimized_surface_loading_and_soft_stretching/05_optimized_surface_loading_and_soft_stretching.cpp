/**
 * @file 05_optimized_surface_loading_and_soft_stretching.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/05_optimized_surface_loading_and_soft_stretching/index.php
 *
 * @brief Up until now we've been blitting our images raw. Since we were only showing one image, it
 * didn't matter. When you're making a game, blitting images raw causes needless slow down. We'll be
 * converting them to an optimized format to speed them up.
 *
 * SDL 2 also has a new feature for SDL surfaces called soft stretching, which allows you to blit an
 * image scaled to a different size. In this tutorial we'll take an image half the size of the
 * screen and stretch it to the full size.
 *
 * La versione ottimizzata di "loadSurface", "loadSurface_Optimised", utilizza a sua volta una
 * chiamata a "SDL_ConvertSurface" per migliorare l'immagine utilizzata. Occorre però ricordarsi di
 * liberare la vecchia immagine, perché inutilizzata.
 *
 * La funzione "SDL_BlitScaled" esegue il blitting dell'immagine con uno stretch. Il quarto
 * parametro serve probabilmente a definire uno stiramento e una posizione personalizzata, dal
 * momento che pare non essere un argomento indispensabile. Infatti, passando NULL come quarto
 * argomento a "SDL_BlitScaled", l'immagine BMP originale viene stirata fino a riempire tutta la
 * finestra.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, standard IO, and strings
#include <SDL.h>
#include <stdio.h>
#include <string>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

// Screen dimension constants
static constexpr int     WINDOW_W = 1024;
static constexpr int     WINDOW_H = 768;
static const std::string Img("stretch.bmp");


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void PressEnter(void);

static SDL_Surface* loadSurface_Optimised( const std::string& path );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*  gWindow           = NULL; // The window we'll be rendering to
static SDL_Surface* gWindowSurface    = NULL; // The surface contained by the window
static SDL_Surface* gStretchedSurface = NULL; // Current displayed image


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

/**
 * @brief Starts up SDL and creates window
 *
 * @return true
 * @return false
 **/
bool init(void)
{
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "\nSDL could not initialize! SDL Error: \"%s\"", SDL_GetError() );
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
      printf( "\nWindow could not be created! SDL Error: \"%s\"", SDL_GetError() );
      success = false;
      PressEnter();
    }
    else
    {
      printf( "\nWindow created" );

      // Get window surface
      gWindowSurface = SDL_GetWindowSurface( gWindow );

      if( gWindowSurface == NULL )
      {
        printf( "\nWindow's surface could not be created! SDL Error: \"%s\"", SDL_GetError() );
        success = false;
        PressEnter();
      }
      else
      {
        printf( "\nWindow's surface created" );
      }

    } // Window created

  } // SDL initialised

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

  // Load stretching surface
  gStretchedSurface = loadSurface_Optimised( Img );

  if( gStretchedSurface == NULL )
  {
    printf( "\nFailed to load stretching image \"%s\"!", Img.c_str() );
    success = false;
    PressEnter();
  }
  else
  {
    printf( "\nOptimised surface \"%s\" loaded, ready to be stretched", Img.c_str() );
  }

  return success;
}


/**
 * @brief Frees media and shuts down SDL
 **/
static void close(void)
{
  // Free loaded image
  SDL_FreeSurface( gStretchedSurface );
  gStretchedSurface = NULL;

  // Destroy window
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  // Quit SDL subsystems
  SDL_Quit();
}


/**
 * @brief Loads individual image. Optimised version (converts image to 32 bits).
 *
 * @param path The path to the image
 * @return SDL_Surface* Pointer to the loaded image
 **/
static SDL_Surface* loadSurface_Optimised( const std::string& path )
{
  // The final optimized image
  SDL_Surface* optimizedSurface = NULL;

  // Load image at specified path
  SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );

  if( loadedSurface == NULL )
  {
    printf( "\nUnable to load image \"%s\"! SDL Error: \"%s\"", path.c_str(), SDL_GetError() );
    PressEnter();
  }
  else
  {
    printf( "\nImage \"%s\" loaded", path.c_str() );

    // Convert surface to screen format
    optimizedSurface = SDL_ConvertSurface( loadedSurface, gWindowSurface->format, 0 );

    if( optimizedSurface == NULL )
    {
      printf( "\nUnable to optimize image \"%s\"! SDL Error: \"%s\"\n", path.c_str(), SDL_GetError() );
      PressEnter();
    }
    else
    {
      printf("\nSurface \"%s\" converted to screen format", path.c_str());
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
    printf( "\nFailed to initialize!" );
    PressEnter();
  }
  else
  {
    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!" );
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

        // Apply the image stretched
        SDL_Rect stretchRect;
        stretchRect.x = 0;
        stretchRect.y = 0;
        stretchRect.w = WINDOW_W;
        stretchRect.h = WINDOW_H;
        SDL_BlitScaled( gStretchedSurface, NULL, gWindowSurface, &stretchRect );

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