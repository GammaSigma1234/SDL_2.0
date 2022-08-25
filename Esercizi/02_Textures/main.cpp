/**
 * @file main.cpp
 *
 * @brief
 *
 * Anche il formato BMP funziona
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>
#include <string>


/**************************************************************************************************
* Private constants and types
***************************************************************************************************/

/**
 * @brief The RGB components for each colour
 **/
enum class Colour_Components
{
  RED,
  GREEN,
  BLUE,
  ALPHA,

  HOW_MANY
};

static constexpr int WINDOW_W  =  1024; // Window's width
static constexpr int WINDOW_H  =  768;  // Window's height
static constexpr int FIRST_ONE = -1;    // index of the first initialisable rendering driver supporting the requested flags

// Colore bianco (R, G, B, A)
static constexpr size_t ArraySize = static_cast<size_t>(Colour_Components::HOW_MANY);
static constexpr Uint8  White[ArraySize]
                            { static_cast<Uint8>(0xFF),
                              static_cast<Uint8>(0xFF),
                              static_cast<Uint8>(0xFF),
                              static_cast<Uint8>(0xFF)  };

// Colore nero
static constexpr int BLACK_R = 0x00; // Amount of red   needed to compose black
static constexpr int BLACK_G = 0x00; // Amount of green needed to compose black
static constexpr int BLACK_B = 0x00; // Amount of blue  needed to compose black
static constexpr int BLACK_A = 0xFF; // Alpha component

// Colore rosso
static constexpr int RED_R = 0xFF; // Amount of red   needed to compose red
static constexpr int RED_G = 0x00; // Amount of green needed to compose red
static constexpr int RED_B = 0x00; // Amount of blue  needed to compose red
static constexpr int RED_A = 0xFF; // Alpha component

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan
static constexpr int CYAN_A = 0xFF; // Alpha component

// Image path
std::string PNGImagePath("SDL_RedDot_CyanBG.png");


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   g_Window   = NULL; // The window we'll be rendering to
static SDL_Renderer* g_Renderer = NULL; // The window renderer
static SDL_Texture*  g_Texture  = NULL; // Currently displayed texture


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

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

  /* Start debugging console */

  printf("\n*** Debugging console ***\n");
  printf("\nProgram started with %d additional arguments.", argc - 1); // Il primo argomento è il nome dell'eseguibile

  for (int i = 1; i != argc; ++i)
  {
    printf("\nArgument #%d: %s\n", i, args[i]);
  }

  /* Initialise SDL */

  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "\nSDL could not initialise! SDL_Error: \"%s\"\n", SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: SDL initialised" );
  }

  /* Set texture filtering to linear */

  if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" ) )
  {
    printf( "\nWarning: Linear texture filtering not enabled!" );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: linear texture filtering enabled" );
  }

  /* Create window */

  g_Window = SDL_CreateWindow(  "Single Texture",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                WINDOW_W               , WINDOW_H,
                                SDL_WINDOW_SHOWN );

  if( g_Window == NULL )
  {
    printf( "\nWindow could not be created! SDL_Error: %s\n", SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: window created" );
  }

  /* Create renderer for window */

  g_Renderer = SDL_CreateRenderer( g_Window, FIRST_ONE, SDL_RENDERER_ACCELERATED );

  if( g_Renderer == NULL )
  {
    printf( "\nRenderer could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: renderer created" );
  }

  /* Initialise renderer colour */

  Uint8 RedIdx   = static_cast<Uint8>(Colour_Components::RED);
  Uint8 GreenIdx = static_cast<Uint8>(Colour_Components::GREEN);
  Uint8 BlueIdx  = static_cast<Uint8>(Colour_Components::BLUE);
  Uint8 AlphaIdx = static_cast<Uint8>(Colour_Components::ALPHA);

  if ( SDL_SetRenderDrawColor( g_Renderer, White[RedIdx], White[GreenIdx], White[BlueIdx], White[AlphaIdx] ) != 0 )
  {
    printf( "\nCould not set render drawing colour! SDL Error: \"%s\"\n", SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: render drawing colour set" );
  }

  /* Initialise PNG loading */

  int imgFlags = static_cast<int>(IMG_INIT_PNG);

  if( !( IMG_Init( imgFlags ) & imgFlags ) )
  {
    printf( "\nSDL_image could not initialise! SDL_image Error: \"%s\"\n", IMG_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: SDL_image initialised" );
  }

  /* Load texture from PNG */

  SDL_Surface* loadedSurface = IMG_Load( PNGImagePath.c_str() );

  if( loadedSurface == NULL )
  {
    printf( "\nUnable to load image \"%s\"! SDL_image Error: \"%s\"", PNGImagePath.c_str(), IMG_GetError() );
  }
  else
  {
    printf( "\nImage \"%s\" loaded", PNGImagePath.c_str() );
  }

  // Colour key image
  if ( SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0x00, 0xFF, 0xFF ) ) != 0 )
  {
    printf("\nFailed to set colour key for surface \"%s\"! SDL error: \"%s\"", PNGImagePath.c_str(), SDL_GetError());
  }
  else
  {
    printf("\nColour key set for surface \"%s\"", PNGImagePath.c_str());
  }

  // Create texture from surface pixels
  g_Texture = SDL_CreateTextureFromSurface( g_Renderer, loadedSurface );

  if( g_Texture == NULL )
  {
    printf( "Unable to create texture from \"%s\"! SDL Error: \"%s\"\n", PNGImagePath.c_str(), SDL_GetError() );
  }
  else
  {
    printf( "\nTexture created from \"%s\"", PNGImagePath.c_str() );
  }


  // g_Texture = IMG_LoadTexture( g_Renderer, PNGImagePath.c_str() );

  // if( g_Texture == NULL )
  // {
  //   printf( "\nUnable to create texture from \"%s\"! SDL Error: \"%s\"\n", PNGImagePath.c_str(), SDL_GetError() );
  //   HasProgramSucceeded = false;
  // }
  // else
  // {
  //   printf( "\nOK: texture created from \"%s\"", PNGImagePath.c_str() );
  // }

  // SDL_SetTextureBlendMode( g_Texture, SDL_BLENDMODE_BLEND );
  // SDL_SetTexture

  /* Preparing for main loop */

  bool Quit = false; // Main loop flag

  SDL_Event EventHandler;

  // While application is running
  while( !Quit )
  {
    // Handle events on queue
    while( SDL_PollEvent( &EventHandler ) != 0 )
    {
      // User requests Quit
      if( EventHandler.type == SDL_QUIT )
      {
        Quit = true;
      }
      else
      { /* Wait for events */ }
    }

    // Clear screen
    SDL_RenderClear( g_Renderer );

    // Render texture to screen
    SDL_Rect Dest{0, 0, 400, 400};
    SDL_RenderCopy( g_Renderer, g_Texture, NULL, &Dest );

    // Update screen
    SDL_RenderPresent( g_Renderer );
  }

  /* Free resources and close SDL */

  SDL_DestroyTexture( g_Texture );
  SDL_DestroyRenderer( g_Renderer );
  SDL_DestroyWindow( g_Window );
  g_Texture  = NULL;
  g_Renderer = NULL;
  g_Window   = NULL;

  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();

  /* Integrity check */

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