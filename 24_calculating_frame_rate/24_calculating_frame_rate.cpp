/**
 * @file 24_calculating_frame_rate.cpp
 *
 * @brief
 *
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, SDL_ttf, standard IO, strings, and string streams
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int INIT_FIRST_ONE_AVAILABLE = -1;

static constexpr int SCREEN_W = 640; // Screen's width
static constexpr int SCREEN_H = 480; // Screen's heigth

// Colore bianco
static constexpr int WHITE_R = 0xFF; // Amount of red   needed to compose white
static constexpr int WHITE_G = 0xFF; // Amount of green needed to compose white
static constexpr int WHITE_B = 0xFF; // Amount of blue  needed to compose white
static constexpr int WHITE_A = 0xFF; // Alpha component

// Colore nero
static constexpr int BLACK_R = 0x00; // Amount of red   needed to compose black
static constexpr int BLACK_G = 0x00; // Amount of green needed to compose black
static constexpr int BLACK_B = 0x00; // Amount of blue  needed to compose black
static constexpr int BLACK_A = 0xFF; // Alpha component

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan
static constexpr int CYAN_A = 0xFF; // Alpha component

static const std::string FontPath("lazy.ttf");


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
  int getWidth (void) const;
  int getHeight(void) const;

  private:
    // The actual hardware texture
    SDL_Texture* mTexture;

    // Image dimensions
    int mWidth;
    int mHeight;
};


// The application time based timer
class LTimer
{
    public:
    // Initializes variables
  LTimer(void);

    // The various clock actions
  void start(void);
  void stop(void);
  void pause(void);
  void unpause(void);

    // Gets the timer's time
  Uint32 getTicks(void);

    // Checks the status of the timer
  bool isStarted(void);
  bool isPaused(void);

    private:
    // The clock time when the timer started
    Uint32 mStartTicks;

    // The ticks stored when the timer was paused
    Uint32 mPausedTicks;

    // The timer status
  bool mIsPaused;
  bool mIsStarted;
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

static TTF_Font *gFont = NULL; // Globally used font

// Scene textures
static LTexture gFPSTextTexture;


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
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, CYAN_R, CYAN_G, CYAN_B ) );

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

    if( newTexture == NULL )
    {
      printf( "\nUnable to create texture from \"%s\"! SDL Error: %s", path.c_str(), SDL_GetError() );
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
    mWidth = 0;
    mHeight = 0;
  }
  else
  {;} // Nothing to destroy
}


void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
  // Modulate texture rgb
  SDL_SetTextureColorMod( mTexture, red, green, blue );
}


void LTexture::setBlendMode( SDL_BlendMode blending )
{
  // Set blending function
  SDL_SetTextureBlendMode( mTexture, blending );
}


void LTexture::setAlpha( Uint8 alpha )
{
  // Modulate texture alpha
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
  else
  {;}

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


LTimer::LTimer(void)
{
    // Initialize the variables
  mStartTicks  = (Uint32)0;
  mPausedTicks = (Uint32)0;

  mIsPaused  = false;
  mIsStarted = false;
}


/**
 * @brief Sets the "mIsStarted" and "mIsPaused" flags, gets the timer's start time and initializes
 * the pause time to 0. For this timer, if we want to restart it we just call start again. Since we
 * can start the timer if it is paused and/or running, we should make sure to clear out the paused
 * data.
 **/
void LTimer::start(void)
{
    // Start the timer
  mIsStarted = true;

    // Unpause the timer
  mIsPaused = false;

    // Get the current clock time
  mStartTicks  = static_cast<Uint32>(SDL_GetTicks64());
  mPausedTicks = (Uint32)0;
}


void LTimer::stop(void)
{
    // Stop the timer
  mIsStarted = false;

    // Unpause the timer
  mIsPaused = false;

  // Clear tick variables
  mStartTicks  = (Uint32)0;
  mPausedTicks = (Uint32)0;
}


