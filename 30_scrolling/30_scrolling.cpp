/**
 * @file 30_scrolling.cpp
 *
 * @brief
 *
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

// Using SDL, SDL_image, standard IO, vectors, and strings
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

// The dimensions of the level
static constexpr int LEVEL_W = 1280;
static constexpr int LEVEL_H = 960;

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

// Colore nero
static constexpr int BLACK_R = 0x00; // Amount of red   needed to compose black
static constexpr int BLACK_G = 0x00; // Amount of green needed to compose black
static constexpr int BLACK_B = 0x00; // Amount of blue  needed to compose black
static constexpr int BLACK_A = 0xFF; // Alpha component

// Muro
static constexpr int WALL_x = 300;
static constexpr int WALL_y = 40;
static constexpr int WALL_w = 40;
static constexpr int WALL_h = 400;

static const std::string DotPath("dot.bmp");
static const std::string BackGroundPath("bg.png");


/***************************************************************************************************
* Classes
****************************************************************************************************/

// A circle stucture
struct Circle
{
  int x, y;
  int r;
};


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


/**
 * @brief The dot that will move around on the screen.
 **/
class Dot
{
    public:

    // The dimensions of the dot
  static constexpr int DOT_WIDTH  = 20;
  static constexpr int DOT_HEIGHT = 20;

    // Initializes the variables
    Dot(void);

    // Takes key presses and adjusts the dot's velocity
  void handleEvent( SDL_Event& );

    // Moves the dot
    void move(void);

    // Shows the dot on the screen relative to the camera
    void render( int, int );

    // Position accessors
    int getPosX(void) const;
    int getPosY(void) const;

    private:

    // Maximum axis velocity of the dot
    static constexpr int DOT_VEL = 10;

    // The X and Y offsets of the dot
    int mPosX, mPosY;

    // The velocity of the dot
    int mVelX, mVelY;
};


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init      ( void );
static bool loadMedia ( void );
static void close     ( void );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer

// Scene textures
static LTexture gDotTexture;
static LTexture gBGTexture;


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
  else {;} // Nothing to destroy
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


Dot::Dot(void)
{
    // Initialize the offsets
    mPosX = 0;
    mPosY = 0;

    // Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}


void Dot::handleEvent( SDL_Event& e )
{
  // If a key was pressed (e.key.repeat == 0 serve per ignorare la ripetizione automatica in caso di
  // tasto tenuto premuto)
  if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
  {
    // Adjust the velocity
    switch( e.key.keysym.sym )
    {
      case SDLK_UP   : mVelY -= DOT_VEL; break;
      case SDLK_DOWN : mVelY += DOT_VEL; break;
      case SDLK_LEFT : mVelX -= DOT_VEL; break;
      case SDLK_RIGHT: mVelX += DOT_VEL; break;
      default        :                   break;
    }
  }
  // If a key was released
  else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
  {
    // Adjust the velocity
    switch( e.key.keysym.sym )
    {
      case SDLK_UP   : mVelY += DOT_VEL; break;
      case SDLK_DOWN : mVelY -= DOT_VEL; break;
      case SDLK_LEFT : mVelX += DOT_VEL; break;
      case SDLK_RIGHT: mVelX -= DOT_VEL; break;
      default        :                   break;
    }
  }
  else { /* Ignore this event */ }
}


void Dot::move(void)
{
    // Move the dot left or right
    mPosX += mVelX;

    // If the dot went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > LEVEL_W ) )
    {
        // Move back
        mPosX -= mVelX;
    }

    // Move the dot up or down
    mPosY += mVelY;

    // If the dot went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > LEVEL_H ) )
    {
        // Move back
        mPosY -= mVelY;
    }
}

void Dot::render( int camX, int camY )
{
    // Show the dot relative to the camera
  gDotTexture.render( mPosX - camX, mPosY - camY );
}


int Dot::getPosX(void) const
{
  return mPosX;
}


int Dot::getPosY(void) const
{
  return mPosY;
}


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
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
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

  // Load dot texture
  if ( !gDotTexture.loadFromFile( DotPath.c_str() ) )
  {
    printf( "\nFailed to load dot texture!" );
    success = false;
  }
  else
  {
    printf( "\nDot texture loaded" );
  }

  // Load background texture
  if ( !gBGTexture.loadFromFile( BackGroundPath.c_str() ) )
  {
    printf( "\nFailed to load background texture!\n" );
    success = false;
  }
  else
  {
    printf( "\nBackground texture loaded" );
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gDotTexture.free();
  gBGTexture.free();

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

      // The dot that will be moving around on the screen
      Dot dot;

      // The camera area
      SDL_Rect camera = { 0, 0, SCREEN_W, SCREEN_H };

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
          else { /* ignore event */ }

          // Handle input for the dot
          dot.handleEvent( e );
        }

        // Move the dot
        dot.move();

        // Center the camera over the dot
        camera.x = ( dot.getPosX() + Dot::DOT_WIDTH  / 2 ) - SCREEN_W / 2;
        camera.y = ( dot.getPosY() + Dot::DOT_HEIGHT / 2 ) - SCREEN_H / 2;

        // Keep the camera in bounds
        if( camera.x < 0 )
        {
          camera.x = 0;
        }
        if( camera.y < 0 )
        {
          camera.y = 0;
        }
        if( camera.x > LEVEL_W - camera.w )
        {
          camera.x = LEVEL_W - camera.w;
        }
        if( camera.y > LEVEL_H - camera.h )
        {
          camera.y = LEVEL_H - camera.h;
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render background
        gBGTexture.render( 0, 0, &camera );

        // Render objects
        dot.render( camera.x, camera.y );

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