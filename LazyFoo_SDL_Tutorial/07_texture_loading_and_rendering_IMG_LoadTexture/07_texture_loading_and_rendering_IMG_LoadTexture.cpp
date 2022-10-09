/**
 * @file 07_texture_loading_and_rendering_IMG_LoadTexture.cpp
 *
 * @brief Come il precedente, ma con l'uso di "IMG_LoadTexture" anziché "loadSurface".
 *
 * main
 * |
 * |----init
 * |    |
 * |    |----SDL_Init               (inizializzazione SDL)
 * |    |----SDL_SetHint            (abilita linear texture filtering)
 * |    |----SDL_CreateWindow       (crea la finestra principale del programma)
 * |    |----SDL_CreateRenderer     (renderizza le immagini nella finestra)
 * |    |----SDL_SetRenderDrawColor (imposta il colore della renderizzazione)
 * |    |----IMG_Init               (inizializzazione SDL_image)
 * |
 * |----loadMedia
 * |    |
 * |    |----loadTexture
 * |         |
 * |         |----IMG_LoadTexture
 * |
 * |----SDL_PollEvent     (gestisce l'ultimo evento nella coda degli eventi)
 * |----SDL_RenderClear   (riempie lo schermo del colore impostato tramite SDL_SetRenderDrawColor)
 * |----SDL_RenderCopy    (predispone la texture ad essere renderizzata)
 * |----SDL_RenderPresent (l'alternativa a SDL_UpdateWindowSurface quando si usano le texture)
 * |----close

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
static constexpr int         WINDOW_W = 640;
static constexpr int         WINDOW_H = 480;
static const     std::string TexturePath("texture.png");


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void PressEnter(void);

static SDL_Texture* loadTexture( std::string path );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer
static SDL_Texture*  gTexture  = NULL; // Currently displayed texture


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

/**
 * @brief Starts up SDL and creates window
 *
 * @return bool
 **/
static bool init(void)
{
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "\nSDL could not initialize! SDL Error: \"%s\"\n", SDL_GetError() );
    success = false;
  }
  else
  {
    printf( "\nSDL initialised" );

    // Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
      printf( "\nWarning: Linear texture filtering not enabled!" );
      success = false;
    }
    else
    {
      printf( "\nLinear texture filtering enabled" );
    }

    // Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nWindow created" );

      // Create renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );

      if( gRenderer == NULL )
      {
        printf( "\nRenderer could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
        success = false;
      }
      else
      {
        printf( "\nRenderer created" );

        // Initialize renderer color
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

        // Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;

        if( !( IMG_Init( imgFlags ) & imgFlags ) )
        {
          printf( "\nSDL_image could not initialize! SDL_image Error: \"%s\"\n", IMG_GetError() );
          success = false;
        }
        else
        {
          printf( "\nSDL_image initialised" );
        }

      } // Renderer created

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

  // Load PNG texture
  gTexture = loadTexture( TexturePath );

  if( gTexture == NULL )
  {
    printf( "\nFailed to load texture image \"%s\"!", TexturePath.c_str() );
    success = false;
  }
  else
  {
    printf( "\nTexture image \"%s\" loaded", TexturePath.c_str() );
  }

  return success;
}


/**
 * @brief Frees media and shuts down SDL
 **/
static void close(void)
{
  // Free loaded image
  SDL_DestroyTexture( gTexture );
  gTexture = NULL;

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow   = NULL;
  gRenderer = NULL;

  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}


/**
 * @brief Loads individual image as texture. Versione modificata rispetto all'altro esempio 07. Fa
 * uso di "IMG_LoadTexture", che risparmia di dover creare una texture temporanea per poi
 * eliminarla.
 *
 * @param path The path to the image.
 * @return SDL_Surface* Pointer to the loaded image.
 **/
static SDL_Texture* loadTexture( std::string path )
{
  SDL_Texture* newTexture = NULL;

  newTexture = IMG_LoadTexture( gRenderer, path.c_str() );

  if( newTexture == NULL )
  {
    printf( "\nUnable to create texture from \"%s\"! SDL Error: \"%s\"\n", path.c_str(), SDL_GetError() );
  }
  else
  {
    printf( "\nTexture created from \"%s\"", path.c_str() );
  }

  return newTexture;
}


static void PressEnter(void)
{
  int UserChoice = '\0';

  printf("\nPress ENTER to continue...");

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
  printf("\nProgram started with %d additional arguments.", argc - 1); // Il primo argomento è il nome dell'eseguibile

  for (int i = 1; i != argc; ++i)
  {
    printf("\nArgument #%d: %s\n", i, args[i]);
  }

  // Start up SDL and create window
  if( !init() )
  {
    printf( "\nFailed to initialize!\n" );
    HasProgramSucceeded = false;
  }
  else
  {
    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!\n" );
      HasProgramSucceeded = false;
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
          else
          {;} // Wait for events
        }

        // Clear screen
        SDL_RenderClear( gRenderer );

        // Render texture to screen
        SDL_RenderCopy( gRenderer, gTexture, NULL, NULL );

        // Update screen
        SDL_RenderPresent( gRenderer );
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