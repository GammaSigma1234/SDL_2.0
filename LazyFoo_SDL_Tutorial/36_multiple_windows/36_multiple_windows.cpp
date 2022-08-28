/**
 * @file 36_multiple_windows.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/36_multiple_windows/index.php
 *
 * @brief One of the new features SDL 2 has is being able to handle multiple windows at once. In
 * this tutorial we'll be moving around 3 resizable windows.
 * 
 * In this demo we did not actually render anything inside of the windows. This would involve having
 * to manage renderers and windows and having them share resources. There is no right way to do
 * this, and the best way depends entirely on what type of application you're building. I recommend
 * reading through the SDL documentation to understand how renderers work and then experimenting to
 * figure out the best way for you to manage your resources.
 *
 * Sunto:
 * - Simile al tutorial precedente, con alcune differenze.
 * - Ora la classe finestra "LWindow" gestisce il proprio renderer ("mRenderer"), che quindi non è
 *   più una variabile globale.
 * - Abbiamo aggiunto delle funzioni per spostare il focus sulla finestra ("focus"), e per
 *   interrogarla ("isShown") e sapere se è attualmente visualizzata. Inoltre, ciascuna finestra
 *   possiede un ID per poter attribuire correttamente gli eventi.
 * - Ora la funzione privata "init" è più breve, perché si limita a inizializzare SDL e il texture
 *   filtering. La maggior parte delle altre operazioni di inizializzazione, come la creazione della
 *   finestra e del renderer, sono ora migrate nel metodo "init" di "LWindow". Abbiamo inoltre
 *   l'identificazione dell'ID. We initially open up only a single window to check if window
 *   creation is functioning properly; before we enter the main loop, we open up the rest of the
 *   windows we have.
 * - Nel main loop, ciascun evento viene spedito ao ognuna delle tre finestre. Il metodo
 *   "handleEvent" di ciascuna finestra si occupa di filtrare l'evento mediante la conoscenza
 *   dell'ID.
 * - Per terminare il programma è necessario chiudere tutte e tre le finestre. When you have
 *   multiple windows, Xing out the window gets interpreted as "SDL_WINDOWEVENT_CLOSE" window
 *   events. When we get these events we're going to hide the window using "SDL_HideWindow".
 * - Se non si usa la chiamata a "SDL_HideWindow" nel caso "SDL_WINDOWEVENT_CLOSE", il tasto X (o
 *   ALT + F4) non hanno alcun effetto.
 * - Nel metodo "focus", usiamo (se necessario) "SDL_ShowWindow" per visualizzare una finestra, e
 *   "SDL_RaiseWindow" per spostare il focus su di essa.
 * - In the clean up function we close out any windows that might be open.
 * - In the main loop, after we handle the events for all the windows, we handle some special key
 *   presses. For this demo, when we press 1, 2, or 3 it will bring the corresponding window to
 *   focus.
 * - Next we render all the windows and then go through all the windows to check if any of them are
 *   shown. If all of them have been closed out we set the quit flag to true to end the program.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022) and may not be
 * redistributed without written permission.
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
  bool isShown(void);

  private:

  // Window data
  SDL_Window*   mWindow;
  SDL_Renderer* mRenderer;
  Uint32        mWindowID;

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

LWindow gWindows[ TOTAL_WINDOWS ]; // Our custom windows


/***************************************************************************************************
* Methods definitions
****************************************************************************************************/

/**
 * @brief Initialize non-existant window
 **/
LWindow::LWindow(void)
  : mWindow(NULL),
    mRenderer(NULL),
    mWindowID (1),
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
      mWindowID = SDL_GetWindowID( mWindow );

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
  // If an event was detected for this window
  if( e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID )
  {
    // Caption update flag
    bool updateCaption = false;

    switch( e.window.event )
    {
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
      mWidth = e.window.data1;
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

    // Update window caption with new data
    if( updateCaption )
    {
      std::stringstream caption;
      caption << "SDL Tutorial - ID: " << mWindowID << " MouseFocus:" << ( ( mMouseFocus ) ? "On" : "Off" ) << " KeyboardFocus:" << ( ( mKeyboardFocus ) ? "On" : "Off" );
      SDL_SetWindowTitle( mWindow, caption.str().c_str() );
    }
    else { /* No need to update window's caption */ }
  }
}


void LWindow::focus()
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


void LWindow::render()
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


bool LWindow::isShown()
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
      printf( "\nWarning: Linear texture filtering not enabled!" );
      success = false;
    }
    else
    {
      printf( "\nOK: linear texture filtering enabled" );
    }

    // Create window
    if( !gWindows[ 0 ].init() )
    {
      printf( "\nWindow 0 could not be created!\n" );
      success = false;
    }
    else
    {
      printf( "\nOK: window 0 created" );
    }
  }

  return success;
}



static void close(void)
{
  // Destroy windows
  for( int i = 0; i < TOTAL_WINDOWS; ++i )
  {
    gWindows[ i ].free();
  }

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

  // Start up SDL and create first window
  if( !init() )
  {
    printf( "\nFailed to initialize!" );
  }
  else
  {
    printf( "\nOK: all systems initialised" );

    // Initialize the rest of the windows
    for( int i = 1; i != TOTAL_WINDOWS; ++i )
    {
      gWindows[ i ].init();
    }

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
        for( int i = 0; i != TOTAL_WINDOWS; ++i )
        {
          gWindows[ i ].handleEvent( e );
        }

        // Pull up window
        if( e.type == SDL_KEYDOWN )
        {
          switch( e.key.keysym.sym )
          {
            case SDLK_1:
            gWindows[ 0 ].focus();
            break;

            case SDLK_2:
            gWindows[ 1 ].focus();
            break;

            case SDLK_3:
            gWindows[ 2 ].focus();
            break;
          }
        }
        else { /* Event not recognised / not managed here */ }
      }

      // Update all windows
      for( int i = 0; i != TOTAL_WINDOWS; ++i )
      {
        gWindows[ i ].render();
      }

      // Check all windows
      bool allWindowsClosed = true;

      for( int i = 0; i != TOTAL_WINDOWS; ++i )
      {
        if( gWindows[ i ].isShown() )
        {
          allWindowsClosed = false;
          break;
        }
        else { /* All windows have been closed */ }
      }

      if( allWindowsClosed )
      {
        quit = true;
      }
      else { /* There is still at least an open window: do not stop the program */ }
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