/**
 * @file 26_motion.cpp
 *
 * @brief Now that we know how to render, handle input, and deal with time, we know everything we
 * need to move around things on the screen. Here we will do a basic program with a dot moving
 * around.
 *
 * "Dot" is the class for the dot we're going to be moving around on the screen. It has some
 * constants to define its dimensions and velocity. It has a constructor, an event handler, a
 * function to move it every frame, and a function to render it. As for data members, it has
 * variables for its (x, y) position and (x, y) velocity.
 *
 * In our event handler we're going to set the velocity based on the key press.
 *
 * You may be wondering why we don't simply just increase the positon when we press the key. If we
 * were to say add to the x position every time we press the right key, we would have to repeatedly
 * press the right key to keep it moving. By setting the velocity, we just have to press the key
 * once.
 *
 * If you're wondering why we're checking if the key repeat is 0, it's because key repeat is enabled
 * by default and if you press and hold a key it will report multiple key presses. That means we
 * have to check if the key press is the first one because we only care when the key was first
 * pressed.
 *
 * For those of you who haven't studied physics yet, velocity is the speed/direction of an object.
 * If an object is moving right at 10 pixels per frame, it has a velocity of 10. If it is moving to
 * the left at 10 pixel per frame, it has a velocity of -10. If the dot's velocity is 10, this means
 * after 10 frames it will have moved 100 pixels over.
 *
 * When we release a key, we have to undo the velocity change when first pressed it. When we pressed
 * right key, we added to the x velocity. When we release the right key, we subtract from the x
 * velocity to return it to 0.
 *
 * "move" is the function we call every frame to move the dot. First, we move the dot along the x
 * axis based on its x velocity. After that, we check if the dot moved off the screen. If it did, we
 * then undo the movement along the x axis. Same for the y axis.
 *
 * In the rendering function ("render"), we render the dot texture at the dot's position.
 *
 * Before we enter the main loop we declare a dot object, to use it in the main loop. In the event
 * loop we handle events for the dot. After that, we update the dot's position and then render it to
 * the screen.
 *
 * In this tutorial we're basing the velocity as amount moved per frame. In most games, the velocity
 * is done per second. The reason were doing it per frame is that it is easier, but if you know
 * physics it shouldn't be hard to update the dot's position based on time. If you don't know
 * physics, just stick with per frame velocity for now.
 *
 * ATTENZIONE: l'autore ha preferito creare una texture globale "gDotTexture" anziché renderla un
 * membro privato della classe "Dot". Infatti, il metodo Dot::render chiama a sua volta il metodo
 * LTexture::render sull'oggetto globale "gDotTexture", che idealmente dovrebbe essere un membro
 * della classe Dot, anziché un entità globale.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

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
static constexpr int CYAN_A = 0xFF; // Alpha component

static const std::string FilePath("dot.bmp");


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
  Uint32 getTicks(void) const;

  // Checks the status of the timer
  bool isStarted(void) const;
  bool isPaused(void)  const;

  private:

  // The clock time when the timer started
  Uint32 mStartTicks;

  // The ticks stored when the timer was paused
  Uint32 mPausedTicks;

  // The timer status
  bool mIsPaused;
  bool mIsStarted;
};


// The dot that will move around on the screen
class Dot
{
  public:

  // Initializes the variables
  Dot(void);

  // Takes key presses and adjusts the dot's velocity
  void handleEvent( SDL_Event& );

  // Moves the dot
  void move(void);

  // Shows the dot on the screen
  void render(void);

  private:

  // The dimensions of the dot
  static constexpr int DOT_WIDTH  = 20;
  static constexpr int DOT_HEIGHT = 20;

  // Maximum axis velocity of the dot
  static constexpr int DOT_VEL = 5;

  // The X and Y offsets of the dot
  int mPosX, mPosY;

  // The velocity of the dot
  int mVelX, mVelY;
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
  else
  {;}
}


void Dot::move(void)
{
  // Move the dot left or right
  mPosX += mVelX;

  // If the dot went too far to the left or right
  if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_W ) )
  {
    // Move back
    mPosX -= mVelX;
  }
  else {;}

  // Move the dot up or down
  mPosY += mVelY;

  // If the dot went too far up or down
  if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_H ) )
  {
    // Move back
    mPosY -= mVelY;
  }
  else {;}
}


void Dot::render(void)
{
  // Show the dot
  gDotTexture.render( mPosX, mPosY );
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

  // Load dot texture
  if( !gDotTexture.loadFromFile( FilePath.c_str() ) )
  {
    printf( "Failed to load dot texture!\n" );
    success = false;
  }
  else { /* OK */ }

  return success;
}


static void close(void)
{
  // Free loaded images
  gDotTexture.free();

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
          else { /* Ignore event */ }

          // Handle input for the dot
          dot.handleEvent( e );
        }

        // Move the dot
        dot.move();

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render objects
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