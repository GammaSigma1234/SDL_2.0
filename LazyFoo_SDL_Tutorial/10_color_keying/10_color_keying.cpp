/**
 * @file 10_color_keying.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/10_color_keying/index.php
 *
 * @brief When rendering multiple images on the screen, having images with transparent backgrounds
 * is usually necessary. Fortunately, SDL provides an easy way to do this using color keying.
 *
 * For this tutorial we're going to wrap the SDL_Texture in a class to make some things easier. For
 * example, if you want to get certain information about the texture such as its width or height you
 * would have to use some SDL functions to query the information for the texture. Instead what we're
 * going to do is use a class to wrap and store the information about the texture.
 *
 * It's a fairly straightforward class in terms of design. It has a constructor/destructor pair, a
 * file loader, a deallocator, a renderer that takes in a position, and functions to get the
 * texture's dimensions. For member variables, it has the texture we're going to wrap, and variables
 * to store the width/height.
 *
 * For this scene there are two textures we're going to load, declared as "gFooTexture" and
 * "gBackgroundTexture". We're going to take the gFooTexture, color key the cyan colored background,
 * and render it on top of the background.
 *
 * The constructor initializes variables, and the destructor calls the deallocator. The deallocator
 * simply checks if a texture exists, destroys it, and reinitializes the member variables.
 *
 * The texture loading function pretty much works as it did in the texture loading tutorial, but
 * with some small but important tweaks. First off, we deallocate the texture in case there's one
 * that's already loaded. Next, we color key the image with SDL_SetColorKey before creating a
 * texture from it. The first argument is the surface we want to color key, the second argument
 * covers whether we want to enable color keying, and the last argument is the pixel we want to
 * color key with.
 *
 * The most cross platform way to create a pixel from RGB color is with "SDL_MapRGB". The first
 * argument is the format we want the pixel in. Fortunately, the loaded surface has a format member
 * variable. The last three variables are the red, green, and blue components for color you want to
 * map. Here we're mapping cyan, which is red 0, green 255, blue 255.
 *
 * After color keying the loaded surface, we create a texture from the loaded and color keyed
 * surface. If the texture was created successfully, we store the width/height of the texture and
 * return whether the texture loaded successfully.
 *
 * Here you see why we needed a wrapper class. Up until now, we have been pretty much been rendering
 * full screen images so we didn't need to specify position. Because we didn't need to specify
 * position, we just called SDL_RenderCopy with the last two arguments as NULL.
 *
 * When rendering a texture in a certain place, you need to specify a destination rectangle that
 * sets the x/y position and width/height. We can't specify the width/height without knowing the
 * original image's dimensions. So here when we render our texture we create a rectangle with the
 * position arguments and the member width/height, and pass in this rectangle to SDL_RenderCopy.
 *
 * The last member functions allow us to get the width/height when we need them.
 *
 * The main loop has our textures rendering. It's a basic loop that handles events, clears the
 * screen, renders the background, renders the stick figure on top of it, and updates the screen.
 *
 * An important thing to note is that order matters when you're rendering multiple things to the
 * screen every frame. If we render the stick figure first, the background will render over it and
 * you won't be able to see the stick figure.
 *
 * Sunto:
 *
 * - main chiama init.
 * - init fa le stesse cose degli ultimi esempi.
 * - la classe privata LTexture racchiude tutte le comuni operazioni che negli esempi precedenti
 *   venivano svolte da funzioni autonome.
 * - in particolare, il metodo "render" di LTexture invoca "SDL_RenderCopy". Quest'ultima copia
 *   una texture nel rendering target. Due parametri facoltativi sono il rettangolo sorgente e
 *   il rettangolo destinazione. Se entrambi sono NULL, l'intera texture sorgente viene copiata
 *   nella destinazione (con eventuale stretching?).
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022) and may not be
 * redistributed without written permission.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

// Screen dimension constants
static constexpr int SCREEN_WIDTH = 640;
static constexpr int SCREEN_HEIGHT = 480;

// Colore bianco
static constexpr int WHITE_R = 0xFF; // Amount of red   needed to compose white
static constexpr int WHITE_G = 0xFF; // Amount of green needed to compose white
static constexpr int WHITE_B = 0xFF; // Amount of blue  needed to compose white
static constexpr int WHITE_A = 0xFF; // Alpha component


/***************************************************************************************************
* Classes
****************************************************************************************************/

// Texture wrapper class
class LTexture
{
  public:
    // Initializes variables
    LTexture(void);

    // Deallocates memory
    ~LTexture(void);

    // Loads image at specified path
    bool loadFromFile( const std::string& path );

    // Deallocates texture
    void free(void);

    // Renders texture at given point
    void render( int x, int y );

    // Gets image dimensions
    int getWidth(void);
    int getHeight(void);

  private:
    SDL_Texture* mTexture; // The actual hardware texture

