/**
 * @file main.cpp
 *
 * @brief
 **/

// Using SDL, SDL_image, standard I/O, vectors, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>
#include "colours.hpp"
#include "Texture.hpp"
#include "Dot.hpp"
#include "GlobConst.hpp"


/***************************************************************************************************
* Private constants
****************************************************************************************************/

// static const SDL_Color g_TextColorBlack { BLACK_R, BLACK_G, BLACK_B, BLACK_A };
// static const SDL_Color g_TextColorRed   { RED_R  , RED_G  , RED_B  , RED_A   };
// static const SDL_Color g_TextColorWhite { WHITE_R, WHITE_G, WHITE_B, WHITE_A };
static const SDL_Color g_TextColorGold  { GOLD_R , GOLD_G , GOLD_B , GOLD_A  };

static const std::string DotPath         ("WhiteDot.bmp");  // Dot        texture's path
static const std::string BackGroundPath  ("Sfondo.png");    // Background texture's path
static const std::string SpriteSheetPath ("SpriteSheet.png");
static const std::string FontPath        ("lazy.ttf");


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   g_Window   = NULL; // The window we'll be rendering to
static SDL_Renderer* g_Renderer = NULL; // The window renderer

// Globally used font
static TTF_Font* g_Font = NULL;

// Textures
static LTexture g_DotTexture;
static LTexture g_BGTexture;
static LTexture g_SSTexture;
static LTexture g_xPosText_Debug;
static LTexture g_yPosText_Debug;
static LTexture g_xVelText_Debug;
static LTexture g_yVelText_Debug;

// The dot that will be moving around on the screen
static Dot ScreenDot;

