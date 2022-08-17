/**
 * @file 07_texture_loading_and_rendering.cpp
 *
 * @brief This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 *
 * Textures in SDL have their own data type, intuitively called an "SDL_Texture". When we deal with
 * SDL textures, you need an "SDL_Renderer" object to render it to the screen, which is why we
 * declare a global renderer named "gRenderer".
 *
 * We have a new image loading routine ("loadTexture") and a globally declared texture ("gTexture")
 * we're going to load.
 *
 * After we create our window, we have to create a renderer for our window, so we can render textures
 * on it. This is easily done with a call to "SDL_CreateRenderer".
 *
 * After creating the renderer, we want to initialize the rendering color using
 * "SDL_SetRenderDrawColor". This controls what color is used for various rendering operations.
 *
 * Our texture loading function looks largely the same as the earlier "loadSurface", only now
 * instead of converting the loaded surface to the display format, we create a texture from the
 * loaded surface using "SDL_CreateTextureFromSurface". As before, this function creates a new
 * texture from an existing surface, which means like before we have to free the loaded surface and
 * then return the loaded texture.
 *
 * Since texture loading is abstracted with our image loading function, the "loadMedia" function
 * works pretty much the same as before.
 *
 * In our clean up function, we have to remember to deallocate our textures using
 * "SDL_DestroyTexture".
 *
 * In the main loop after the event loop, we call "SDL_RenderClear". This function fills the screen
 * with the color that was last set with "SDL_SetRenderDrawColor".
 *
 * With the screen cleared, we render the texture with "SDL_RenderCopy". With the texture rendered, we
 * still have to update the screen, but since we're not using "SDL_Surfaces" to render, we can't use
 * "SDL_UpdateWindowSurface". Instead we have to use "SDL_RenderPresent".
 *
 * Now, there is a new API call called "IMG_LoadTexture". It's not in the official documentation as of
 * this writing, but you can find it in the SDL_image header files. It allows you to load a texture
 * without having to create a temporary surface. The reason I haven't gone back and updated the
 * tutorial to use this method is because I don't want to go back an update 40+ tutorials. Even
 * though it is undocumented, it works just fine.
 *
 * Sunto:
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
 * |         |----IMG_Load                      (carica immagine come surface, e non come texture)
 * |         |----SDL_CreateTextureFromSurface  (converte l'immagine a texture)
 * |         |----SDL_FreeSurface               (libera l'immagine, ora inutilizzata)
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
static constexpr int         SCREEN_WIDTH  = 640;
static constexpr int         SCREEN_HEIGHT = 480;
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

SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
SDL_Renderer* gRenderer = NULL; // The window renderer
SDL_Texture*  gTexture  = NULL; // Currently displayed texture


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
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

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
 * @brief Loads individual image as texture.
 *
 * @param path The path to the image.
 * @return SDL_Surface* Pointer to the loaded image.
 **/
static SDL_Texture* loadTexture( std::string path )
{
  // The final texture
  SDL_Texture* newTexture = NULL;

  // Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

  if( loadedSurface == NULL )
  {
    printf( "\nUnable to load image \"%s\"! SDL_image Error: \"%s\"\n", path.c_str(), IMG_GetError() );
  }
  else
  {
    printf( "\nImage \"%s\" loaded", path.c_str() );

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

    if( newTexture == NULL )
    {
      printf( "\nUnable to create texture from \"%s\"! SDL Error: \"%s\"\n", path.c_str(), SDL_GetError() );
    }
    else
    {
      printf( "\nTexture created from \"%s\"", path.c_str() );
    }

    // Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
  }

  return newTexture;
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