    // Image dimensions
    int mWidth;
    int mHeight;
};


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void PressEnter(void);


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer

// Scene textures
static LTexture gFooTexture;
static LTexture gBackgroundTexture;

// Paths to files
static std::string Texture("foo.png");
static std::string Background("background.png");


/***************************************************************************************************
* Methods definitions
****************************************************************************************************/

LTexture::LTexture(void)
{
  // Initialize
  mTexture = NULL;
  mWidth   = 0;
  mHeight  = 0;
}


LTexture::~LTexture(void)
{
  // Deallocate
  free();
}


bool LTexture::loadFromFile( const std::string& path )
{
  // Get rid of preexisting texture
  free();

  // The final texture
  SDL_Texture* newTexture = NULL;

  // Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

  if( loadedSurface == NULL )
  {
    printf( "Unable to load image \"%s\"! SDL_image Error: \"%s\"\n", path.c_str(), IMG_GetError() );
  }
  else
  {
    printf( "\nImage \"%s\" loaded", path.c_str() );

    // Color key image
    if ( SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0x00, 0xFF, 0xFF ) ) != 0 )
    {
      printf("\nFailed to set colour key for surface \"%s\"! SDL error: \"%s\"", path.c_str(), SDL_GetError());
    }
    else
    {
      printf("\nColour key set for surface \"%s\"", path.c_str());
    }

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

    if( newTexture == NULL )
    {
      printf( "Unable to create texture from \"%s\"! SDL Error: \"%s\"\n", path.c_str(), SDL_GetError() );
    }
    else
    {
      printf( "\nTexture created from \"%s\"", path.c_str() );

      // Get image dimensions
      mWidth  = loadedSurface->w;
      mHeight = loadedSurface->h;
    }

    // Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
  }

  // Return success
  mTexture = newTexture;

  return mTexture != NULL;
}


void LTexture::free()
{
  // Free texture if it exists
  if( mTexture != NULL )
  {
    SDL_DestroyTexture( mTexture );
    mTexture = NULL;
    mWidth   = 0;
    mHeight  = 0;

    printf("\nTexture freed");
  }
  else
  {
    printf("\nTexture is already free");
  }
}


/**
 * @brief When rendering a texture in a certain place, you need to specify a destination rectangle
 * that sets the (x, y) position and (width, height).
 *
 * @param x x position of the texture
 * @param y y position of the texture
 **/
void LTexture::render( int x, int y )
{
  // Set rendering space and render to screen
  SDL_Rect renderQuad = { x, y, mWidth, mHeight };
  SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad );
}


int LTexture::getWidth()
{
  return mWidth;
}

int LTexture::getHeight()
{
  return mHeight;
}


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
    printf( "\nSDL could not initialize! SDL Error: \"%s\"\n", SDL_GetError() );
    success = false;
  }
  else
  {
    printf( "\nSDL initialised" );

    // Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
      printf( "\nWarning: Linear texture filtering not enabled!" );
      success = false;
    }
    else
    {
      printf( "\nLinear texture filtering enabled" );
    }

    // Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nWindow created" );

      // Create renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );

      if( gRenderer == NULL )
      {
        printf( "\nRenderer could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
        success = false;
      }
      else
      {
        printf( "\nRenderer created" );

        // Initialize renderer color
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

        // Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;

        if( !( IMG_Init( imgFlags ) & imgFlags ) )
        {
          printf( "\nSDL_image could not initialize! SDL_image Error: \"%s\"\n", IMG_GetError() );
          success = false;
        }
        else
        {
          printf( "\nSDL_image initialised" );
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

  // Load Foo' texture
  if( !gFooTexture.loadFromFile( Texture ) )
  {
    printf( "\nFailed to load Foo' texture image!" );
    success = false;
  }
  else
  {
    printf( "\nTexture image \"%s\" loaded", Texture.c_str() );
  }

  // Load background texture
  if( !gBackgroundTexture.loadFromFile( Background ) )
  {
    printf( "\nFailed to load background texture image!" );
    success = false;
  }
  else
  {
    printf( "\nTexture image \"%s\" loaded", Background.c_str() );
  }

  return success;
}


/**
 * @brief Frees media and shuts down SDL
 **/
static void close(void)
{
  // Free loaded images
  gFooTexture.free();
  gBackgroundTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow   = NULL;
  gRenderer = NULL;

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

  // Start up SDL and create window
  if( !init() )
  {
    printf( "\nFailed to initialize!\n" );
    HasProgramSucceeded = false;
  }
  else
  {
    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!\n" );
      HasProgramSucceeded = false;
    }
    else
    {
      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

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
          else
          {;} // Wait for events
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render background texture to screen (order matters!)
        gBackgroundTexture.render( 0, 0 );

        // Render Foo to the screen (order matters!)
        gFooTexture.render( 240, 190 );

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