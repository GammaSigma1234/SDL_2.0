/**
 * @file 11_clip_rendering_and_sprite_sheets.cpp
 *
 * @brief Sometimes you only want to render part of a texture. A lot of times games like to keep
 * multiple images on the same sprite sheet as opposed to having a bunch of textures. Using clip
 * rendering, we can define a portion of the texture to render as opposed to rendering the whole
 * thing.
 *
 * Here we're making a small tweak to the rendering function in the texture class. The render
 * function now accepts a rectangle defining which portion of the texture we want to render. We give
 * it a default argument of NULL in case we want to render the whole texture.
 *
 * For this tutorial, we're going to take a sprite sheet (dots.png) and render each sprite in a
 * different corner of the window. So, we're going to need a texture image (LTexture
 * gSpriteSheetTexture) and 4 rectangles to define the sprites (SDL_Rect gSpriteClips[ 4 ]).
 *
 * The new rendering function for the texture class that supports clip rendering is mostly
 * the same as the previous texture rendering function but with two changes.
 * First, when you're clipping and you're using the dimensions of the clip rectangle instead of the
 * texture, we're going to set the width/height of the destination rectangle (here called
 * renderQuad) to the size of the clip rectangle.
 * Secondly, we're going to pass in the clip rectangle to SDL_RenderCopy as the source rectangle.
 * The source rectangle defines what part of the texture you want to render. When the source
 * rectangle is NULL, the whole texture is rendered.
 *
 * The media loading function loads the texture and then defines the clip rectangles for the circle
 * sprites if the texture loaded successfully.
 *
 * Finally, in the main loop we render the same texture 4 times, but we're rendering a different
 * portion of the sprite sheet in different places each call.
 *
 * Sunto:
 *  - Carichiamo l'immagine principale (4 cerchi) in gSpriteSheetTexture.
 *  - Vogliamo imparare a renderizzare solo una porzione dell'immagine principale, in una posizione
 *    a piacere della finestra.
 *  - Vogliamo renderizzare solo un cerchio alla volta. Poiché l'immagine principale ha sfondo
 *    ciano, usiamo SDL_SetColorKey per impostare il ciano come colore di trasparenza.
 *  - Modifichiamo il metodo di renderizzazione (::render) dell'immagine principale
 *    gSpriteSheetTexture, in modo che accetti le coordinate della posizione della finestra su cui
 *    renderizzare, e un rettangolo che indichi quale porzione di gSpriteSheetTexture vada
 *    renderizzata (NULL in caso di renderizzazione completa). L'idea è di renderizzare un cerchio
 *    per ciascuno dei quattro angoli della finestra.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, standard math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

// Screen dimension constants
static constexpr int SCREEN_WIDTH  = 640;
static constexpr int SCREEN_HEIGHT = 480;
static const std::string FilePath("dots.png");


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

    void render( int x, int y, SDL_Rect* clip = NULL );

    // Gets image dimensions
    int getWidth(void) const;
    int getHeight(void) const;

  private:
    // The actual hardware texture
    SDL_Texture* mTexture;

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

// Scene sprites
static SDL_Rect gSpriteClips[ 4 ];   // Singoli clip, riuniti in un array per comodità. Sono degli SDL_Rect perché servono solo a definire la posizione e la dimensione dei clip
static LTexture gSpriteSheetTexture; // Immagine completa, da clippare


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
    printf( "\nUnable to load image \"%s\"! SDL_image Error: %s", path.c_str(), IMG_GetError() );
  }
  else
  {
    printf( "\nImage \"%s\" loaded", path.c_str() );

    // Color key image
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

    if( newTexture == NULL )
    {
      printf( "\nUnable to create texture from \"%s\"! SDL Error: %s", path.c_str(), SDL_GetError() );
    }
    else
    {
      printf( "\nTexture create from \"%s\"", path.c_str() );

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

void LTexture::free(void)
{
  // Free texture if it exists
  if( mTexture != NULL )
  {
    SDL_DestroyTexture( mTexture );
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
  }
}

/**
 * @brief Renders texture at given (x, y) point. Accepts a rectangle defining which portion of the texture
 * we want to render. Default argument of NULL to render the whole texture.
 *
 * @param x x coordinate of the rendering point.
 * @param y y coordinate of the rendering point.
 * @param clip Rectangle defining which portion of the texture we want to render. NULL to render the
 * whole texture.
 **/
