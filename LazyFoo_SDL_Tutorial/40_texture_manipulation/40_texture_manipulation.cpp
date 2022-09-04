/**
 * @file 40_texture_manipulation.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/40_texture_manipulation/index.php
 *
 * To do graphics effects often requires pixel access. In this tutorial we'll be altering an image's
 * pixels to white out the background.
 *
 * Sunto:
 *  - Lock/unlock: serve per poter manipolare i pixel.
 *  - mPixel: puntatore ai pixel grezzi dopo il locking.
 *  - Pitch: occupazione in memoria della texture.
 *  - "loadFromFile" è cambiata: ora fa uso di "SDL_ConvertSurfaceFormat" e "SDL_CreateTexture" con
 *    "SDL_TEXTUREACCESS_STREAMING". La creazione di texture che abbiamo utilizzato fino al tutorial
 *    precedente assegnava implicitamente "SDL_TEXTUREACCESS_STATIC".
 *  - Dopo aver bloccato la superficie, copiamo i pixel grezzi nella nuova texture vuota con memcpy.
 *    The pitch SDL gives us is the number of bytes per row of pixels, so all we have to do is
 *    multiply by the height of the surface to copy in all the pixels from the image.
 *  - Una volta caricata la texture streamabile, la modifichiamo in "loadMedia". In sostanza, è un
 *    colour-keying manuale.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/**************************************************************************************************
* Includes
***************************************************************************************************/

// Using SDL, SDL_image, standard I/O, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int FIRST_ONE = -1;

static constexpr int WINDOW_W = 800; // Screen's width
static constexpr int WINDOW_H = 600; // Screen's heigth

// Colore bianco
static constexpr int WHITE_R = 0xFF; // Amount of red   needed to compose white
static constexpr int WHITE_G = 0xFF; // Amount of green needed to compose white
static constexpr int WHITE_B = 0xFF; // Amount of blue  needed to compose white
static constexpr int WHITE_A = 0xFF; // Alpha component

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan
static constexpr int CYAN_A = 0xFF; // Alpha component


/* Paths */

static const std::string g_FooTexture( "foo.png" );


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
  bool loadFromFile( const std::string& );

  #if defined(SDL_TTF_MAJOR_VERSION)
  // Creates image from font string
  bool loadFromRenderedText( const std::string&, SDL_Color );
  #endif

  // Deallocates texture
  void free(void);

  // Set color modulation
  void setColor( Uint8, Uint8, Uint8 );

  // Set blending
  void setBlendMode( SDL_BlendMode );

  // Set alpha modulation
  void setAlpha( Uint8 );

  // Renders texture at given point
  void render( int, int, SDL_Rect* = NULL, double = 0.0, SDL_Point* = NULL, SDL_RendererFlip = SDL_FLIP_NONE );

  // Gets image dimensions
  int getWidth ( void ) const;
  int getHeight( void ) const;

  // Pixel manipulators
  bool  lockTexture   ( void );
  bool  unlockTexture ( void );
  void* getPixels     ( void ) const;
  int   getPitch      ( void ) const;

private:

  // The actual hardware texture
  SDL_Texture* mTexture;
  void*        mPixels;
  int          mPitch;

  // Image dimensions
  int mWidth;
  int mHeight;
};


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init      (void);
static bool loadMedia (void);
static void close     (void);


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer

// Scene textures
static LTexture gFooTexture;


/***************************************************************************************************
* Methods definitions
****************************************************************************************************/

LTexture::LTexture(void)
{
  // Initialize
  mTexture = NULL;
  mWidth   = 0;
  mHeight  = 0;
  mPixels  = NULL;
  mPitch   = 0;
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
    printf( "\nUnable to load image \"%s\"! SDL_image Error: %s", path.c_str(), IMG_GetError() );
  }
  else
  {
    printf( "\nOK: image \"%s\" loaded", path.c_str() );

    // Convert surface to display format
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_GetWindowPixelFormat( gWindow ), 0 );

    if( formattedSurface == NULL )
    {
      printf( "\nUnable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError() );
    }
    else
    {
      // Create blank streamable texture
      newTexture = SDL_CreateTexture( gRenderer, SDL_GetWindowPixelFormat( gWindow ), SDL_TEXTUREACCESS_STREAMING,
                                      formattedSurface->w, formattedSurface->h );

      if( newTexture == NULL )
      {
        printf( "\nUnable to create blank texture! SDL Error: %s\n", SDL_GetError() );
      }
      else
      {
        // Lock texture for manipulation
        SDL_LockTexture( newTexture, NULL, &mPixels, &mPitch );

        // Copy loaded/formatted surface pixels
        memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

        // Unlock texture to update
        SDL_UnlockTexture( newTexture );
        mPixels = NULL;

        // Get image dimensions
        mWidth = formattedSurface->w;
        mHeight = formattedSurface->h;
      }

      // Get rid of old formatted surface
      SDL_FreeSurface( formattedSurface );
    }

    // Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
  }

  // Return success
  mTexture = newTexture;

  return mTexture != NULL;
}


