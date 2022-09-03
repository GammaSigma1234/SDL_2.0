/**
 * @file 08_geometry_rendering.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/08_geometry_rendering/index.php
 *
 * @brief Along with the new texturing API, SDL has new primitive rendering calls as part of its
 * rendering API. So, if you need some basic shapes rendered and you don't want to create additional
 * graphics for them, SDL can save you the effort.
 *
 * In our media loading function, we load no media. SDL's primitive rendering allows you to render
 * shapes without loading special graphics.
 *
 * At the top of the main loop we handle the quit event as before, and clear the screen. Also notice
 * that we're setting the clearing color to white with "SDL_SetRenderDrawColor" every frame as
 * opposed to setting it once in the initialization function. We'll cover why this happens when we
 * get to the end of the main loop.
 *
 * The first primitive we're going to draw is a fill rectangle, which is a solid rectangle.
 *
 * First we define a rectangle to define the area we want to fill with color. If you have never seen
 * a struct initialized like this, know that the member variables that make up an SDL rect are x, y,
 * w, and h for the x position, y position, width, and height respectively. You can initialize a
 * struct by giving it an array of variables in the order they are in the struct. Here we're setting
 * the rectangle one quarter of the screen width in the x direction, one quarter of the screen
 * height in the y direction, and with half the screen's width/height.
 *
 * After defining the rectangle area, we set the rendering color with SDL_SetRenderDrawColor. This
 * function takes in the renderer for the window we're using and the RGBA values for the color we
 * want to render with. R is the red component, G is green, B is blue, and A is alpha. Alpha
 * controls how opaque something is and we'll cover that in the transparency tutorial. These values
 * go from 0 to 255 (or FF hex as you see above) and are mixed together to create all the colors you
 * see on your screen. This call to SDL_SetRenderDrawColor sets the drawing color to opaque red.
 *
 * After the rectangle and color have been set, SDL_RenderFillRect is called to draw the rectangle.
 *
 * You can also draw a rectangle outline with an empty center using SDL_RenderDrawRect. As you can
 * see it pretty much works the same as a solid filled rectangle as this piece of code is almost the
 * same as the one above it. The major difference is that this rectangle is 2 thirds of the screen
 * in size and that the color we're using here is green.
 *
 * Also if you mess with the position of the rectangle, you may notice something strange about the y
 * coordinate. Making the y coordinate larger makes it go down and making the y coordinate smaller
 * makes it go up. This is because SDL and many 2D rendering APIs use a different coordinate system.
 *
 * The last bit of geometry we render is a sequence of dots using SDL_RenderDrawPoint. We're just
 * taking a set of points and drawing them from top to bottom. Again notice the y coordinate and the
 * inverted y axis. After we're finished drawing all our geometry, we update the screen.
 *
 * Notice the call to SDL_SetRenderDrawColor. We're using 255 red and 255 green which combine
 * together to make yellow. Remember that call to SDL_SetRenderDrawColor at the top of the loop? If
 * that wasn't there, the screen would be cleared with whatever color was last set with
 * SDL_SetRenderDrawColor, resulting in a yellow background in this case.
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
 * |    |----SDL_CreateRenderer     (motore di renderizzazione per le immagini nella finestra)
 * |    |----SDL_SetRenderDrawColor (imposta il colore della renderizzazione)
 * |    |----IMG_Init               (inizializzazione SDL_image)
 * |
 * |----loadMedia (vuoto: in questo esempio disegniamo delle forme basilari con il renderer)
 * |
 * |----SDL_SetRenderDrawColor     (a ogni nuova renderizzazione bisogna reimpostare il colore)
 * |----SDL_RenderClear            (ripulisce la finestra  con il colore impostato da SDL_SetRenderDrawColor)
 * |----SDL_RenderFillRect         (riempie un rettangolo  con il colore impostato da SDL_SetRenderDrawColor)
 * |----SDL_RenderDrawRect         (contorna un rettangolo con il colore impostato da SDL_SetRenderDrawColor)
 * |----SDL_RenderDrawLine         (disegna una linea      con il colore impostato da SDL_SetRenderDrawColor)
 * |----SDL_RenderDrawPoint        (disegna un punto       con il colore impostato da SDL_SetRenderDrawColor)
 * |----close
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022) and may not be
 * redistributed without written permission.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, standard I/O, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <cmath>
#include <string>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

// Screen dimension constants
static constexpr int SCREEN_WIDTH  = 640;
static constexpr int SCREEN_HEIGHT = 480;

// Colore bianco
static constexpr int WHITE_R(0xFF);
static constexpr int WHITE_G(0xFF);
static constexpr int WHITE_B(0xFF);
static constexpr int WHITE_A(0xFF);

// Colore rosso
static constexpr int RED_R(0xFF);
static constexpr int RED_G(0x00);
static constexpr int RED_B(0x00);
static constexpr int RED_A(0xFF);

// Colore verde
static constexpr int GREEN_R(0x00);
static constexpr int GREEN_G(0xFF);
static constexpr int GREEN_B(0x00);
static constexpr int GREEN_A(0xFF);

// Colore giallo
static constexpr int YELLOW_R(0xFF);
static constexpr int YELLOW_G(0xFF);
static constexpr int YELLOW_B(0x00);
static constexpr int YELLOW_A(0xFF);

// Colore blu
static constexpr int BLUE_R(0x00);
static constexpr int BLUE_G(0x00);
static constexpr int BLUE_B(0xFF);
static constexpr int BLUE_A(0xFF);

/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void PressEnter(void);

// static SDL_Texture* loadTexture( std::string path );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer


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

  // Nothing to load
  return success;
}


/**
 * @brief Frees media and shuts down SDL
 **/
