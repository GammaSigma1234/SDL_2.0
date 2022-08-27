/**
 * @file main.cpp
 *
 * @brief Renderizzazione del testo. Richiede di usare un font True Type (TTF).
 * Anziché ricaricare la texture ogni volta che si preme il tasto freccia UP o DOWN, si potrebbe in
 * alternativa preparare uno sprite sheet con i testi già pronti, e renderizzare solo il clip
 * contenente il testo d'interesse.
 *
 * - Inizializzazione SDL con "SDL_init"
 * - Texture filtering lineare con "SDL_SetHint"
 * - Creazione della finestra principale con "SDL_CreateWindow"
 * - Creazione del renderer con "SDL_CreateRenderer"
 * - Impostazione colore base del renderer con "SDL_SetRenderDrawColor"
 * - Inizializzazione SDL_image con "IMG_Init"
 * - Inizializzazione SDL_ttf   con "TTF_Init"
 * - Apertura font globale con "TTF_OpenFont"
 * - (Testo) Preparazione superficie temporanea con "TTF_RenderText_Blended"
 * - (Testo) Creazione textures dalla superficie temporanea con "SDL_CreateTextureFromSurface"
 * - Dimensioni delle textures mediante "SDL_QueryTexture"
 * - Creazione main loop, e analisi eventi con "SDL_PollEvent"
 * - Pulizia finestra con "SDL_RenderClear"
 * - Rendering delle texture mediante "SDL_RenderCopy"
 * - Aggiornamento della finestra con "SDL_RenderPresent"
 * - Alla pressione dei tasti freccia UP o DOWN, viene creata una nuova superficie e assegnata alla
 *   texture centrale, per cambiare il testo
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
#include <vector>
#include "colours.hpp"


/**************************************************************************************************
* Private constants and types
***************************************************************************************************/

static constexpr int WINDOW_W  =  800; // Window's width
static constexpr int WINDOW_H  =  600; // Window's height
static constexpr int FIRST_ONE = -1;   // index of the first initialisable rendering driver supporting the requested flags
static constexpr int FONT_SIZE =  50;

// Paths
// static const std::string FontPath ("RachelBrown.ttf");
static const std::string FontPath ("georgia.ttf");

enum class PromptMessages
{
  Press_UP_or_DOWN_arrow_keys,
  Press_any_other_key_to_reset,
  UP_key_pressed,
  DOWN_key_pressed,
  Waiting_for_UP_or_DOWN,

  HOW_MANY
};

static const std::vector<std::string> Prompts_Vec
{
  "Press UP or DOWN arrow keys",
  "Press any other key to reset",
  "UP key pressed",
  "DOWN key pressed",
  "Waiting for UP or DOWN..."
};


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   g_Window   (NULL); // The window we'll be rendering to
static SDL_Renderer* g_Renderer (NULL); // The window renderer

static SDL_Texture*  g_Text1_Tex (NULL);
static SDL_Texture*  g_Text2_Tex (NULL);
static SDL_Texture*  g_Text3_Tex (NULL);

static SDL_Rect      g_Text1_Size{0, 0, 0, 0};
static SDL_Rect      g_Text2_Size{0, 0, 0, 0};
static SDL_Rect      g_Text3_Size{0, 0, 0, 0};

