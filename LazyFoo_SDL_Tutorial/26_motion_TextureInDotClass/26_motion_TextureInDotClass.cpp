/**
 * @file 26_motion_TextureInDotClass.cpp
 *
 * @brief Come l'esempio 26, ma rendendo la classe Dot autonoma, cioè eliminando la texture globale
 * "gDotTexture" e trasformandola in un membro privato di Dot, "m_DotTexture". Il constructor di Dot
 * alloca la texture; di conseguenza, la funzione privata globale "LoadMedia" diventa superflua.
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


/***************************************************************************************************
* Classes
****************************************************************************************************/

// Texture wrapper class
class Texture
{
  public:
  // Initializes variables
  Texture(void);

  // Deallocates memory
  ~Texture(void);

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
  ~Dot(void);

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
  int m_PosX, m_PosY;

  // The velocity of the dot
  int m_VelX, m_VelY;

  // The texture used for the dot
  Texture m_DotTexture;

  // The texture's path
  std::string m_FilePath;
};


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static void close(void);
static void PressEnter(void);


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer



/***************************************************************************************************
* Methods definitions
****************************************************************************************************/

Texture::Texture(void)
{
  // Initialize
  mTexture = NULL;
  mWidth   = 0;
  mHeight  = 0;
}


Texture::~Texture(void)
{
  // Deallocate
  free();
}


bool Texture::loadFromFile( const std::string& path )
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
bool Texture::loadFromRenderedText( const std::string& textureText, SDL_Color textColor )
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


void Texture::free(void)
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


void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
  // Modulate texture rgb
  SDL_SetTextureColorMod( mTexture, red, green, blue );
}


void Texture::setBlendMode( SDL_BlendMode blending )
{
  // Set blending function
  SDL_SetTextureBlendMode( mTexture, blending );
}


void Texture::setAlpha( Uint8 alpha )
{
  // Modulate texture alpha
  SDL_SetTextureAlphaMod( mTexture, alpha );
}


void Texture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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


int Texture::getWidth(void) const
{
  return mWidth;
}


int Texture::getHeight(void) const
{
  return mHeight;
}


/**
 * @brief Initialises the offsets and the velocity
 **/
Dot::Dot(void)
  : m_PosX(0), m_PosY(0), m_VelX(0), m_VelY(0), m_FilePath("dot.bmp")
{
  m_DotTexture.loadFromFile( m_FilePath.c_str() );
}


Dot::~Dot(void)
{
  // Free loaded images
  m_DotTexture.free();
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
      case SDLK_UP   : m_VelY -= DOT_VEL; break;
      case SDLK_DOWN : m_VelY += DOT_VEL; break;
      case SDLK_LEFT : m_VelX -= DOT_VEL; break;
      case SDLK_RIGHT: m_VelX += DOT_VEL; break;
      default        :                   break;
    }
  }
  // If a key was released
  else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
  {
    // Adjust the velocity
    switch( e.key.keysym.sym )
    {
      case SDLK_UP   : m_VelY += DOT_VEL; break;
      case SDLK_DOWN : m_VelY -= DOT_VEL; break;
      case SDLK_LEFT : m_VelX += DOT_VEL; break;
      case SDLK_RIGHT: m_VelX -= DOT_VEL; break;
      default        :                   break;
    }
  }
  else
  {;}
}


void Dot::move(void)
{
  // Move the dot left or right
  m_PosX += m_VelX;

  // If the dot went too far to the left or right
  if( ( m_PosX < 0 ) || ( m_PosX + DOT_WIDTH > SCREEN_W ) )
  {
    // Move back
    m_PosX -= m_VelX;
  }
  else {;}

  // Move the dot up or down
  m_PosY += m_VelY;

  // If the dot went too far up or down
  if( ( m_PosY < 0 ) || ( m_PosY + DOT_HEIGHT > SCREEN_H ) )
  {
    // Move back
    m_PosY -= m_VelY;
  }
  else {;}
}


void Dot::render(void)
{
  // Show the dot
  m_DotTexture.render( m_PosX, m_PosY );
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
    if( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) == SDL_TRUE )
    {
      printf( "\nLinear texture filtering enabled" );
    }
    else
    {
      printf( "\nWarning: Linear texture filtering not enabled!" );
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


static void close(void)
{
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