void LTexture::render( int x, int y, SDL_Rect* clip )
{
  // Set rendering space and render to screen
  SDL_Rect renderQuad = { x, y, mWidth, mHeight };

  // Set clip rendering dimensions
  if( clip != NULL )
  {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }
  else
  {;}

  // Render to screen
  SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );
}

int LTexture::getWidth(void) const
{
  return mWidth;
}

int LTexture::getHeight(void) const
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
    printf( "\nSDL could not initialize! SDL Error: %s", SDL_GetError() );
    success = false;
  }
  else
  {
    printf( "\nSDL initialised" );

    // Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
      printf( "\nWarning: Linear texture filtering not enabled!" );
    }
    else
    {
      printf( "\nLinear texture filtering enabled" );
    }

    // Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: %s", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nWindow created" );

      // Create renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );

      if( gRenderer == NULL )
      {
        printf( "\nRenderer could not be created! SDL Error: %s", SDL_GetError() );
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
          printf( "\nSDL_image could not initialize! SDL_mage Error: %s", IMG_GetError() );
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
 * @brief Loads all necessary media for this project. Loads the texture and then defines the clip
 * rectangles for the circle sprites if the texture loaded successfully.
 *
 * @return true if loading was successful; false otherwise
 **/
static bool loadMedia(void)
{
  // Loading success flag
  bool success = true;

  // Load sprite sheet texture
  if( !gSpriteSheetTexture.loadFromFile( FilePath ) )
  {
    printf( "\nFailed to load sprite sheet texture \"%s\"!", FilePath.c_str() );
    success = false;
  }
  else
  {
    printf( "\nSprite sheet texture \"%s\" loaded", FilePath.c_str() );
    printf( "\nSetting sprite sheet clips" );

    // Set top left sprite
    gSpriteClips[ 0 ].x =   0;
    gSpriteClips[ 0 ].y =   0;
    gSpriteClips[ 0 ].w = 100;
    gSpriteClips[ 0 ].h = 100;

    // Set top right sprite
    gSpriteClips[ 1 ].x = 100;
    gSpriteClips[ 1 ].y =   0;
    gSpriteClips[ 1 ].w = 100;
    gSpriteClips[ 1 ].h = 100;

    // Set bottom left sprite
    gSpriteClips[ 2 ].x =   0;
    gSpriteClips[ 2 ].y = 100;
    gSpriteClips[ 2 ].w = 100;
    gSpriteClips[ 2 ].h = 100;

    // Set bottom right sprite
    gSpriteClips[ 3 ].x = 100;
    gSpriteClips[ 3 ].y = 100;
    gSpriteClips[ 3 ].w = 100;
    gSpriteClips[ 3 ].h = 100;
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gSpriteSheetTexture.free();

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
          {;} // Continue main loop
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        // Render top left sprite
        gSpriteSheetTexture.render( 0                                 , 0                                  , &gSpriteClips[ 0 ] );

        // Render top right sprite
        gSpriteSheetTexture.render( SCREEN_WIDTH - gSpriteClips[ 1 ].w, 0                                  , &gSpriteClips[ 1 ] );

        // Render bottom left sprite
        gSpriteSheetTexture.render( 0                                 , SCREEN_HEIGHT - gSpriteClips[ 2 ].h, &gSpriteClips[ 2 ] );

        // Render bottom right sprite
        gSpriteSheetTexture.render( SCREEN_WIDTH - gSpriteClips[ 3 ].w, SCREEN_HEIGHT - gSpriteClips[ 3 ].h, &gSpriteClips[ 3 ] );

        // Update screen
        SDL_RenderPresent( gRenderer );
      }
    }
  }

  // Free resources and close SDL
  close();

  //  Integrity check
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