/**
 * @file 09_the_viewport.cpp
 *
 * @brief https:// lazyfoo.net/tutorials/SDL/09_the_viewport/index.php
 *
 * Sometimes you only want to render part of the screen for things like minimaps. Using the
 * viewport you can control where you render on the screen.
 *
 * After we clear the screen, it's time to get drawing. There are 3 regions we're going to draw a full
 * screen image to.
 *
 * First we're going to render the top left. This is as easy as creating a rectangle with half the
 * width/height as the screen, and passing this region to SDL_RenderSetViewport. Any rendering done
 * after that call (usando "SDL_RenderPresent"? NdGS) will render inside the region defined by the
 * given viewport. It will also use the coordinate system of the window it was created in, so the
 * bottom of the view we created will still be y = 480 even though it's only 240 pixels down from
 * the top.
 *
 * Then we define the top right area and draw to it. It's pretty much the same as before, only now
 * the x coordinate is half the screen over.
 *
 * Finally we render one more time in the bottom half of the screen. Again, the viewport will use
 * the same coordinate system as the window it is used in, so the image will appear squished since
 * the viewport is half the height.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, standard I/O, and strings
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
static const     std::string TexturePath("viewport.png");


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

  // Load texture
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
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        // Top left corner viewport
        SDL_Rect topLeftViewport;
        topLeftViewport.x = 0;
        topLeftViewport.y = 0;
        topLeftViewport.w = WINDOW_W / 2;
        topLeftViewport.h = WINDOW_H / 2;
        SDL_RenderSetViewport( gRenderer, &topLeftViewport );

        // Render texture to screen
        SDL_RenderCopy( gRenderer, gTexture, NULL, NULL );


        // Top right viewport
        SDL_Rect topRightViewport;
        topRightViewport.x = WINDOW_W / 2;
        topRightViewport.y = 0;
        topRightViewport.w = WINDOW_W / 2;
        topRightViewport.h = WINDOW_H / 2;
        SDL_RenderSetViewport( gRenderer, &topRightViewport );

        // Render texture to screen
        SDL_RenderCopy( gRenderer, gTexture, NULL, NULL );


        // Bottom viewport
        SDL_Rect bottomViewport;
        bottomViewport.x = 0;
        bottomViewport.y = WINDOW_H / 2;
        bottomViewport.w = WINDOW_W;
        bottomViewport.h = WINDOW_H / 2;
        SDL_RenderSetViewport( gRenderer, &bottomViewport );


        // Render texture to screen
        SDL_RenderCopy( gRenderer, gTexture, NULL, NULL );


        // Update screen
        SDL_RenderPresent( gRenderer );

      } // event loop

    } // media loaded

  } // SDL initialised

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