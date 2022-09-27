/**
 * @file 44_frame_independent_movement.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/44_frame_independent_movement/index.php
 *
 * Whether you want to be able to handle unstable frame rates or support multiple frame rates, you
 * can set your movement based on time to make it independent of frame rate.
 *
 * Simile all'esempio già visto in precedenza in cui VSync era stata rimossa dalla chiamata a
 * "SDL_CreateRenderer". Il frame rate di questo esempio è verosimilmente molto alto (migliaia di
 * frames al secondo): bisognerebbe reintrodurre il codice di analisi degli esempi 23-25 per
 * accertarsene. Tuttavia, il pallino percorre comunque i percorsi previsti in tempi previsti (640
 * pixel al secondo), perché la funzione "Dot::move" ora riceve un intervallo di tempo su cui basare
 * l'equazione del moto lineare, mentre prima il moto era agganciato alla frequenza di aggiornamento
 * dello schermo, essendo il renderer regolato dal VSync. L'intervallo di tempo è calcolato da un
 * apposito timer, e garantisce che il pallino si muova nella posizione corretta a prescindere dal
 * frame rate.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/**************************************************************************************************
* Includes
***************************************************************************************************/

// Using SDL, SDL_image, standard I/O, and, strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "colours.hpp"


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int FIRST_ONE       = -1;
static constexpr int WINDOW_W        = 640; // Screen's width
static constexpr int WINDOW_H        = 480; // Screen's heigth
static constexpr int TRANSPARENCY    = 0x00;
static constexpr int NUM_OF_FRAMES   = 4; // I quattro "foo_walk"
static constexpr int BYTES_PER_PIXEL = 4;

static constexpr double MS_IN_S = 1000.0; // Milliseconds in a second

static std::string Path("dot.bmp");


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

  // Creates blank texture
	bool createBlank( int, int, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING );

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

  // Set self as render target
  void setAsRenderTarget(void);

  // Gets image dimensions
  int getWidth ( void ) const;
  int getHeight( void ) const;

  // Pixel manipulators
  bool   lockTexture   ( void  );
  bool   unlockTexture ( void  );
  void*  getPixels     ( void  ) const;
  void   copyPixels    ( void* );
  int    getPitch      ( void  ) const;
  Uint32 getPixel32    ( unsigned int, unsigned int );

  private:

  // The actual hardware texture
  SDL_Texture* mTexture;
  void*        mPixels;
  int          mPitch;

  // Image dimensions
  int mWidth;
  int mHeight;
};


// The application time based timer
class LTimer
{
  public:
  LTimer(void);

  // The various clock actions
  void start  (void);
  void stop   (void);
  void pause  (void);
  void unpause(void);

  // Gets the timer's time
  Uint32 getTicks(void);

  // Checks the status of the timer
  bool isStarted(void) const;
  bool isPaused (void) const;

  private:
  // The clock time when the timer started
  Uint32 mStartTicks;

  // The ticks stored when the timer was paused
  Uint32 mPausedTicks;

  // The timer status
  bool mPaused;
  bool mStarted;
};


// The dot that will move around on the screen
class Dot
{
  public:

  // The dimensions of the dot
  static const int DOT_WIDTH  = 20;
  static const int DOT_HEIGHT = 20;

  // Maximum axis velocity of the dot
  static const int DOT_MAX_VEL_pxPerSec = 640;

  Dot();

  void handleEvent( SDL_Event& e );
  void move       ( double timeStep );
  void render     ( void );

  private:

