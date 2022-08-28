/**
 * @file 35_window_events.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/35_window_events/index.php
 *
 * @brief SDL also supports resizable windows. When you have resizable windows there are additional
 * events to handle, which is what we'll be doing here.
 *
 * Sunto:
 * - Aggiunta una nuova classe "LWindow". Contiene dei dati relativi alla finestra (dimensioni,
 *   focus ecc.). Viene usata come oggetto globale.
 * - Il metodo "LWindow::init" viene chiamato sull'oggetto finestra "gWindow" nella funzione privata
 *   "init". Contiene una parte delle istruzioni che nei tutorial precedenti venivano interamente
 *   eseguite nella funzione privata "init". Ora, alcune di queste istruzioni sono migrate nella
 *   classe "LWindow".
 * - La finestra viene ora inizializzata con "SDL_WINDOW_RESIZABLE".
 * - L'autore ha scelto di far creare e inizializzare il renderer dall'oggetto "gWindow", nella
 *   funzione privata "init", mediante il metodo "createRenderer", sebbene questo renderer sia
 *   globale e gestito nel "main".
 * - La gestione degli eventi è delegata all'oggetto finestra, mediante chiamata del metodo
 *   "handleEvent".
 * - Una volta determinato che il tipo di evento è un "SDL_WINDOWEVENT", possiamo processarlo
 *   mediante uno switch-case su "e.window.event".
 * - Al termine del metodo "handleEvent", gestiamo l'eventuale aggiornamento della barra della
 *   finestra mediante "SDL_SetWindowTitle", e la dimensione della stessa mediante
 *   "SDL_SetWindowFullscreen".
 * - Il rendering viene gestito nella parte finale del "main", come sempre. Tuttavia, stavolta
 *   prestiamo attenzione a non renderizzare quando la finestre è minimizzata, perché ciò può
 *   provocare dei bachi.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/**************************************************************************************************
* Includes
***************************************************************************************************/

// Using SDL, SDL_image, standard I/O, strings, and string streams
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <sstream>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int FIRST_ONE = -1;

static constexpr int WINDOW_W = 640; // Screen's width
static constexpr int WINDOW_H = 480; // Screen's heigth

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

// Colore rosso
static constexpr int RED_R = 0xFF; // Amount of red   needed to compose red
static constexpr int RED_G = 0x00; // Amount of green needed to compose red
static constexpr int RED_B = 0x00; // Amount of blue  needed to compose red
static constexpr int RED_A = 0xFF; // Alpha component

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan
static constexpr int CYAN_A = 0xFF; // Alpha component

static const std::string TexPath("window.png");

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


class LWindow
{
  public:

  // Intializes internals
  LWindow(void);

  // Creates window
  bool init(void);

  // Creates renderer from internal window
  SDL_Renderer* createRenderer(void);

  // Handles window events
  void handleEvent( SDL_Event& );

  // Deallocates internals
  void free(void);

  // Window dimensions
  int getWidth(void) const;
  int getHeight(void) const;

  // Window focii
  bool hasMouseFocus(void) const;
  bool hasKeyboardFocus(void) const;
  bool isMinimized(void) const;

  private:

  // Window data
  SDL_Window* mWindow;

  // Window dimensions
  int mWidth;
  int mHeight;

