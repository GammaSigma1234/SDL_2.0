/**
 * @file 01_Hello_SDL_finestre_multiple.cpp
 *
 * @brief Simile al tutorial 01, con la differenza che qui si creano due finestre.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2020)
 * and may not be redistributed without written permission.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

// Screen dimension constants
constexpr int WINDOW_WIDTH  = 640;
constexpr int WINDOW_HEIGHT = 480;


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

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
  /* Start debugging console */

  printf("\n*** Debugging console ***\n");
  printf("\nProgram started with %d additional arguments.", argc - 1); // Il primo argomento è il nome dell'eseguibile

  for (int i = 1; i != argc; ++i)
  {
    printf("\nArgument #%d: %s\n", i, args[i]);
  }

  bool HasProgramSucceeded = true;

  SDL_Window*  Window_1         = NULL; // The first  window we'll be rendering to
  SDL_Window*  Window_2         = NULL; // The second window we'll be rendering to
  SDL_Surface* Window_1_Surface = NULL; // The surface contained by the first  window
  SDL_Surface* Window_2_Surface = NULL; // The surface contained by the second window

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "\nSDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nSDL initialised" );

    // Create window
    Window_1 = SDL_CreateWindow( "SDL Tutorial", 20,                50, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN );
    Window_2 = SDL_CreateWindow( "SDL Tutorial", 30 + WINDOW_WIDTH, 50, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN );

    if( Window_1 == NULL || Window_2 == NULL )
    {
      printf( "\nA window could not be created! SDL_Error: %s\n", SDL_GetError() );
      HasProgramSucceeded = false;
    }
    else
    {
      printf( "\nTwo windows created" );

      // Get window surface
      Window_1_Surface = SDL_GetWindowSurface( Window_1 );
      Window_2_Surface = SDL_GetWindowSurface( Window_2 );

      if( Window_1_Surface == NULL || Window_2_Surface == NULL )
      {
        printf( "\nA window surface could not be created! SDL_Error: %s\n", SDL_GetError() );
        HasProgramSucceeded = false;
      }
      else
      {
        printf( "\nSurface created for both windows" );

        // Fill the first surface green
        SDL_FillRect( Window_1_Surface, NULL, SDL_MapRGB( Window_1_Surface->format, 0x00, 0x80, 0x00 ) );
        printf( "\nWindow 1's surface filled green" );

        // Fill the second surface red
        SDL_FillRect( Window_2_Surface, NULL, SDL_MapRGB( Window_1_Surface->format, 0xFF, 0x00, 0x00 ) );
        printf( "\nWindow 2's surface filled red" );

        // Update the surfaces
        if ( SDL_UpdateWindowSurface( Window_1 ) != 0 || SDL_UpdateWindowSurface( Window_2 ) != 0 )
        {
          printf( "\nA window surface could not be updated! SDL_Error: %s\n", SDL_GetError() );
          HasProgramSucceeded = false;
        }
        else
        {
          printf( "\nWindow surfaces updated" );
        }

        PressEnter();

      } // Surface created for both windows

    } // Two windows created

  } // SDL initialised

  // Destroy window
  SDL_DestroyWindow( Window_1 );
  SDL_DestroyWindow( Window_2 );
  printf("\nWindows destroyed");

  // Quit SDL subsystems
  SDL_Quit();
  printf("\nSDL subsystems closed");

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