#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( const std::string& textureText, SDL_Color textColor )
{
  // Get rid of preexisting texture
  free();

  // Render text surface
  SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );

  if( textSurface != NULL )
  {
    // Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );

    if( mTexture == NULL )
    {
      printf( "\nUnable to create texture from rendered text! SDL Error: %s", SDL_GetError() );
    }
    else
    {
      printf( "\nOK: texture created from rendered text" );
      // Get image dimensions
      mWidth  = textSurface->w;
      mHeight = textSurface->h;
    }

    // Get rid of old surface
    SDL_FreeSurface( textSurface );
  }
  else
  {
    printf( "\nUnable to render text surface! SDL_ttf Error: %s", TTF_GetError() );
  }

  // Return success
  return mTexture != NULL;
}
#endif


void LTexture::free(void)
{
  // Free texture if it exists
  if( mTexture != NULL )
  {
    SDL_DestroyTexture( mTexture );
    mTexture = NULL;
    mWidth   = 0;
    mHeight  = 0;
    mPixels  = NULL;
    mPitch  = 0;
  }
  else { /* Nothing to destroy */ }
}


/**
 * @brief Modulate texture rgb
 *
 * @param red
 * @param green
 * @param blue
 **/
void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
  SDL_SetTextureColorMod( mTexture, red, green, blue );
}


/**
 * @brief Set blending function
 *
 * @param blending
 **/
void LTexture::setBlendMode( SDL_BlendMode blending )
{
  SDL_SetTextureBlendMode( mTexture, blending );
}


/**
 * @brief Modulate texture alpha
 *
 * @param alpha
 **/
void LTexture::setAlpha( Uint8 alpha )
{
  SDL_SetTextureAlphaMod( mTexture, alpha );
}


void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
  // Set rendering space and render to screen
  SDL_Rect renderQuad = { x, y, mWidth, mHeight };

  // Set clip rendering dimensions
  if( clip != NULL )
  {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }
  else { /* Render the whole texture */ }

  // Render to screen
  SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}


int LTexture::getWidth(void) const
{
  return mWidth;
}


int LTexture::getHeight(void) const
{
  return mHeight;
}


bool LTexture::lockTexture(void)
{
  bool success = true;

  // Texture is already locked
  if( mPixels != NULL )
  {
    printf( "\nTexture is already locked!" );
    success = false;
  }
  // Lock texture
  else
  {
    printf( "\nLocking texture..." );

    if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
    {
      printf( "\nUnable to lock texture! %s\n", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nOK: texture locked" );
    }
  }

  return success;
}


bool LTexture::unlockTexture(void)
{
  bool success = true;

  // Texture is not locked
  if( mPixels == NULL )
  {
    printf( "\nTexture is not locked!" );
    success = false;
  }
  // Unlock texture
  else
  {
    printf( "\nUnlocking texture..." );

    SDL_UnlockTexture( mTexture );
    mPixels = NULL;
    mPitch = 0;
  }

  return success;
}


void* LTexture::getPixels(void) const
{
  return mPixels;
}


int LTexture::getPitch(void) const
{
  return mPitch;
}


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
    gWindow = SDL_CreateWindow( "SDL Tutorial"         ,
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                WINDOW_W               , WINDOW_H               ,
                                SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nOK: window created" );

      // Create renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, FIRST_ONE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

      if( gRenderer == NULL )
      {
        printf( "\nRenderer could not be created! SDL Error: %s", SDL_GetError() );
        success = false;
      }
      else
      {
        printf( "\nOK: renderer created" );

        // Initialize renderer color
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );

        // Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;

        if( !( IMG_Init( imgFlags ) & imgFlags ) )
        {
          printf( "\nSDL_image could not initialise! SDL_image Error: %s", IMG_GetError() );
          success = false;
        }
        else
        {
          printf( "\nOK: SDL_image initialised" );
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
static bool loadMedia( void )
{
  // Loading success flag
  bool success = true;

  // Load Foo texture
  if( !gFooTexture.loadFromFile( g_FooTexture ) )
  {
    printf( "\nFailed to load corner texture!" );
    success = false;
  }
  else
  {
    printf( "\nOK: corner texture loaded" );

    // Lock texture
    if( !gFooTexture.lockTexture() )
    {
      printf( "\nUnable to lock Foo' texture!\n" );
    }
    // Manual color key
    else
    {
      printf( "\nOK: Foo texture locked" );

      // Allocate format from window
      Uint32           format        = SDL_GetWindowPixelFormat( gWindow );
      SDL_PixelFormat* mappingFormat = SDL_AllocFormat( format );

      // Get pixel data
      Uint32* pixels     = (Uint32*)gFooTexture.getPixels();
      int     pixelCount = ( gFooTexture.getPitch() / 4 ) * gFooTexture.getHeight();

      // Map colors
      Uint32 colorKey    = SDL_MapRGB ( mappingFormat, CYAN_R , CYAN_G , CYAN_B );
      Uint32 transparent = SDL_MapRGBA( mappingFormat, WHITE_R, WHITE_G, WHITE_B, 0x00 );

      // Color key pixels
      for( int i = 0; i != pixelCount; ++i )
      {
        if( pixels[ i ] == colorKey )
        {
          pixels[ i ] = transparent;
        }
        else { /*  */ }
      }

      // Unlock texture
      gFooTexture.unlockTexture();

      // Free format
      SDL_FreeFormat( mappingFormat );
    }
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gFooTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
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
          else { /* Event not managed here */ }
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render stick figure
        gFooTexture.render( ( WINDOW_W - gFooTexture.getWidth() ) / 2, ( WINDOW_H - gFooTexture.getHeight() ) / 2 );

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