/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init      ( void );
static bool loadMedia ( void );
static void close     ( void );


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
    printf( "\nOK: SDL initialised" );

    // Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
      printf( "\nWarning: Linear texture filtering not enabled!" );
      success = false;
    }
    else
    {
      printf( "\nOK: linear texture filtering enabled" );
    }

    // Create window
    g_Window = SDL_CreateWindow( "Pallina",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  WINDOW_W_px,
                                  WINDOW_H_px,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP );

    if( g_Window == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nOK: window created" );

      // Create accelerated and vsynced renderer for window
      g_Renderer = SDL_CreateRenderer( g_Window, INIT_FIRST_ONE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

      if( g_Renderer == NULL )
      {
        printf( "\nRenderer could not be created! SDL Error: %s", SDL_GetError() );
        success = false;
      }
      else
      {
        printf( "\nOK: renderer created" );

        // Initialize renderer color
        SDL_SetRenderDrawColor( g_Renderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );

        // Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;

        if( !( IMG_Init( imgFlags ) & imgFlags ) )
        {
          printf( "\nSDL_image could not initialize! SDL_image Error: %s", IMG_GetError() );
          success = false;
        }
        else
        {
          printf( "\nOK: SDL_image initialised" );
        }

        // Initialize SDL_ttf
        if( TTF_Init() == -1 )
        {
          printf( "\nSDL_ttf could not initialize! SDL_ttf Error: \"%s\"\n", TTF_GetError() );
          success = false;
        }
        else
        {
          printf( "\nOK: SDL_ttf initialised" );
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

 // Open the font
  g_Font = TTF_OpenFont( FontPath.c_str(), 28 );

  if( g_Font == NULL )
  {
    printf( "\nFailed to load lazy font! SDL_ttf Error: \"%s\"\n", TTF_GetError() );
    success = false;
  }
  else
  {
    printf( "\nOK: Lazy Font loaded" );
  }

  // Load dot texture
  if ( !ScreenDot.LoadTexture( g_Renderer, DotPath ) )
  {
    printf( "\nFailed to load dot texture!" );
    success = false;
  }
  else
  {
    printf( "\nOK: dot texture loaded" );
  }

  // Load background texture
  if ( !g_BGTexture.loadFromFile( g_Renderer, BackGroundPath.c_str() ) )
  {
    printf( "\nFailed to load background texture!\n" );
    success = false;
  }
  else
  {
    printf( "\nOK: background texture loaded" );
  }

  // Load sprite sheet
  if ( !g_SSTexture.loadFromFile( g_Renderer, SpriteSheetPath.c_str() ) )
  {
    printf( "\nFailed to load sprite sheet texture!\n" );
    success = false;
  }
  else
  {
    printf( "\nOK: sprite sheet texture loaded" );
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  // g_DotTexture.free();
  g_BGTexture.free();

  // Destroy window
  SDL_DestroyRenderer( g_Renderer );
  SDL_DestroyWindow  ( g_Window );
  g_Renderer = NULL;
  g_Window   = NULL;

  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
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
    printf( "\nFailed to initialize!" );
  }
  else
  {
    printf( "\nOK: all systems initialised" );

    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!" );
    }
    else
    {
      printf( "\nOK: all media loaded" );

      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

            // The camera area
      SDL_Rect camera = { 0, 0, WINDOW_W_px, WINDOW_H_px };

      // The lower wall tile
      SDL_Rect Wall_Lower{0, 0, WALL_W_px, WALL_H_px};

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
          else { /* ignore event */ }

          // Handle input for the dot
          ScreenDot.handleEvent( e );
        }

        // Move the dot
        ScreenDot.ProcessMovement();

        // Center the camera over the dot
        camera.x = ( ScreenDot.getPosX() + ScreenDot.getWidth()  / 2 ) - WINDOW_W_px / 2;
        camera.y = ( ScreenDot.getPosY() + ScreenDot.getHeigth() / 2 ) - WINDOW_H_px / 2;

        // Keep the camera in bounds
        if( camera.x < 0 )
        {
          camera.x = 0;
        }
        else { /* Camera's position is OK */ }

        if( camera.y < 0 )
        {
          camera.y = 0;
        }
        else { /* Camera's position is OK */ }

        if( camera.x > LEVEL_W_px - camera.w )
        {
          camera.x = LEVEL_W_px - camera.w;
        }
        else { /* Camera's position is OK */ }

        if( camera.y > LEVEL_H_px - camera.h )
        {
          camera.y = LEVEL_H_px - camera.h;
        }
        else { /* Camera's position is OK */ }


        // Clear screen
        SDL_SetRenderDrawColor( g_Renderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear       ( g_Renderer );

        // Render background
        g_BGTexture.render( g_Renderer, 0, 0, &camera ); // x = 0, y = 0 --> Il background va sempre agganciato all'origine della finestra

        // Render dot
        ScreenDot.render( g_Renderer, camera.x, camera.y );

        int NumOfWallTiles( LEVEL_W_px / WALL_W_px );

        // Render lower wall
        for ( int i = 0; i != NumOfWallTiles; ++i )
        {
          g_SSTexture.render( g_Renderer, i * WALL_W_px - camera.x, ( LEVEL_H_px - WALL_H_px ) - camera.y, &Wall_Lower );
        }

        /************************************
        * Stampa info di debugging on-screen
        *************************************/

        std::stringstream StringStream_xPos;
        std::stringstream StringStream_yPos;
        std::stringstream StringStream_xVel;
        std::stringstream StringStream_yVel;

        StringStream_xPos << "x pos: " << std::fixed << std::setprecision(Dot::NUM_OF_DIGITS) << ScreenDot.getPosX();
        StringStream_yPos << "y pos: " << std::fixed << std::setprecision(Dot::NUM_OF_DIGITS) << ScreenDot.getPosY();
        StringStream_xVel << "x vel: " << std::fixed << std::setprecision(Dot::NUM_OF_DIGITS) << ScreenDot.getVelX_Debug();
        StringStream_yVel << "y vel: " << std::fixed << std::setprecision(Dot::NUM_OF_DIGITS) << ScreenDot.getVelY_Debug();

        g_xPosText_Debug.loadFromRenderedText(g_Renderer, g_Font, StringStream_xPos.str(), g_TextColorGold );
        g_yPosText_Debug.loadFromRenderedText(g_Renderer, g_Font, StringStream_yPos.str(), g_TextColorGold );
        g_xVelText_Debug.loadFromRenderedText(g_Renderer, g_Font, StringStream_xVel.str(), g_TextColorGold );
        g_yVelText_Debug.loadFromRenderedText(g_Renderer, g_Font, StringStream_yVel.str(), g_TextColorGold );

        // Position of debugging text
        int xPosText_xPos_Debug = ( WINDOW_W_px - g_xPosText_Debug.getWidth()  ) / 2;
        int xPosText_yPos_Debug = ( WINDOW_H_px - g_xPosText_Debug.getHeight() ) / 2 + 0 * g_yPosText_Debug.getHeight();
        int yPosText_xPos_Debug = ( WINDOW_W_px - g_yPosText_Debug.getWidth()  ) / 2;
        int yPosText_yPos_Debug = ( WINDOW_H_px - g_yPosText_Debug.getHeight() ) / 2 + 1 * g_yPosText_Debug.getHeight();

        int xVelText_xPos_Debug = ( WINDOW_W_px - g_xVelText_Debug.getWidth()  ) / 2;
        int xVelText_yPos_Debug = ( WINDOW_H_px - g_xVelText_Debug.getHeight() ) / 2 + 2 * g_xVelText_Debug.getHeight();
        int yVelText_xPos_Debug = ( WINDOW_W_px - g_yVelText_Debug.getWidth()  ) / 2;
        int yVelText_yPos_Debug = ( WINDOW_H_px - g_yVelText_Debug.getHeight() ) / 2 + 3 * g_yVelText_Debug.getHeight();

        g_xPosText_Debug.render( g_Renderer, xPosText_xPos_Debug, xPosText_yPos_Debug );
        g_yPosText_Debug.render( g_Renderer, yPosText_xPos_Debug, yPosText_yPos_Debug );
        g_xVelText_Debug.render( g_Renderer, xVelText_xPos_Debug, xVelText_yPos_Debug );
        g_yVelText_Debug.render( g_Renderer, yVelText_xPos_Debug, yVelText_yPos_Debug );

        // Update screen
        SDL_RenderPresent( g_Renderer );

      } // Main loop

    } // All media loaded

  } // All systems initialised

  close(); // Free resources and close SDL

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