  double mPosX, mPosY;
  double mVelX, mVelY;
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
static LTexture gDotTexture;


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
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0 );

    if( formattedSurface == NULL )
    {
      printf( "\nUnable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError() );
    }
    else
    {
      // Create blank streamable texture
      newTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );

      if( newTexture == NULL )
      {
        printf( "\nUnable to create blank texture! SDL Error: %s\n", SDL_GetError() );
      }
      else
      {
        // Enable blending on texture
        SDL_SetTextureBlendMode( newTexture, SDL_BLENDMODE_BLEND );

        // Lock texture for manipulation
        SDL_LockTexture( newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch );

        // Copy loaded/formatted surface pixels
        memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

        // Get image dimensions
        mWidth  = formattedSurface->w;
        mHeight = formattedSurface->h;

        // Get pixel data in editable format
        Uint32* pixels = (Uint32*)mPixels;
        int pixelCount = ( mPitch / BYTES_PER_PIXEL ) * mHeight;

        // Map colors
        Uint32 colorKey    = SDL_MapRGB ( formattedSurface->format, CYAN_R, CYAN_G, CYAN_B );
        Uint32 transparent = SDL_MapRGBA( formattedSurface->format, CYAN_R, CYAN_G, CYAN_B, TRANSPARENCY );

        // Color key pixels
        for( int i = 0; i != pixelCount; ++i )
        {
          if( pixels[ i ] == colorKey )
          {
            pixels[ i ] = transparent;
          }
          else { /* Skip this pixel */ }
        }

        // Unlock texture to update
        SDL_UnlockTexture( newTexture );
        mPixels = NULL;
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


bool LTexture::createBlank( int width, int height, SDL_TextureAccess access )
{
  // Create uninitialized texture
  mTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height );

  if( mTexture == NULL )
  {
    printf( "\nUnable to create blank texture! SDL Error: \"%s\"", SDL_GetError() );
  }
  else
  {
    printf( "\nOK: blank texture created" );
    mWidth = width;
    mHeight = height;
  }

  return mTexture != NULL;
}


void LTexture::free( void )
{
  // Free texture if it exists
  if( mTexture != NULL )
  {
    SDL_DestroyTexture( mTexture );
    mTexture = NULL;
    mWidth   = 0;
    mHeight  = 0;
    mPixels  = NULL;
    mPitch   = 0;
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


void LTexture::setAsRenderTarget(void)
{
  // Make self render target
  SDL_SetRenderTarget( gRenderer, mTexture );
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
    if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
    {
      printf( "\nUnable to lock texture! %s\n", SDL_GetError() );
      success = false;
    }
		else {;}
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


/**
 * @brief Copy the pixels from the stream into the texture
 *
 * @param pixels
 **/
void LTexture::copyPixels( void* pixels )
{
  // Texture is locked
  if( mPixels != NULL )
  {
    // Copy to locked pixels
    memcpy( mPixels, pixels, mPitch * mHeight );
  }
  else { /* Cannot copy */ }
}


int LTexture::getPitch(void) const
{
  return mPitch;
}


/**
 * @brief I pixel di una texture bidimensionale vengono memorizzati come un'unica, singola sequenza
 * di bytes. Quindi, è necessario "trasformare" una matrice bidimensionale in un vettore
 * monodimensionale.
 *
 * @param x Coordinata x del pixel da ricavare
 * @param y Coordinata y del pixel da ricavare
 * @return Uint32
 **/
Uint32 LTexture::getPixel32( unsigned int x, unsigned int y )
{
  // Convert the pixels to 32 bit
  Uint32* pixels = (Uint32*)mPixels;

  // Get the pixel requested
  return pixels[ ( y * ( mPitch / 4 ) ) + x ];
}


/**
 * @brief Initializes variables
 **/
LTimer::LTimer(void)
{
  // Initialize the variables
  mStartTicks  = static_cast<Uint32>(0);
  mPausedTicks = static_cast<Uint32>(0);

  mPaused  = false;
  mStarted = false;
}


void LTimer::start(void)
{
  // Start the timer
  mStarted = true;

  // Unpause the timer
  mPaused = false;

  // Get the current clock time
  mStartTicks  = SDL_GetTicks();
  mPausedTicks = 0;
}


void LTimer::stop(void)
{
  // Stop the timer
  mStarted = false;

  // Unpause the timer
  mPaused = false;

  // Clear tick variables
  mStartTicks = 0;
  mPausedTicks = 0;
}


void LTimer::pause(void)
{
  // If the timer is running and isn't already paused
  if( mStarted && !mPaused )
  {
    // Pause the timer
    mPaused = true;

    // Calculate the paused ticks
    mPausedTicks = SDL_GetTicks() - mStartTicks;
    mStartTicks  = 0;
  }
  else { /* Not started or already paused */ }
}


void LTimer::unpause(void)
{
  // If the timer is running and paused
  if( mStarted && mPaused )
  {
    // Unpause the timer
    mPaused = false;

    // Reset the starting ticks
    mStartTicks = SDL_GetTicks() - mPausedTicks;

    // Reset the paused ticks
    mPausedTicks = 0;
  }
  else { /* Not started or already running */ }
}


Uint32 LTimer::getTicks(void)
{
  // The actual timer time
  Uint32 time = 0;

  // If the timer is running
  if( mStarted )
  {
    // If the timer is paused
    if( mPaused )
    {
      // Return the number of ticks when the timer was paused
      time = mPausedTicks;
    }
    else
    {
      // Return the current time minus the start time
      time = SDL_GetTicks() - mStartTicks;
    }
  }
  else { /* Timer not running */ }

  return time;
}


bool LTimer::isStarted(void) const
{
  // Timer is running and paused or unpaused
  return mStarted;
}


bool LTimer::isPaused(void) const
{
  // Timer is running and paused
  return mPaused && mStarted;
}


/**
 * @brief Construct a new Dot:: Dot object
 **/
Dot::Dot(void)
{
  // Initialize the position
  mPosX = 0;
  mPosY = 0;

  // Initialize the velocity
  mVelX = 0;
  mVelY = 0;
}


/**
 * @brief Takes key presses and adjusts the dot's velocity.
 *
 * @param e SDL_Event to manage
 **/
void Dot::handleEvent( SDL_Event& e )
{
  // If a key was pressed
  if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
      // Adjust the velocity
      switch( e.key.keysym.sym )
      {
        case SDLK_UP: mVelY -= DOT_MAX_VEL_pxPerSec; break;
        case SDLK_DOWN: mVelY += DOT_MAX_VEL_pxPerSec; break;
        case SDLK_LEFT: mVelX -= DOT_MAX_VEL_pxPerSec; break;
        case SDLK_RIGHT: mVelX += DOT_MAX_VEL_pxPerSec; break;
      }
    }
    // If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
      // Adjust the velocity
      switch( e.key.keysym.sym )
      {
        case SDLK_UP: mVelY += DOT_MAX_VEL_pxPerSec; break;
        case SDLK_DOWN: mVelY -= DOT_MAX_VEL_pxPerSec; break;
        case SDLK_LEFT: mVelX += DOT_MAX_VEL_pxPerSec; break;
        case SDLK_RIGHT: mVelX -= DOT_MAX_VEL_pxPerSec; break;
      }
    }
}


/**
 * @brief Moves the dot based on a time step.
 *
 * @param timeStep
 **/
void Dot::move( double timeStep )
{
  // Move the dot left or right
  mPosX += mVelX * timeStep;

  // If the dot went too far to the left or right
  if( mPosX < 0 )
  {
    mPosX = 0;
  }
  else if( mPosX > WINDOW_W - DOT_WIDTH )
  {
    mPosX = WINDOW_W - DOT_WIDTH;
  }
  else { /* Movement was OK */ }

  // Move the dot up or down
  mPosY += mVelY * timeStep;

  // If the dot went too far up or down
  if( mPosY < 0 )
  {
    mPosY = 0;
  }
  else if( mPosY > WINDOW_H - DOT_HEIGHT )
  {
    mPosY = WINDOW_H - DOT_HEIGHT;
  }
  else { /* Movement was OK */ }
}


/**
 * @brief Shows the dot on the screen.
 **/
void Dot::render(void)
{
  // Show the dot
  gDotTexture.render( static_cast<int>(mPosX), static_cast<int>(mPosY) );
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
      gRenderer = SDL_CreateRenderer( gWindow, FIRST_ONE, SDL_RENDERER_ACCELERATED );

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

  // Load dot texture
  if( !gDotTexture.loadFromFile( Path ) )
  {
    printf( "\nFailed to load dot texture!" );
    success = false;
  }
	else
	{
		printf( "\nOK: dot texture created" );
	}

  return success;
}


static void close(void)
{
  // Free loaded images
  gDotTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow	 ( gWindow );
  gRenderer = NULL;
	gWindow   = NULL;

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
    printf( "\nFailed to initialise!" );
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

      // The dot that will be moving around on the screen
      Dot dot;

      // Keeps track of time between steps
      LTimer stepTimer;

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

          // Handle input for the dot
          dot.handleEvent( e );
        }

        // Calculate time step
        double timeStep = static_cast<double>(stepTimer.getTicks()) / MS_IN_S;

        // Move for time step
        dot.move( timeStep );

        // Restart step timer
        stepTimer.start();

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render dot
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