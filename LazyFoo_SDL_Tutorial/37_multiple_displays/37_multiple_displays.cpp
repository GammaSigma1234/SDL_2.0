/**
 * @file 37_multiple_displays.cpp
 *
 * @brief
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 *
 * Sunto:
 * - Our LWindow from previous tutorials has been updated with a window display ID to keep track of
 *   which display the window is on.
 * - Our displays all have an integer ID and a rectangle associated with them so we know the
 *   position and dimensions of each display on our desktop.
 * - Il metodo init è simile al precedente: crea la finestra e il renderer. L'indice del display si
 *   ottiene mediante "SDL_GetWindowDisplayIndex".
 * - Se i display multipli sono disponibili, i tasti freccia SU e GIU permettono di ciclarli. La
 *   funzione "SDL_SetWindowPosition" sposta la finestra al centro del display.
 * - Nell'init privato, "SDL_GetNumVideoDisplays" determina il numero di display disponibili.
 * - Now that we know how many displays are connected, we allocate rectangles for each of them and
 *   get the bounds for each one using "SDL_GetDisplayBounds". After this, we initialize our window.
 * - Il main loop è quasi identico al precedente, perché tutte le variazioni sono state incapsulate
 *   nella classe LWindow.
 **/


/**************************************************************************************************
* Includes
***************************************************************************************************/

// Using SDL, standard I/O, strings, and string streams
#include <SDL.h>
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

// Total windows
static constexpr int TOTAL_WINDOWS = 3;

/***************************************************************************************************
* Classes
****************************************************************************************************/


class LWindow
{
  public:

  // Intializes internals
  LWindow(void);

  // Creates window
  bool init(void);

  // Handles window events
  void handleEvent( SDL_Event& );

  // Focuses on window
  void focus(void);

  // Shows windows contents
  void render(void);

  // Deallocates internals
  void free(void);

  // Window dimensions
  int getWidth(void) const;
  int getHeight(void) const;

    // Window focii
  bool hasMouseFocus(void) const;
  bool hasKeyboardFocus(void) const;
  bool isMinimized(void) const;
  bool isShown(void) const;

  private:

  // Window data
  SDL_Window*   mWindow;
  SDL_Renderer* mRenderer;
  Uint32        mWindowID;
  int           mWindowDisplayID;

  // Window dimensions
  int mWidth;
  int mHeight;

  // Window focus
  bool mMouseFocus;
  bool mKeyboardFocus;
  bool mFullScreen;
  bool mMinimized;
  bool mShown;
};


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init (void);
static void close(void);


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

LWindow gWindow; // Our custom window

// Display data
int       gTotalDisplays = 0;
SDL_Rect* gDisplayBounds = NULL;


/***************************************************************************************************
* Methods definitions
****************************************************************************************************/

/**
 * @brief Initialize non-existant window
 **/
LWindow::LWindow(void)
  : mWindow(NULL),
    mRenderer(NULL),
    mWindowID(-1),
    mWindowDisplayID(-1),
    mWidth(0),
    mHeight(0),
    mMouseFocus(false),
    mKeyboardFocus(false),
    mFullScreen(false),
    mMinimized(false),
    mShown(false)
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

    // Create renderer for window
    mRenderer = SDL_CreateRenderer( mWindow, FIRST_ONE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

    if( mRenderer == NULL )
    {
      printf( "\nRenderer could not be created! SDL Error: %s", SDL_GetError() );
      SDL_DestroyWindow( mWindow );
      mWindow = NULL;
    }
    else
    {
      printf( "\nOK: renderer created" );

      // Initialize renderer color
      SDL_SetRenderDrawColor( mRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );

      // Grab window identifier
      mWindowID        = SDL_GetWindowID( mWindow );
      mWindowDisplayID = SDL_GetWindowDisplayIndex( mWindow );

      // Flag as opened
      mShown = true;
    }
  }
  else
  {
    printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
  }

  return mWindow != NULL && mRenderer != NULL;
}


