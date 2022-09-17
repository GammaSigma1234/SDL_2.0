/**
 * @file 50_SDL_and_opengl_2.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/50_SDL_and_opengl_2/index.php
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022) and may not be
 * redistributed without written permission.
 **/


/**************************************************************************************************
* Includes
***************************************************************************************************/

// Using SDL, SDL OpenGL, standard I/O, and, strings

#include <SDL.h>
#include <SDL_opengl.h>
#include <GL\GLU.h>
#include <stdio.h>
#include <string>
#include "colours.hpp"


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int WINDOW_W = 640;
static constexpr int WINDOW_H = 480;


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init      (void); // Starts up SDL, creates window, and initializes OpenGL
static bool initGL    (void); // Initializes matrices and clear color
static void handleKeys( unsigned char key, int x, int y ); // Input handler
static void update    (void); // Per frame update
static void render    (void); // Renders quad to the screen
static void close     (void); // Frees media and shuts down SDL


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow = NULL; // The window we'll be rendering to
static SDL_GLContext gContext; // OpenGL context
static bool          gRenderQuad = true; // Render flag


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

static bool init(void)
{
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    success = false;
  }
  else
  {
    printf( "\nOK: SDL initialised" );

    // Use OpenGL 2.1
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

    // Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W, WINDOW_H, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nOK: window created" );

      // Create context
      gContext = SDL_GL_CreateContext( gWindow );

      if( gContext == NULL )
      {
        printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
        success = false;
      }
      else
      {
        printf( "OK: OpenGL context created" );

        // Use Vsync
        if( SDL_GL_SetSwapInterval( 1 ) < 0 )
        {
          printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
          printf( "\nOK: VSync set" );
        }

        // Initialize OpenGL
        if( !initGL() )
        {
          printf( "Unable to initialize OpenGL!\n" );
          success = false;
        }
        else
        {
          printf( "\nOK: OpenGL initialised" );
        }
      }
    }
  }

  return success;
}


static bool initGL(void)
{
  bool success = true;
  GLenum error = GL_NO_ERROR;

  // Initialize Projection Matrix
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  // Check for error
  error = glGetError();

  if( error != GL_NO_ERROR )
  {
    printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
    success = false;
  }
  else
  {
    printf( "\nOK: Projection Matrix initialised" );
  }

  // Initialize Modelview Matrix
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // Check for error
  error = glGetError();

  if( error != GL_NO_ERROR )
  {
    printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
    success = false;
  }
  else
  {
    printf( "\nOK: Modelview Matrix initialised" );
  }

  // Initialize clear color
  glClearColor( 0.f, 0.f, 0.f, 1.f );

  // Check for error
  error = glGetError();

  if( error != GL_NO_ERROR )
  {
    printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
    success = false;
  }
  else
  {
    printf( "\nOK: clear colour initialised" );
  }

  return success;
}


static void handleKeys( unsigned char key, [[maybe_unused]] int x, [[maybe_unused]] int y )
{
  // Toggle quad
  if( key == 'q' )
  {
    gRenderQuad = !gRenderQuad;
  }
  else { /*  */ }
}


/**
 * @brief the update handler is just there for compatibility's sake
 **/
static void update(void)
{
  // No per frame update needed
}


static void render(void)
{
  // Clear color buffer
  glClear( GL_COLOR_BUFFER_BIT );

  // Render quad
  if( gRenderQuad )
  {
    glBegin( GL_QUADS );
      glVertex2f( -0.5f, -0.5f );
      glVertex2f(  0.5f, -0.5f );
      glVertex2f(  0.5f,  0.5f );
      glVertex2f( -0.5f,  0.5f );
    glEnd();
  }
}


static void close(void)
{
  // Destroy window
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  // Quit SDL subsystems
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
    printf( "\nFailed to initialise!" );
  }
  else
  {
    printf( "\nOK: all systems initialised" );

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // Enable text input
    SDL_StartTextInput();

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
        // Handle keypress with current mouse position
        else if( e.type == SDL_TEXTINPUT )
        {
          int x = 0, y = 0;
          SDL_GetMouseState( &x, &y );
          handleKeys( e.text.text[ 0 ], x, y );
        }
        else { /* Event not managed here */ }
      }

      // Render quad
      render();

      // Update screen
      SDL_GL_SwapWindow( gWindow );
    }

    // Disable text input
    SDL_StopTextInput();
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