void LTimer::pause(void)
{
    // If the timer is running and isn't already paused
  if( mIsStarted && !mIsPaused )
  {
        // Pause the timer
    mIsPaused = true;

        // Calculate the paused ticks
    mPausedTicks = static_cast<Uint32>(SDL_GetTicks64()) - mStartTicks;
    mStartTicks  = (Uint32)0;
  }
  else
  {;}
}


void LTimer::unpause(void)
{
    // If the timer is running and paused
  if( mIsStarted && mIsPaused )
  {
    // Unpause the timer
    mIsPaused = false;

    // Reset the starting ticks
    mStartTicks = static_cast<Uint32>(SDL_GetTicks64()) - mPausedTicks;

    // Reset the paused ticks
    mPausedTicks = (Uint32)0;
  }
  else
  {;}
}


Uint32 LTimer::getTicks(void)
{
  // The actual timer time
  Uint32 time = (Uint32)0;

    // If the timer is running
  if( mIsStarted )
  {
    // If the timer is paused
    if( mIsPaused )
    {
        // Return the number of ticks when the timer was paused
        time = mPausedTicks;
    }
    else
    {
      // Return the current time minus the start time
      time = static_cast<Uint32>(SDL_GetTicks64()) - mStartTicks;
    }
  }
  else
  {;}

  return time;
}


bool LTimer::isStarted(void)
{
  // Timer is running and paused or unpaused
  return mIsStarted;
}


bool LTimer::isPaused(void)
{
  // Timer is running and paused
  return mIsPaused && mIsStarted;
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
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: %s", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nWindow created" );

      // Create accelerated and vsynced renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, INIT_FIRST_ONE_AVAILABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

      if( gRenderer == NULL )
      {
        printf( "\nRenderer could not be created! SDL Error: %s", SDL_GetError() );
        success = false;
      }
      else
      {
        printf( "\nRenderer created" );

        // Initialize renderer color
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );

        // Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;

        if( !( IMG_Init( imgFlags ) & imgFlags ) )
        {
          printf( "\nSDL_image could not initialize! SDL_image Error: %s", IMG_GetError() );
          success = false;
        }
        else
        {
          printf( "\nSDL_image initialised" );
        }

         // Initialize SDL_ttf
        if( TTF_Init() == -1 )
        {
          printf( "\nSDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError() );
          success = false;
        }
        else
        {
          printf( "\nSDL_ttf initialised" );
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
  gFont = TTF_OpenFont( FontPath.c_str(), 28 );
  if( gFont == NULL )
  {
    printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
    success = false;
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gFPSTextTexture.free();

  // Free global font
  TTF_CloseFont( gFont );
  gFont = NULL;

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gRenderer = NULL;
  gWindow   = NULL;

  // Quit SDL subsystems
  TTF_Quit();
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
    printf( "\nAll systems initialised" );

    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!" );
    }
    else
    {
      printf( "\nAll media loaded" );

      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

      // Set text color as black
      SDL_Color textColor = { 0, 0, 0, 255 };

      // The frames per second timer
      LTimer fpsTimer;

      // In memory text stream
      std::stringstream timeText;

      // Start counting frames per second
      int countedFrames = 0;
      fpsTimer.start();

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
          {;}
        }

        // Calculate and correct fps
        double avgFPS = countedFrames / ( static_cast<double>(fpsTimer.getTicks()) / 1000.0 );

        if( avgFPS > 2000000 )
        {
          avgFPS = 0.0;
        }
        else
        {;}

        // Set text to be rendered
        timeText.str( "" );
        timeText << "Average Frames Per Second " << avgFPS;

        // Render text
        if( !gFPSTextTexture.loadFromRenderedText( timeText.str().c_str(), textColor ) )
        {
          printf( "Unable to render FPS texture!\n" );
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        // Render textures
        gFPSTextTexture.render( ( SCREEN_W - gFPSTextTexture.getWidth() ) / 2, ( SCREEN_H - gFPSTextTexture.getHeight() ) / 2 );

        // Update screen
        SDL_RenderPresent( gRenderer );
        ++countedFrames;
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