void LWindow::handleEvent( SDL_Event& e )
{
  // Caption update flag
  bool updateCaption = false;

  // If an event was detected for this window
  if( e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID )
  {
    switch( e.window.event )
    {
      // Window moved
      case SDL_WINDOWEVENT_MOVED:
      mWindowDisplayID = SDL_GetWindowDisplayIndex( mWindow );
      updateCaption = true;
      break;

      // Window appeared
      case SDL_WINDOWEVENT_SHOWN:
      mShown = true;
      break;

      // Window disappeared
      case SDL_WINDOWEVENT_HIDDEN:
      mShown = false;
      break;

      // Get new dimensions and repaint
      case SDL_WINDOWEVENT_SIZE_CHANGED:
      mWidth  = e.window.data1;
      mHeight = e.window.data2;
      SDL_RenderPresent( mRenderer );
      break;

      // Repaint on expose
      case SDL_WINDOWEVENT_EXPOSED:
      SDL_RenderPresent( mRenderer );
      break;

      // Mouse enter
      case SDL_WINDOWEVENT_ENTER:
      mMouseFocus = true;
      updateCaption = true;
      break;

      // Mouse exit
      case SDL_WINDOWEVENT_LEAVE:
      mMouseFocus = false;
      updateCaption = true;
      break;

      // Keyboard focus gained
      case SDL_WINDOWEVENT_FOCUS_GAINED:
      mKeyboardFocus = true;
      updateCaption = true;
      break;

      // Keyboard focus lost
      case SDL_WINDOWEVENT_FOCUS_LOST:
      mKeyboardFocus = false;
      updateCaption = true;
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

      // Hide on close
      case SDL_WINDOWEVENT_CLOSE:
      SDL_HideWindow( mWindow );
      break;
    }
  }
  else if( e.type == SDL_KEYDOWN )
  {
    // Display change flag
    bool switchDisplay = false;

    // Cycle through displays on up/down
    switch( e.key.keysym.sym )
    {
      case SDLK_UP:
      ++mWindowDisplayID;
      switchDisplay = true;
      break;

      case SDLK_DOWN:
      --mWindowDisplayID;
      switchDisplay = true;
      break;
    }

    // Display needs to be updated
    if( switchDisplay )
    {
      // Bound display index
      if( mWindowDisplayID < 0 )
      {
        mWindowDisplayID = gTotalDisplays - 1;
      }
      else if( mWindowDisplayID >= gTotalDisplays )
      {
        mWindowDisplayID = 0;
      }

      // Move window to center of next display
      SDL_SetWindowPosition( mWindow, gDisplayBounds[ mWindowDisplayID ].x + ( gDisplayBounds[ mWindowDisplayID ].w - mWidth  ) / 2,
                                      gDisplayBounds[ mWindowDisplayID ].y + ( gDisplayBounds[ mWindowDisplayID ].h - mHeight ) / 2 );
      updateCaption = true;
    }
  }


  // Update window caption with new data
  if( updateCaption )
  {
    std::stringstream caption;
    caption << "SDL Tutorial - ID: " << mWindowID << " Display: " << mWindowDisplayID
            << " MouseFocus:"    << ( ( mMouseFocus )    ? "On" : "Off" )
            << " KeyboardFocus:" << ( ( mKeyboardFocus ) ? "On" : "Off" );
    SDL_SetWindowTitle( mWindow, caption.str().c_str() );
  }
  else { /* No need to update window's caption */ }
}


void LWindow::focus(void)
{
  // Restore window if needed
  if( !mShown )
  {
    SDL_ShowWindow( mWindow );
  }
  else
  { /* Window is already shown */ }

  // Move window forward
  SDL_RaiseWindow( mWindow );
}


void LWindow::render(void)
{
  if( !mMinimized )
  {
    // Clear screen
    SDL_SetRenderDrawColor( mRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
    SDL_RenderClear( mRenderer );

    // Update screen
    SDL_RenderPresent( mRenderer );
  }
  else { /* Window is minimised: do not render it */ }
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


bool LWindow::isShown(void) const
{
  return mShown;
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
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" ) )
    {
      printf( "\nWarning: linear texture filtering not enabled!" );
      success = false;
    }
    else
    {
      printf( "\nOK: linear texture filtering enabled" );
    }

    // Get number of displays
    gTotalDisplays = SDL_GetNumVideoDisplays();

    if( gTotalDisplays < 2 )
    {
      printf( "\nWarning: only one display connected!" );
    }
    else
    {
      printf( "\nOK: multiple displays connected" );
    }

    // Get bounds of each display
    gDisplayBounds = new SDL_Rect[ gTotalDisplays ];

    for( int i = 0; i != gTotalDisplays; ++i )
    {
      SDL_GetDisplayBounds( i, &gDisplayBounds[ i ] );
    }

    // Create window
    if( !gWindow.init() )
    {
      printf( "\nWindow could not be created!\n" );
      success = false;
    }
    else
    {
      printf( "\nOK: window created" );
    }
  }

  return success;
}



static void close(void)
{
  // Destroy window
  gWindow.free();

  // Deallocate bounds
  delete[] gDisplayBounds;
  gDisplayBounds = NULL;

  // Quit SDL subsystems
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

      // Update window
      gWindow.render();
    }
  }

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