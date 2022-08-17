/**
 * @file 18_key_states.cpp
 *
 * @brief As we saw in the mouse input tutorial, there are ways to get the state of the input
 * devices (mouse, keyboard, etc) other than using events. In this tutorial, we'll be remaking the
 * keyboard input tutorial using key states instead of events.
 *
 * Right before we enter the main loop, we declare a texture pointer ("currentTexture") to keep
 * track of which texture we're rendering to the screen.
 *
 * As you can see, we aren't checking for key events in the event loop. All our keyboard input is
 * going to be handled with key states.
 *
 * One important thing to know about how SDL handles key states is that you still need an event loop
 * running. SDL's internal keystates are updated every time SDL_PollEvent is called, so make sure
 * you polled all events on queue before checking key states.
 * 
 * In the main loop, first we get a pointer to the array of key states using SDL_GetKeyboardState.
 * The state of all the keys are ordered by SDL_Scancode. Scan codes are like the SDL_Keycode
 * values, only scan codes are designed to work with international keyboards. Depending on the
 * keyboard layout, different letters might be in different places. Scan codes go off default
 * physical position of the keys as opposed to where they might be on a specific keyboard.
 * 
 * All you have to do to check if a key is down is to check its state in the key state array. If the
 * key is down we set the current texture to the corresponding texture. If none of the keys are
 * down, we set the default texture.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
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

static constexpr int INIT_FIRST_ONE_AVAILABLE = -1;

static constexpr int SCREEN_W = 640; // Screen's width
static constexpr int SCREEN_H = 480; // Screen's heigth

// Colore bianco
static constexpr int WHITE_R = 0xFF; // Amount of red   needed to compose white
static constexpr int WHITE_G = 0xFF; // Amount of green needed to compose white
static constexpr int WHITE_B = 0xFF; // Amount of blue  needed to compose white
static constexpr int WHITE_A = 0xFF; // Alpha component

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan

static const std::string PressPath("Press.png");
static const std::string UpPath("Up.png");
static const std::string DownPath("Down.png");
static const std::string LeftPath("Left.png");
static const std::string RightPath("Right.png");


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
    bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
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

// Scene textures
LTexture gPressTexture;
LTexture gUpTexture;
LTexture gDownTexture;
LTexture gLeftTexture;
LTexture gRightTexture;


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
      printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
    }
    else
    {
      // Get image dimensions
      mWidth = textSurface->w;
      mHeight = textSurface->h;
    }

    // Get rid of old surface
    SDL_FreeSurface( textSurface );
  }
  else
  {
    printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
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
  {;}
}


void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
  // Modulate texture's RGB
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


/**
 * @brief Renders texture at given (x, y) point of the target window. Accepts a rectangle defining
 * which portion of the texture we want to render. Default argument of NULL to render the whole
 * texture.
 *
 * @param x x coordinate of the destination rendering point.
 * @param y y coordinate of the destination rendering point.
 * @param SourceClip Rectangle defining which portion of the texture we want to render. Defaults to
 * NULL (to render the whole texture).
 **/
void LTexture::render( int x, int y, SDL_Rect* SourceClip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
  // Set rendering space and render texture to screen
  SDL_Rect Destination = { x, y, mWidth, mHeight };

  // Set source clip's rendering dimensions
  if( SourceClip != NULL )
  {
    Destination.w = SourceClip->w;
    Destination.h = SourceClip->h;
  }
  else
  {;}

  // Render to screen
  SDL_RenderCopyEx( gRenderer, mTexture, SourceClip, &Destination, angle, center, flip );
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

  // Load press texture
  if( !gPressTexture.loadFromFile( PressPath ) )
  {
    printf( "Failed to load press texture!\n" );
    success = false;
  }

  // Load up texture
  if( !gUpTexture.loadFromFile( UpPath ) )
  {
    printf( "Failed to load up texture!\n" );
    success = false;
  }

  // Load down texture
  if( !gDownTexture.loadFromFile( DownPath ) )
  {
    printf( "Failed to load down texture!\n" );
    success = false;
  }

  // Load left texture
  if( !gLeftTexture.loadFromFile( LeftPath ) )
  {
    printf( "Failed to load left texture!\n" );
    success = false;
  }

  // Load right texture
  if( !gRightTexture.loadFromFile( RightPath ) )
  {
    printf( "Failed to load right texture!\n" );
    success = false;
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gPressTexture.free();
  gUpTexture.free();
  gDownTexture.free();
  gLeftTexture.free();
  gRightTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
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

  // Start up SDL and create window
  if( !init() )
  {
    printf( "\nFailed to initialize!" );
  }
  else
  {
    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!" );
    }
    else
    {
      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

      // Current rendered texture
      LTexture* currentTexture = NULL;

      // While application is running
      while( !quit )
      {
        // Handle events on queue. IMPORTANTE: l'event loop è richiesto anche se usiamo gli stati anziché gli eventi!
        while( SDL_PollEvent( &e ) != 0 )
        {
          // User requests quit
          if( e.type == SDL_QUIT )
          {
            quit = true;
          }
          else
          {;} // No events to process
        }

        // Set texture based on current keystate
        const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );

        if( currentKeyStates[ SDL_SCANCODE_UP ] )
        {
          currentTexture = &gUpTexture;
        }
        else if( currentKeyStates[ SDL_SCANCODE_DOWN ] )
        {
          currentTexture = &gDownTexture;
        }
        else if( currentKeyStates[ SDL_SCANCODE_LEFT ] )
        {
          currentTexture = &gLeftTexture;
        }
        else if( currentKeyStates[ SDL_SCANCODE_RIGHT ] )
        {
          currentTexture = &gRightTexture;
        }
        else
        {
          currentTexture = &gPressTexture;
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render current texture
        currentTexture->render( 0, 0 );

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