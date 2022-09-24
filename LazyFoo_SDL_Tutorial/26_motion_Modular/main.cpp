/**
 * @file main.cpp
 * 
 * @brief https://lazyfoo.net/articles/article07/index.php
 * 
 * Come l'esempio 26, ma suddiviso in sorgenti multipli.
 * 
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

#include "Util.h"
#include "Dot.h"
#include "Globals.h"

int main( int argc, char* args[] )
{
  bool HasProgramSucceeded = true;

  printf("\n*** Debugging console ***\n");
  printf("\nProgram started with %d additional arguments.", argc - 1); //  Il primo argomento è il nome dell'eseguibile

  for (int i = 1; i != argc; ++i)
  {
    printf("\nArgument #%d: %s\n", i, args[i]);
  }

  // Start up SDL and create window
  if( !init() )
  {
    printf( "Failed to initialize!\n" );
  }
  else
  {
    // Load media
    if( !loadMedia() )
    {
      printf( "Failed to load media!\n" );
    }
    else
    {
      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

      // The dot that will be moving around on the screen
      Dot dot;

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

          // Handle input for the dot
          dot.handleEvent( e );
        }

        // Move the dot
        dot.move();

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        // Render objects
        dot.render();

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