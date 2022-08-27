/**
 * @file main.cpp
 *
 * @brief Apparentemente, il canale alpha e l'alpha modulation di una texture sono due concetti
 * diversi. Il canale alpha è un insieme di pixel trasparenti, mentre l'alpha modulation è la
 * trasparenza globale di una texture?
 *
 * In questo esempio vengono caricate due textures: una senza e una con canale alpha. Modificare
 * l'alpha modulation di entrambe pare non avere alcuna conseguenza sul canale alpha.
 *
 * - Inizializzazione SDL con "SDL_init"
 * - Texture filtering lineare con "SDL_SetHint"
 * - Creazione della finestra principale con "SDL_CreateWindow"
 * - Creazione del renderer con "SDL_CreateRenderer"
 * - Impostazione colore base del renderer con "SDL_SetRenderDrawColor"
 * - Inizializzazione SDL_image con "IMG_Init"
 * - Inizializzazione SDL_ttf   con "TTF_Init"
 * - Apertura font globale con "TTF_OpenFont"
 * - Creazione textures (una senza canale alpha e una con) mediante caricamento file immagine con "IMG_LoadTexture"
 * - (Testo) Preparazione superficie temporanea con "TTF_RenderText_Blended"
 * - (Testo) Creazione textures dalla superficie temporanea con "SDL_CreateTextureFromSurface"
 * - Dimensioni delle textures mediante "SDL_QueryTexture"
 * - Impostazione blending mode (per poter modificare il blending) con "SDL_SetTextureBlendMode"
 * - Creazione main loop, e analisi eventi con "SDL_PollEvent"
 * - Pulizia finestra con "SDL_RenderClear"
 * - Rendering delle texture mediante "SDL_RenderCopy"
 * - Impostazione del valore globale di alpha della texture con "SDL_SetTextureAlphaMod"
 * - Aggiornamento della finestra con "SDL_RenderPresent"
 * - Liberazione risorse mediante metodi "SDL_Destroy"
 * - Liberazione sottosistemi mediante metodi "_Quit"
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
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

static constexpr Uint8 INCREMENT = static_cast<Uint8>(8);
static constexpr Uint8 MAX_ALPHA = static_cast<Uint8>(255);

// Paths
static const std::string PNGImageWithCyanBGPath ("SDL_RedDot_CyanBG.png");
static const std::string PNGImageWithAlphaBGPath("SDL_RedDot_AlphaBG.png");
static const std::string FontPath               ("RachelBrown.ttf");

// Prompts
static const std::string Prompt_1("\nPress UP or DOWN arrow keys");
static const std::string Prompt_2("\nPress any other key to reset");


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   g_Window   (NULL); // The window we'll be rendering to
static SDL_Renderer* g_Renderer (NULL); // The window renderer

static SDL_Texture*  g_RedDotCyanBG_Tex  (NULL);
static SDL_Texture*  g_RedDotAlphaBG_Tex (NULL);
static SDL_Texture*  g_Text1_Tex         (NULL);
static SDL_Texture*  g_Text2_Tex         (NULL);

static TTF_Font*     g_Font (NULL); // Globally used font

static int g_RedDotCyanBG_W  (0);
static int g_RedDotCyanBG_H  (0);
static int g_RedDotAlphaBG_W (0);
static int g_RedDotAlphaBG_H (0);

static int g_Text1_Tex_W (0);
static int g_Text1_Tex_H (0);
static int g_Text2_Tex_W (0);
static int g_Text2_Tex_H (0);


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

  /* Initialize SDL_ttf */

  if( TTF_Init() == -1 )
  {
    printf( "\nSDL_ttf could not initialise! SDL_ttf Error: \"%s\"", TTF_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: SDL_ttf initialised" );
  }

  g_Font = TTF_OpenFont( FontPath.c_str(), 28 );

  if( g_Font == NULL )
  {
    printf( "\nFailed to load \"%s\" font! SDL_ttf Error: \"%s\"\n", FontPath.c_str(), TTF_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: \"%s\" font loaded", FontPath.c_str() );
  }

  /* Load textures from PNG */

  g_RedDotCyanBG_Tex  = IMG_LoadTexture( g_Renderer, PNGImageWithCyanBGPath.c_str() );
  g_RedDotAlphaBG_Tex = IMG_LoadTexture( g_Renderer, PNGImageWithAlphaBGPath.c_str() );

  if( g_RedDotCyanBG_Tex == NULL )
  {
    printf( "\nUnable to create texture from \"%s\"! SDL Error: \"%s\"\n", PNGImageWithCyanBGPath.c_str(), SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else if (  g_RedDotAlphaBG_Tex == NULL )
  {
    printf( "\nUnable to create texture from \"%s\"! SDL Error: \"%s\"\n", PNGImageWithAlphaBGPath.c_str(), SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: texture created from \"%s\"", PNGImageWithCyanBGPath.c_str() );
    printf( "\nOK: texture created from \"%s\"", PNGImageWithAlphaBGPath.c_str() );
  }

  /* Caricamento texture da font */

  SDL_Color    TextColour_FG{BLACK_R, BLACK_G, BLACK_B, BLACK_A};
  SDL_Surface* TempSurface_1 = TTF_RenderText_Blended( g_Font, Prompt_1.c_str(), TextColour_FG );
  SDL_Surface* TempSurface_2 = TTF_RenderText_Blended( g_Font, Prompt_2.c_str(), TextColour_FG );

  if( TempSurface_1 == NULL || TempSurface_2 == NULL )
  {
    printf( "\nUnable to render text surfaces! SDL_ttf Error: \"%s\"", TTF_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: text surfaces loaded" );
  }

  // Create texture from surface pixels
  g_Text1_Tex = SDL_CreateTextureFromSurface( g_Renderer, TempSurface_1 );
  g_Text2_Tex = SDL_CreateTextureFromSurface( g_Renderer, TempSurface_2 );

  if( g_Text1_Tex == NULL || g_Text2_Tex == NULL )
  {
    printf( "\nUnable to create textures from rendered text! SDL Error: \"%s\"", SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: text textures created" );
  }

  // Get rid of old surfaces
  SDL_FreeSurface( TempSurface_1 );
  SDL_FreeSurface( TempSurface_2 );
  TempSurface_1 = NULL;
  TempSurface_2 = NULL;

  /* Ottenimento delle dimensioni delle textures */

  SDL_QueryTexture( g_RedDotAlphaBG_Tex, NULL, NULL, &g_RedDotAlphaBG_W, &g_RedDotAlphaBG_H );
  SDL_QueryTexture( g_RedDotCyanBG_Tex , NULL, NULL, &g_RedDotCyanBG_W , &g_RedDotCyanBG_H  );
  SDL_QueryTexture( g_Text1_Tex        , NULL, NULL, &g_Text1_Tex_W    , &g_Text1_Tex_H     );
  SDL_QueryTexture( g_Text2_Tex        , NULL, NULL, &g_Text2_Tex_W    , &g_Text2_Tex_H     );

  /* Impostazione del blending mode delle textures */

  SDL_SetTextureBlendMode( g_RedDotCyanBG_Tex,  SDL_BLENDMODE_BLEND );
  SDL_SetTextureBlendMode( g_RedDotAlphaBG_Tex, SDL_BLENDMODE_BLEND );

  /* Preparing for main loop */

  bool Quit = false; // Main loop flag

  SDL_Event EventHandler;

  Uint8 Alpha = static_cast<Uint8>(0xFF);

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
      else if( EventHandler.type == SDL_KEYDOWN )
      {
        switch ( EventHandler.key.keysym.sym )
        {
          case SDLK_UP:

            if ( Alpha + INCREMENT > MAX_ALPHA )
            {
              Alpha = MAX_ALPHA;
            }
            else
            {
              Alpha = static_cast<Uint8>( Alpha + INCREMENT );
            }
            break;

          case SDLK_DOWN:

            if ( Alpha - INCREMENT < 0 )
            {
              Alpha = 0;
            }
            else
            {
              Alpha = static_cast<Uint8>( Alpha - INCREMENT );
            }
            break;

          default:

            Alpha = MAX_ALPHA;
            break;
        }
      }
      else
      { /* Wait for events */ }
    }

    // Clear screen
    SDL_SetRenderDrawColor( g_Renderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
    SDL_RenderClear( g_Renderer );

    // Render textures to screen
    SDL_Rect CyanDest {   0                             , 0            , g_RedDotCyanBG_W , g_RedDotCyanBG_H  };
    SDL_Rect AlphaDest{   WINDOW_W - g_RedDotAlphaBG_W  , 0            , g_RedDotAlphaBG_W, g_RedDotAlphaBG_H };
    SDL_Rect Text1Dest{ ( WINDOW_W - g_Text1_Tex_W ) / 2, 0            , g_Text1_Tex_W    , g_Text1_Tex_H     };
    SDL_Rect Text2Dest{ ( WINDOW_W - g_Text2_Tex_W ) / 2, g_Text1_Tex_H, g_Text2_Tex_W    , g_Text2_Tex_H     };

    SDL_RenderCopy( g_Renderer, g_RedDotCyanBG_Tex , NULL, &CyanDest  );
    SDL_RenderCopy( g_Renderer, g_RedDotAlphaBG_Tex, NULL, &AlphaDest );
    SDL_RenderCopy( g_Renderer, g_Text1_Tex        , NULL, &Text1Dest );
    SDL_RenderCopy( g_Renderer, g_Text2_Tex        , NULL, &Text2Dest );

    // Set alpha
    SDL_SetTextureAlphaMod( g_RedDotCyanBG_Tex , Alpha );
    SDL_SetTextureAlphaMod( g_RedDotAlphaBG_Tex, Alpha );

    // Update screen
    SDL_RenderPresent( g_Renderer );
  }

  /* Free resources and close SDL */

  SDL_DestroyTexture ( g_RedDotCyanBG_Tex );
  SDL_DestroyTexture ( g_RedDotAlphaBG_Tex );
  SDL_DestroyTexture ( g_Text1_Tex );
  SDL_DestroyTexture ( g_Text2_Tex );
  SDL_DestroyRenderer( g_Renderer );
  SDL_DestroyWindow  ( g_Window );
  g_RedDotCyanBG_Tex  = NULL;
  g_RedDotAlphaBG_Tex = NULL;
  g_Renderer          = NULL;
  g_Window            = NULL;

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