  // Window focus
  bool mMouseFocus;
  bool mKeyboardFocus;
  bool mFullScreen;
  bool mMinimized;
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

static LWindow       gWindow;          // Our custom window
static SDL_Renderer* gRenderer = NULL; // The window renderer

// Scene textures
static LTexture gSceneTexture;


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
    printf( "\nOK: image \"%s\" loaded", path.c_str() );

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
      printf( "\nOK: texture created from \"%s\"", path.c_str() );

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
    mWidth = 0;
    mHeight = 0;
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


/**
 * @brief Initialize non-existant window
 **/
LWindow::LWindow(void)
  : mWindow(NULL),
    mWidth(0),
    mHeight(0),
    mMouseFocus(false),
    mKeyboardFocus(false),
    mFullScreen(false),
    mMinimized(false)
{;}


bool LWindow::init(void)
{
  // Create window
  mWindow = SDL_CreateWindow(  "SDL Tutorial",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                WINDOW_W, WINDOW_H,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );

  if( mWindow != NULL )
  {
    mMouseFocus     = true;
    mKeyboardFocus  = true;
    mWidth          = WINDOW_W;
    mHeight         = WINDOW_H;
    printf("\nOK: window initialised");
  }
  else
  {
    printf("\nFailed to initialise window!");
  }

  return mWindow != NULL;
}


SDL_Renderer* LWindow::createRenderer(void)
{
  return SDL_CreateRenderer( mWindow, FIRST_ONE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
}


void LWindow::handleEvent( SDL_Event& e )
{
  // Window event occured
  if( e.type == SDL_WINDOWEVENT )
  {
    // Caption update flag
    bool updateCaption = false;

    switch( e.window.event )
    {
      // Get new dimensions and repaint on window size change
      case SDL_WINDOWEVENT_SIZE_CHANGED:
        mWidth  = e.window.data1;
        mHeight = e.window.data2;
        SDL_RenderPresent( gRenderer );
        break;

      // Repaint on exposure
      case SDL_WINDOWEVENT_EXPOSED:
        SDL_RenderPresent( gRenderer );
        break;

      // Mouse entered window
      case SDL_WINDOWEVENT_ENTER:
        mMouseFocus   = true;
        updateCaption = true;
        break;

      // Mouse left window
      case SDL_WINDOWEVENT_LEAVE:
        mMouseFocus   = false;
        updateCaption = true;
        break;

      // Window has keyboard focus
      case SDL_WINDOWEVENT_FOCUS_GAINED:
        mKeyboardFocus = true;
        updateCaption  = true;
        break;

      // Window lost keyboard focus
      case SDL_WINDOWEVENT_FOCUS_LOST:
        mKeyboardFocus = false;
        updateCaption  = true;
        break;

      // Window minimized
      case SDL_WINDOWEVENT_MINIMIZED:
        mMinimized = true;
        break;

      // Window maximized
      case SDL_WINDOWEVENT_MAXIMIZED:
        mMinimized = false;
        break;

      // Window restored
      case SDL_WINDOWEVENT_RESTORED:
        mMinimized = false;
        break;
    }

    // Update window caption with new data
    if( updateCaption )
    {
      std::stringstream caption;
      caption << "SDL Tutorial - MouseFocus:" << ( ( mMouseFocus )    ? "On" : "Off" )
              << " KeyboardFocus:"            << ( ( mKeyboardFocus ) ? "On" : "Off" );
      SDL_SetWindowTitle( mWindow, caption.str().c_str() );
    }
    else { /* No need to update window's caption */ }

  }
  // Enter or exit full screen on return key
  else if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN )
  {
    if( mFullScreen )
    {
      SDL_SetWindowFullscreen( mWindow, SDL_FALSE );
      mFullScreen = false;
    }
    else
    {
      // SDL_SetWindowFullscreen( mWindow, SDL_TRUE ); // Istruzione originale
      SDL_SetWindowFullscreen( mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP ); // Borderless full screen (non c'è cambio di video mode)
      mFullScreen = true;
      mMinimized  = false;
    }
  }
  else { /* Event not recognised / not managed here */ }
}


void LWindow::free(void)
{
  if( mWindow != NULL )
  {
    SDL_DestroyWindow( mWindow );
  }

  mMouseFocus = false;
  mKeyboardFocus = false;
  mWidth = 0;
  mHeight = 0;
}


int LWindow::getWidth(void) const
{
  return mWidth;
}


int LWindow::getHeight(void) const
{
  return mHeight;
}


bool LWindow::hasMouseFocus(void) const
{
  return mMouseFocus;
}


bool LWindow::hasKeyboardFocus(void) const
{
  return mKeyboardFocus;
}


bool LWindow::isMinimized(void) const
{
  return mMinimized;
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
    if( !gWindow.init() )
    {
      printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nOK: window created" );

      // Create renderer for window
      gRenderer = gWindow.createRenderer();

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
          printf( "\nSDL_image could not initialize! SDL_image Error: %s", IMG_GetError() );
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
static bool loadMedia(void)
{
  // Loading success flag
  bool success = true;

  // Load scene texture
  if( !gSceneTexture.loadFromFile( TexPath ) )
  {
    printf( "\nFailed to load window texture!" );
    success = false;
  }
  else { /* All OK */ }

  return success;
}


static void close(void)
{
  // Free loaded images
  gSceneTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  gWindow.free();

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
          else
          { /* Continue until QUIT event */ }

          // Handle window events
          gWindow.handleEvent( e );
        }

        if( !gWindow.isMinimized() )
        {
          // Clear screen
          SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
          SDL_RenderClear( gRenderer );

          // Render text textures
          gSceneTexture.render( ( gWindow.getWidth()  - gSceneTexture.getWidth()  ) / 2,
                                ( gWindow.getHeight() - gSceneTexture.getHeight() ) / 2 );

          // Update screen
          SDL_RenderPresent( gRenderer );
        }
        else { /* Only draw when not minimised */ }
      }

    } // All media loaded

  } // All systems initialised


  close(); // Free resources and close SDL

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