static void close(void)
{
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
 * @param path The path to the image
 * @return SDL_Surface* Pointer to the loaded image
 **/
// static SDL_Texture* loadTexture( std::string path )
// {
//   // The final texture
//   SDL_Texture* newTexture = NULL;

//   // Load image at specified path
//   SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

//   if( loadedSurface == NULL )
//   {
//     printf( "\nUnable to load image \"%s\"! SDL_image Error: \"%s\"\n", path.c_str(), IMG_GetError() );
//   }
//   else
//   {
//     printf( "\nImage \"%s\" loaded", path.c_str() );

//     // Create texture from surface pixels
//     newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

//     if( newTexture == NULL )
//     {
//       printf( "\nUnable to create texture from \"%s\"! SDL Error: \"%s\"\n", path.c_str(), SDL_GetError() );
//     }
//     else
//     {
//       printf( "\nTexture created from \"%s\"", path.c_str() );
//     }

//     // Get rid of old loaded surface
//     SDL_FreeSurface( loadedSurface );
//   }

//   return newTexture;
// }


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
    printf( "\nAll systems initialised" );

    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!\n" );
      HasProgramSucceeded = false;
    }
    else
    {
      printf( "\nAll media loaded" );

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

        // We're setting the clearing color to white at every frame as opposed to setting it once in
        // the initialization function
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );

        // Clear screen
        SDL_RenderClear( gRenderer );

        // Render red filled quad
        SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
        SDL_SetRenderDrawColor( gRenderer, RED_R, RED_G, RED_B, RED_A );
        SDL_RenderFillRect( gRenderer, &fillRect );

        // Render green outlined quad
        SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
        SDL_SetRenderDrawColor( gRenderer, GREEN_R, GREEN_G, GREEN_B, GREEN_A );
        SDL_RenderDrawRect( gRenderer, &outlineRect );

        // Draw blue horizontal line
        SDL_SetRenderDrawColor( gRenderer, BLUE_R, BLUE_G, BLUE_B, BLUE_A );
        SDL_RenderDrawLine( gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 );

        // Draw vertical line of yellow dots
        SDL_SetRenderDrawColor( gRenderer, YELLOW_R, YELLOW_G, YELLOW_B, YELLOW_A );
        for( int i = 0; i != SCREEN_HEIGHT; i += 4 )
        {
          SDL_RenderDrawPoint( gRenderer, SCREEN_WIDTH / 2, i );
        }

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