static TTF_Font*     g_Font (NULL); // Globally used font


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

  /* Initialise SDL_ttf */

  if( TTF_Init() == -1 )
  {
    printf( "\nSDL_ttf could not initialise! SDL_ttf Error: \"%s\"", TTF_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: SDL_ttf initialised" );
  }

  g_Font = TTF_OpenFont( FontPath.c_str(), FONT_SIZE );

  if( g_Font == NULL )
  {
    printf( "\nFailed to load \"%s\" font! SDL_ttf Error: \"%s\"\n", FontPath.c_str(), TTF_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: \"%s\" font loaded", FontPath.c_str() );
  }

  /* Caricamento texture da font */

  SDL_Color    Text1_Colour{BLACK_R  , BLACK_G  , BLACK_B  , BLACK_A  };
  SDL_Color    Text2_Colour{RED_R    , RED_G    , RED_B    , RED_A    };
  SDL_Color    Text3_Colour{GRN_DRK_R, GRN_DRK_G, GRN_DRK_B, GRN_DRK_A};

  size_t Index1 = static_cast<size_t>(PromptMessages::Press_UP_or_DOWN_arrow_keys);
  size_t Index2 = static_cast<size_t>(PromptMessages::Press_any_other_key_to_reset);
  size_t Index3 = static_cast<size_t>(PromptMessages::Waiting_for_UP_or_DOWN);

  SDL_Surface* TempSurface1 = TTF_RenderText_Blended( g_Font, Prompts_Vec[Index1].c_str(), Text1_Colour );
  SDL_Surface* TempSurface2 = TTF_RenderText_Blended( g_Font, Prompts_Vec[Index2].c_str(), Text2_Colour );
  SDL_Surface* TempSurface3 = TTF_RenderText_Blended( g_Font, Prompts_Vec[Index3].c_str(), Text3_Colour );

  if( TempSurface1 == NULL || TempSurface2 == NULL || TempSurface3 == NULL )
  {
    printf( "\nUnable to render text surfaces! SDL_ttf Error: \"%s\"", TTF_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: text surfaces loaded" );
  }

  // Create texture from surface pixels
  g_Text1_Tex = SDL_CreateTextureFromSurface( g_Renderer, TempSurface1 );
  g_Text2_Tex = SDL_CreateTextureFromSurface( g_Renderer, TempSurface2 );
  g_Text3_Tex = SDL_CreateTextureFromSurface( g_Renderer, TempSurface3 );

  if( g_Text1_Tex == NULL || g_Text2_Tex == NULL || g_Text3_Tex == NULL )
  {
    printf( "\nUnable to create textures from rendered text! SDL Error: \"%s\"", SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nOK: text textures created" );
  }

  // Get rid of old surfaces
  SDL_FreeSurface( TempSurface1 );
  SDL_FreeSurface( TempSurface2 );
  SDL_FreeSurface( TempSurface3 );
  TempSurface1 = NULL;
  TempSurface2 = NULL;
  TempSurface3 = NULL;

  /* Ottenimento delle dimensioni delle textures */

  SDL_QueryTexture( g_Text1_Tex, NULL, NULL, &g_Text1_Size.w, &g_Text1_Size.h );
  SDL_QueryTexture( g_Text2_Tex, NULL, NULL, &g_Text2_Size.w, &g_Text2_Size.h );
  SDL_QueryTexture( g_Text3_Tex, NULL, NULL, &g_Text3_Size.w, &g_Text3_Size.h );

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
      else if( EventHandler.type == SDL_KEYDOWN )
      {
        switch ( EventHandler.key.keysym.sym )
        {
          case SDLK_UP:
            Index3 = static_cast<size_t>(PromptMessages::UP_key_pressed);
            break;

          case SDLK_DOWN:
            Index3 = static_cast<size_t>(PromptMessages::DOWN_key_pressed);
            break;

          default:
            Index3 = static_cast<size_t>(PromptMessages::Waiting_for_UP_or_DOWN);
            break;
        }

        TempSurface3 = TTF_RenderText_Blended( g_Font, Prompts_Vec[Index3].c_str(), Text3_Colour );
        g_Text3_Tex   = SDL_CreateTextureFromSurface( g_Renderer, TempSurface3 );
        SDL_FreeSurface(TempSurface3);
        TempSurface3 = NULL;
        SDL_QueryTexture( g_Text3_Tex, NULL, NULL, &g_Text3_Size.w, &g_Text3_Size.h );
      }
      else
      { /* Wait for events */ }
    }

    // Clear screen
    SDL_SetRenderDrawColor( g_Renderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
    SDL_RenderClear( g_Renderer );

    // Render textures to screen
    SDL_Rect Text1Dest{ ( WINDOW_W - g_Text1_Size.w ) / 2, 0             , g_Text1_Size.w, g_Text1_Size.h };
    SDL_Rect Text2Dest{ ( WINDOW_W - g_Text2_Size.w ) / 2, g_Text1_Size.h, g_Text2_Size.w, g_Text2_Size.h };
    SDL_Rect Text3Dest{ ( WINDOW_W - g_Text3_Size.w ) / 2, WINDOW_H / 2  , g_Text3_Size.w, g_Text3_Size.h };

    SDL_RenderCopy( g_Renderer, g_Text1_Tex, NULL, &Text1Dest );
    SDL_RenderCopy( g_Renderer, g_Text2_Tex, NULL, &Text2Dest );
    SDL_RenderCopy( g_Renderer, g_Text3_Tex, NULL, &Text3Dest );

    // Update screen
    SDL_RenderPresent( g_Renderer );
  }

  /* Free resources and close SDL */

  SDL_DestroyTexture ( g_Text1_Tex );
  SDL_DestroyTexture ( g_Text2_Tex );
  SDL_DestroyTexture ( g_Text3_Tex );
  SDL_DestroyRenderer( g_Renderer );
  SDL_DestroyWindow  ( g_Window );
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