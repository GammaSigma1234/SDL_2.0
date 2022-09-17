/**
 * @file 51_SDL_and_modern_opengl.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php
 *
 * With OpenGL 3 there was a massive overhaul that made everything shader-based. In this tutorial
 * we'll be rendering a quad using core modern OpenGL.
 *
 * For this tutorial we'll be using the OpenGL Extension Wrangler. Certain operating systems, like
 * Windows, only support a limited amount of OpenGL by default. Using GLEW you can get the latest
 * functionality.
 *
 * The way modern OpenGL works is that we create shader programs (gProgramID) that process vertex
 * attributes like positions (gVertexPos2DLocation). We put vertices in Vertex Buffer Objects (gVBO)
 * and specify the order in which to draw them using Index Buffer Objects.
 *
 * "initGL": After we create our context, we initialize GLEW. Since we want the latest features, we
 * have to set "glewExperimental" to true. After that we call "glewInit()" to initialize GLEW.
 *
 * After we get the shader program working, we create the VBO and IBO. The VBO has the same
 * positions as the quad from the last tutorial.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/**************************************************************************************************
* Includes
***************************************************************************************************/

// Using SDL, SDL OpenGL, GLEW, standard I/O, and strings
#include <SDL.h>
#include <glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
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
static bool initGL    (void); // Initializes rendering program and clear color
static void handleKeys( unsigned char key, int x, int y ); // Input handler
static void update    (void); // Per frame update
static void render    (void); // Renders quad to the screen
static void close     (void); // Frees media and shuts down SDL

// Shader loading utility programs
static void printProgramLog( GLuint program );
static void printShaderLog ( GLuint shader );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow = NULL; // The window we'll be rendering to
static SDL_GLContext gContext; // OpenGL context
static bool          gRenderQuad = true; // Render flag

// Graphics program
static GLuint gProgramID           =  0;
static GLint  gVertexPos2DLocation = -1;
static GLuint gVBO                 =  0;
static GLuint gIBO                 =  0;


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

    // Use OpenGL 3.1 core
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

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
        printf( "\nOpenGL context could not be created! SDL Error: %s", SDL_GetError() );
        success = false;
      }
      else
      {
        printf( "\nOK: OpenGL context created" );

        // Initialize GLEW
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();

        if( glewError != GLEW_OK )
        {
          printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
        }
        else
        {
          printf( "\nOK: GLEW initialised" );
        }

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
  // Success flag
  bool success = true;

  // Generate shader program
  gProgramID = glCreateProgram();

  // Create vertex shader
  GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );

  // Get vertex source
  const GLchar* vertexShaderSource[] =
  {
    "#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"
  };

  // Set vertex source
  glShaderSource( vertexShader, 1, vertexShaderSource, NULL );

  // Compile vertex source
  glCompileShader( vertexShader );

  // Check vertex shader for errors
  GLint vShaderCompiled = GL_FALSE;
  glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &vShaderCompiled );

  if( vShaderCompiled != GL_TRUE )
  {
    printf( "\nUnable to compile vertex shader %d!", vertexShader );
    printShaderLog( vertexShader );
    success = false;
  }
  else
  {
    printf( "\nOK: vertex shader compiled" );

    // Attach vertex shader to program
    glAttachShader( gProgramID, vertexShader );

    // Create fragment shader
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    // Get fragment source
    const GLchar* fragmentShaderSource[] =
    {
      "#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"
    };

    // Set fragment source
    glShaderSource( fragmentShader, 1, fragmentShaderSource, NULL );

    // Compile fragment source
    glCompileShader( fragmentShader );

    // Check fragment shader for errors
    GLint fShaderCompiled = GL_FALSE;
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );

    if( fShaderCompiled != GL_TRUE )
    {
      printf( "\nUnable to compile fragment shader %d!", fragmentShader );
      printShaderLog( fragmentShader );
      success = false;
    }
    else
    {
      printf( "\nOK: fragment shader compiled" );

      // Attach fragment shader to program
      glAttachShader( gProgramID, fragmentShader );

      // Link program
      glLinkProgram( gProgramID );

      // Check for errors
      GLint programSuccess = GL_TRUE;
      glGetProgramiv( gProgramID, GL_LINK_STATUS, &programSuccess );

      if( programSuccess != GL_TRUE )
      {
        printf( "\nError linking program %d!", gProgramID );
        printProgramLog( gProgramID );
        success = false;
      }
      else
      {
        printf( "\nOK: program linked" );

        // Get vertex attribute location
        gVertexPos2DLocation = glGetAttribLocation( gProgramID, "LVertexPos2D" );

        if( gVertexPos2DLocation == -1 )
        {
          printf( "LVertexPos2D is not a valid glsl program variable!\n" );
          success = false;
        }
        else
        {
          printf( "\nOK: LVertexPos2D is a valid glsl program variable" );

          // Initialize clear color
          glClearColor( 0.f, 0.f, 0.f, 1.f );

          // VBO data
          GLfloat vertexData[] =
          {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f
          };

          // IBO data
          GLuint indexData[] = { 0, 1, 2, 3 };

          // Create VBO
          glGenBuffers( 1, &gVBO );
          glBindBuffer( GL_ARRAY_BUFFER, gVBO );
          glBufferData( GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW );

          // Create IBO
          glGenBuffers( 1, &gIBO );
          glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
          glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW );
        }
      }
    }
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
    // Bind program
    glUseProgram( gProgramID );

    // Enable vertex position
    glEnableVertexAttribArray( gVertexPos2DLocation );

    // Set vertex data
    glBindBuffer( GL_ARRAY_BUFFER, gVBO );
    glVertexAttribPointer( gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL );

    // Set index data and render
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
    glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

    // Disable vertex position
    glDisableVertexAttribArray( gVertexPos2DLocation );

    // Unbind program
    glUseProgram( NULL );
  }
}


static void close(void)
{
  // Deallocate program
  glDeleteProgram( gProgramID );

  // Destroy window
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  // Quit SDL subsystems
  SDL_Quit();
}


void printProgramLog( GLuint program )
{
  // Make sure name is shader
  if( glIsProgram( program ) )
  {
    // Program log length
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    // Get info string length
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

    // Allocate string
    char* infoLog = new char[ maxLength ];

    // Get info log
    glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
    if( infoLogLength > 0 )
    {
      // Print Log
      printf( "%s\n", infoLog );
    }

    // Deallocate string
    delete[] infoLog;
  }
  else
  {
    printf( "\nName \"%d\" is not a program", program );
  }
}


void printShaderLog( GLuint shader )
{
  // Make sure name is shader
  if( glIsShader( shader ) )
  {
    // Shader log length
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    // Get info string length
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

    // Allocate string
    char* infoLog = new char[ maxLength ];

    // Get info log
    glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
    if( infoLogLength > 0 )
    {
      // Print Log
      printf( "%s\n", infoLog );
    }

    // Deallocate string
    delete[] infoLog;
  }
  else
  {
    printf( "\nName \"%d\" is not a shader", shader );
  }
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