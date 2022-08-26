/**
 * @file main.cpp
 *
 * @brief Uso di textures con e senza canale alpha, e geometria di base.
 *
 * Usato un header file esterno per definire i colori. I "#define" al posto dei "constexpr" sono una
 * buona scelta?
 *
 * - Inizializzazione SDL con "SDL_init"
 * - Texture filtering lineare con "SDL_SetHint"
 * - Creazione della finestra principale con "SDL_CreateWindow"
 * - Creazione del renderer con "SDL_CreateRenderer"
 * - Impostazione colore base del renderer con "SDL_SetRenderDrawColor"
 * - Inizializzazione SDL_image con "IMG_Init"
 * - Creazione textures (una senza canale alpha e una con) mediante caricamento file immagine con "IMG_LoadTexture"
 * - Creazione main loop, e analisi eventi con "SDL_PollEvent"
 * - Pulizia finestra con "SDL_RenderClear"
 * - Rendering delle texture mediante "SDL_RenderCopy"
 * - Disegno di forme geometriche con "FillRect", "DrawRect", "DrawLine" e "DrawPoint"
 * - Aggiornamento della finestra con "SDL_RenderPresent"
 * - Liberazione risorse mediante metodi "SDL_Destroy"
 * - Liberazione sottosistemi mediante metodi "_Quit"
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>
#include <string>
#include "colours.hpp"


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


// Image path
std::string PNGImageWithCyanBGPath("SDL_RedDot_CyanBG.png");
std::string PNGImageWithAlphaBGPath("SDL_RedDot_AlphaBG.png");


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   g_Window   = NULL; // The window we'll be rendering to
static SDL_Renderer* g_Renderer = NULL; // The window renderer

static SDL_Texture*  g_Texture_CyanBG  = NULL;
static SDL_Texture*  g_Texture_AlphaBG = NULL;

static int g_Texture_CyanBG_W  = 0;
static int g_Texture_CyanBG_H  = 0;
static int g_Texture_AlphaBG_W = 0;
static int g_Texture_AlphaBG_H = 0;


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
  printf("\nProgram \"%s\" started with %d additional arguments.", args[0], argc - 1); // Il primo argomento è il nome dell'eseguibile

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

  if ( SDL_SetRenderDrawColor( g_Renderer, YELLOW_R, YELLOW_G, YELLOW_B, YELLOW_A ) != 0 )
  {
    printf( "\nCould not set render drawing colour! SDL Error: \"%s\"\n", SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: render drawing colour set" );
  }

  /* Initialise SDL_image */

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

  /* Load textures from PNG */

  g_Texture_CyanBG  = IMG_LoadTexture( g_Renderer, PNGImageWithCyanBGPath.c_str() );
  g_Texture_AlphaBG = IMG_LoadTexture( g_Renderer, PNGImageWithAlphaBGPath.c_str() );

  if( g_Texture_CyanBG == NULL )
  {
    printf( "\nUnable to create texture from \"%s\"! SDL Error: \"%s\"\n", PNGImageWithCyanBGPath.c_str(), SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else if (  g_Texture_AlphaBG == NULL )
  {
    printf( "\nUnable to create texture from \"%s\"! SDL Error: \"%s\"\n", PNGImageWithAlphaBGPath.c_str(), SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: texture created from \"%s\"", PNGImageWithCyanBGPath.c_str() );
    printf( "\nOK: texture created from \"%s\"", PNGImageWithAlphaBGPath.c_str() );
  }

  SDL_QueryTexture( g_Texture_AlphaBG, NULL, NULL, &g_Texture_AlphaBG_W, &g_Texture_AlphaBG_H );
  SDL_QueryTexture( g_Texture_CyanBG , NULL, NULL, &g_Texture_CyanBG_W , &g_Texture_CyanBG_H  );

  // SDL_SetTextureBlendMode( g_Texture, SDL_BLENDMODE_BLEND );

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
    SDL_SetRenderDrawColor( g_Renderer, YELLOW_R, YELLOW_G, YELLOW_B, YELLOW_A );
    SDL_RenderClear( g_Renderer );

    // Render texture to screen
    SDL_Rect CyanDest {0                             , 0, g_Texture_CyanBG_H , g_Texture_CyanBG_W};
    SDL_Rect AlphaDest{WINDOW_W - g_Texture_AlphaBG_W, 0, g_Texture_AlphaBG_H, g_Texture_AlphaBG_W};

    SDL_RenderCopy( g_Renderer, g_Texture_CyanBG , NULL, &CyanDest );
    SDL_RenderCopy( g_Renderer, g_Texture_AlphaBG, NULL, &AlphaDest );

    // Render orange-filled quad
    SDL_Rect fillRect = { WINDOW_W / 4, WINDOW_H / 4, WINDOW_W / 2, WINDOW_H / 2 };
    SDL_SetRenderDrawColor( g_Renderer, ORANGE_R, ORANGE_G, ORANGE_B, ORANGE_A );
    SDL_RenderFillRect( g_Renderer, &fillRect );

    // Render green-outlined quad
    SDL_Rect outlineRect = { WINDOW_W / 6, WINDOW_H / 6, WINDOW_W * 2 / 3, WINDOW_H * 2 / 3 };
    SDL_SetRenderDrawColor( g_Renderer, GREEN_R, GREEN_G, GREEN_B, GREEN_A );
    SDL_RenderDrawRect( g_Renderer, &outlineRect );

    // Draw blue horizontal line
    SDL_SetRenderDrawColor( g_Renderer, BLUE_R, BLUE_G, BLUE_B, BLUE_A );
    SDL_RenderDrawLine( g_Renderer, 0, WINDOW_H / 2, WINDOW_W, WINDOW_H / 2 );

    // Draw vertical line of fuchsia dots
    SDL_SetRenderDrawColor( g_Renderer, FUCHSIA_R, FUCHSIA_G, FUCHSIA_B, FUCHSIA_A );
    for( int i = 0; i != WINDOW_H; i += 4 )
    {
      SDL_RenderDrawPoint( g_Renderer, WINDOW_W / 2, i );
    }

    // Update screen
    SDL_RenderPresent( g_Renderer );
  }

  /* Free resources and close SDL */

  SDL_DestroyTexture( g_Texture_CyanBG );
  SDL_DestroyTexture( g_Texture_AlphaBG );
  SDL_DestroyRenderer( g_Renderer );
  SDL_DestroyWindow( g_Window );
  g_Texture_CyanBG  = NULL;
  g_Texture_AlphaBG = NULL;
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