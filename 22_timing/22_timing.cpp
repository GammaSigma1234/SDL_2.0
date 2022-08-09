/**
 * @file 22_timing.cpp
 *
 * @brief Another important part of any sort of gaming API is the ability to handle time. In this
 * tutorial we'll make a timer we can restart.
 *
 * For this tutorial we'll be using string streams, and have to include the "sstream" header, which
 * should come standard with your C++ compiler.
 *
 * As mentioned in the font rendering tutorial, you want to minimize the amount of times you render
 * text. We'll have a texture to prompt input and a texture to display the current time in
 * milliseconds. The time texture changes every frame so we have to render that every frame, but the
 * prompt texture doesn't change so we can render it once in the file loading function.
 *
 * Before we enter the main loop, we want to declare some variables. The two we want to pay
 * attention to are the "startTime" variable (which is an unsigned 32-bits integer) and the
 * "timeText" variable, which is a string stream.
 *
 * For those of you who have never used string streams, just know that they function like iostreams,
 * only instead of reading or writing to the console, they allow you to read and write to a string
 * in memory. It'll be easier to see when we see them used further on in the program.
 *
 * There's a function called SDL_GetTicks which returns the time since the program started in
 * milliseconds. For this demo, we'll be having a timer that restarts every time we press the return
 * key.
 *
 * Remember how we initialized the start time to 0 at the start of the program? This means the
 * timer's time is just the current time since the program started returned by SDL_GetTicks. If we
 * were to restart the timer when SDL_GetTicks was at 5000 milliseconds (5 seconds), then at 10,000
 * milliseconds the current time - the start time would be 10000 minus 5000 would be 5000
 * milliseconds. So, even though the timer contained by SDL_GetTicks hasn't restarted, we can have a
 * timer keep track of a relative start time and reset its start time.
 *
 * string stream: First we call "str" with an empty string to initialize it to be empty. Then we
 * treat it like cout and print to it "Milliseconds since start", and the current time minus the
 * relative start time, so it will print the time since we last started the timer. Now that we have
 * the time in a string stream, we can get a string from it and use it to render the current time to
 * a texture. Finally we render the prompt texture and the time texture to the screen.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022) and may not be
 * redistributed without written permission.
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

// static const std::string FontPath("lazy.ttf");
static const std::string FontPath("RachelBrown.ttf");


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
    bool loadFromRenderedText( const std::string& textureText, SDL_Color textColor );
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
static LTexture gTimeTextTexture;
static LTexture gPromptTextTexture;


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
    printf( "\nFailed to load font! SDL_ttf Error: %s", TTF_GetError() );
    success = false;
  }
  else
  {
    printf( "\nFont loaded" );

    // Set text color as black
    SDL_Color textColor = { BLACK_R, BLACK_G, BLACK_B, BLACK_A };

    // Load prompt texture
    if( !gPromptTextTexture.loadFromRenderedText( "Press Enter to Reset Start Time.", textColor ) )
    {
      printf( "\nUnable to render prompt texture!" );
      success = false;
    }
    else
    {
      printf( "\nPrompt texture rendered" );
    }
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gTimeTextTexture.free();
  gPromptTextTexture.free();

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

      // Set text color as black
      SDL_Color textColor = { BLACK_R, BLACK_G, BLACK_B, BLACK_A };

      // Current time start time
      Uint32 startTime = 0;

      // In memory text stream
      std::stringstream timeText;

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
          // Reset start time on return keypress
          else if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN )
          {
            startTime = SDL_GetTicks();
          }
          else
          {;} // Unrecognised event
        }

        // Set text to be rendered
        timeText.str( "" ); // Svuota lo stringstream prima di riempirlo con la nuova stringa da renderizzare
        timeText << "Milliseconds since start: " << SDL_GetTicks64() - startTime;

        // Render text
        if( !gTimeTextTexture.loadFromRenderedText( timeText.str().c_str(), textColor ) )
        {
          printf( "\nUnable to render time texture!" );
        }
        else
        {;} // Time texture rendered

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render textures
        int CENTERED_HORIZONTALLY = ( SCREEN_W - gPromptTextTexture.getWidth()  ) / 2;
        int CENTERED_VERTICALLY   = ( SCREEN_H - gPromptTextTexture.getHeight() ) / 2;
        gPromptTextTexture.render( CENTERED_HORIZONTALLY, 0 );
        gTimeTextTexture.render  ( CENTERED_HORIZONTALLY, CENTERED_VERTICALLY );

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