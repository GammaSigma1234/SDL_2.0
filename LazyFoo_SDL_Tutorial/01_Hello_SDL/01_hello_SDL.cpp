/**
 * @file 01_hello_SDL.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/mingw/
 *
 * - Primo tutorial. Per maggiori informazioni, consultare README.md.
 * - Compilare con "Build.bat" ed eseguire con "Run.bat".
 * - Per evitare di visualizzare la console di debugging, ricompilare passando l'opzione
 *   "-Wl,-subsystem,windows".
 *
 * Sunto:
 *  - Servono due puntatori: uno per la finestra (Window) e uno per disegnare nella superficie della
 *    finestra (WindowSurface).
 *  - Primo passo: inizializzare SDL mediante SDL_Init.
 *  - Assegnare il puntatore Window mediante SDL_CreateWindow.
 *  - Assegnare il puntatore WindowSurface mediante SDL_GetWindowSurface.
 *  - In questo semplice esempio, disegneremo un rettangolo bianco sulla superficie mediante
 *    SDL_FillRect.
 *  - Una volta disegnata la superficie, è necessario aggiornarla mediante SDL_UpdateWindowSurface.
 *  - Per evitare che la finestra si chiuda subito, possiamo congelarla con SDL_Delay.
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
constexpr int SCREEN_W = 1024;
constexpr int SCREEN_H = 768;


/***************************************************************************************************
* Main function
****************************************************************************************************/

int main( int argc, char* args[] )
{
  printf( "\n***** Debugging console *****\n" );

  bool HasProgramSucceeded = true;

  SDL_Window*  Window        = NULL; // The window we'll be rendering to
  SDL_Surface* WindowSurface = NULL; // The surface contained by the window

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
    Window = SDL_CreateWindow( "SDL Tutorial - Wait 5 seconds", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN );

    if( Window == NULL )
    {
      printf( "\nWindow could not be created! SDL_Error: %s\n", SDL_GetError() );
      HasProgramSucceeded = false;
    }
    else
    {
      printf( "\nWindow created" );

      // Get window surface
      WindowSurface = SDL_GetWindowSurface( Window );

      if( WindowSurface == NULL )
      {
        printf( "\nWindow surface could not be created! SDL_Error: %s\n", SDL_GetError() );
        HasProgramSucceeded = false;
      }
      else
      {
        printf( "\nWindow surface created" );

        // Fill the surface white
        SDL_FillRect( WindowSurface, NULL, SDL_MapRGB( WindowSurface->format, 0xFF, 0xFF, 0xFF ) );
        printf( "\nWindow surface filled white" );

        // Update the surface
        SDL_UpdateWindowSurface( Window );
        printf( "\nWindow surface updated" );

        // Wait two seconds
        printf( "\nWait for 5 seconds..." );
        SDL_Delay( 5000 );
      }

    } // Window created

  } // SDL initialised

  // Destroy window
  SDL_DestroyWindow( Window );

  // Quit SDL subsystems
  SDL_Quit();

  // Integrity check - Questi messaggi non saranno leggibili, perché la console si chiuderà subito.
  // Nei prossimi esempi si userà una funzione di nome "PressEnter".
  if ( HasProgramSucceeded == true )
  {
    printf("\nProgram ended successfully!\n");
  }
  else
  {
    printf("\nThere was a problem during the execution of the program!\n");
  }

  return 0;
}