/**
 * @file 04_key_presses.cpp
 *
 * @brief Le funzioni e le macro di SDL sono sempre precedute dal prefisso SDL. Pertanto, tutte le
 * altre funzioni utilizzate in questo esempio sono funzioni private, di comodo.
 *
 * Flusso di lavoro:
 *  - main chiama init
 *    - init chiama SDL_Init, necessaria per inizializzare SDL.
 *      - In caso di successo, SDL_Init chiama SDL_CreateWindow per creare la finestra principale.
 *      - In caso di successo, SDL_Init chiama SDL_GetWindowSurface, per poter controllare ciò che
 *        la finestra visualizza.
 *  - main chiama loadMedia
 *    - loadMedia chiama diverse volte loadSurface, per caricare le immagini da visualizzare.
 *      - loadSurface chiama SDL_LoadBMP per caricare l'immagine ricevuta come argomento.
 *  - main crea un oggetto SDL_Event, predispone l'immagine di default ad essere blittata, e fa
 *    partire il main loop. Mediante l'oggetto SDL_Event, inizia ad "ascoltare" la tastiera usando
 *    SDL_PollEvent e passando l'oggetto SDL_Event.
 *  - se l'utente preme un tasto freccia, l'evento viene intercettato nel main loop, e un immagine
 *    appropriata viene sostituita all'immagine di default.
 *  - come ultima istruzione, il main loop fa il blitting dell'immagine impostata in gCurrentSurface
 *    e chiama l'aggiornamento della finestra mediante SDL_UpdateWindowSurface.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, standard IO, and strings
#include <SDL.h>
#include <stdio.h>
#include <string>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

// Screen dimension constants
static constexpr int SCREEN_WIDTH  = 640;
static constexpr int SCREEN_HEIGHT = 480;

// Key press surfaces constants
enum KeyPressSurfaces
{
  KEY_PRESS_SURFACE_DEFAULT,
  KEY_PRESS_SURFACE_UP,
  KEY_PRESS_SURFACE_DOWN,
  KEY_PRESS_SURFACE_LEFT,
  KEY_PRESS_SURFACE_RIGHT,

  KEY_PRESS_SURFACE_TOTAL
};

// Error Timer
static constexpr size_t MS_IN_S = 1000; // Milliseconds in a second

// Paths to BMP images
const std::string Img_Press("press.bmp");
const std::string Img_Up("up.bmp");
const std::string Img_Down("down.bmp");
const std::string Img_Left("left.bmp");
const std::string Img_Right("right.bmp");


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void MsgTimer(size_t Time_S = (size_t)1);

static SDL_Surface* loadSurface( const std::string& path );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*  gWindow         = NULL; // The window we'll be rendering to
static SDL_Surface* gScreenSurface  = NULL; // The surface contained by the window
static SDL_Surface* gCurrentSurface = NULL; // Currently displayed image
static SDL_Surface* gKeyPressSurfaces[ KEY_PRESS_SURFACE_TOTAL ]; // The images that correspond to a keypress


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

/**
 * @brief Starts up SDL and creates window
 *
 * @return true
 * @return false
 **/
bool init(void)
{
  // Initialization flag
  bool success = true;

  // Initialize SDL and check
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    success = false;
    MsgTimer();
  }
  else
  {
    // Create window and check
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
      success = false;
      MsgTimer();
    }
    else
    {
      // Get window surface
      gScreenSurface = SDL_GetWindowSurface( gWindow );
    }
  }

  return success;
}


/**
 * @brief Loads all necessary media for this project.
 *
 * @return true if loading was successful; false otherwise
 **/
bool loadMedia(void)
{
  // Loading success flag
  bool success = true;

  // Load default surface
  gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] = loadSurface( Img_Press );

  if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] == NULL )
  {
    printf( "Failed to load %s image! SDL error: %s\n", Img_Press.c_str(), SDL_GetError() );
    success = false;
    MsgTimer();
  }
  else
  {;} // Load successful

  // Load "up" surface
  gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] = loadSurface( Img_Up );

  if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] == NULL )
  {
    printf( "Failed to load %s image! SDL error: %s\n", Img_Up.c_str(), SDL_GetError() );
    success = false;
    MsgTimer();
  }
  else
  {;} // Load successful

  // Load "down" surface
  gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] = loadSurface( Img_Down );

  if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] == NULL )
  {
    printf( "Failed to load %s image! SDL error: %s\n", Img_Down.c_str(), SDL_GetError() );
    success = false;
  }
  else
  {;} // Load successful

  // Load "left" surface
  gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ] = loadSurface( Img_Left );

  if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ] == NULL )
  {
    printf( "Failed to load %s image! SDL error: %s\n", Img_Left.c_str(), SDL_GetError() );
    success = false;
    MsgTimer();
  }
  else
  {;} // Load successful

  // Load right surface
  gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ] = loadSurface( Img_Right );

  if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ] == NULL )
  {
    printf( "Failed to load %s image! SDL error: %s\n", Img_Press.c_str(), SDL_GetError() );
    success = false;
    MsgTimer();
  }
  else
  {;} // Load successful

  return success;
}


/**
 * @brief Frees media and shuts down SDL
 **/
void close(void)
{
  // Deallocate surfaces
  for( int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i )
  {
    SDL_FreeSurface( gKeyPressSurfaces[ i ] );
    gKeyPressSurfaces[ i ] = NULL;
  }

  // Destroy window
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  // Quit SDL subsystems
  SDL_Quit();
}


/**
 * @brief Loads individual image
 *
 * @param path
 * @return SDL_Surface*
 **/
static SDL_Surface* loadSurface( const std::string& path )
{
  // Load image at specified path
  SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );

  if( loadedSurface == NULL )
  {
    printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
    MsgTimer();
  }
  else
  {;} // Image loaded correctly

  return loadedSurface;
}


/**
 * @brief
 * @brief Used to keep the console open for a short time when a message is displayed, to allow the
 * user to read it before the console automatically closes.
 *
 * @param Time_S Time in seconds. Defaults to 1
 **/
static void MsgTimer(size_t Time_S)
{
  for (size_t i = 0; i != Time_S; ++i)
  {
    printf(".");
    SDL_Delay(MS_IN_S); // Freeze the program for a second
  }

  printf("\n");
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
    HasProgramSucceeded = false;
    MsgTimer();
  }
  else
  {
    // Load media
    if( !loadMedia() )
    {
      printf( "Failed to load media!\n" );
      HasProgramSucceeded = false;
      MsgTimer();
    }
    else
    {
      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

      // Set default current surface
      gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];

      // While application is running
      while( !quit )
      {
        // Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
          if( e.type == SDL_QUIT ) // User requests quit
          {
            quit = true;
          }
          else if( e.type == SDL_KEYDOWN ) // User presses a key
          {
            // Select surfaces based on key press
            switch( e.key.keysym.sym )
            {
              case SDLK_UP:
                gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ];
                break;

              case SDLK_DOWN:
                gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ];
                break;

              case SDLK_LEFT:
                gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ];
                break;

              case SDLK_RIGHT:
                gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ];
                break;

              default:
                gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
                break;
            }
          }
          else
          {;}
        }

        // Apply the current image
        SDL_BlitSurface( gCurrentSurface, NULL, gScreenSurface, NULL );

        // Update the surface
        SDL_UpdateWindowSurface( gWindow );
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

  printf("\nPress ENTER to exit...");

  int UserChoice = '\0';

  while ( ( UserChoice = getchar() ) != '\n' )
  {;}

  return 0;
}