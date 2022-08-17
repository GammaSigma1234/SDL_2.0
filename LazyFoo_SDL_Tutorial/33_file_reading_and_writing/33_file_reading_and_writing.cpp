/**
 * @file 33_file_reading_and_writing.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/33_file_reading_and_writing/index.php
 *
 * Being able to save and load data is needed to keep data between play sessions. SDL_RWops file
 * handling allows us to do cross platform file IO to save data.
 *
 * We're declaring an array of signed integers that are 32 bits long. This will be the data we
 * will be loading and saving. For this demo, this array will be of length 10.
 *
 * In our media loading function we're opening the save file for reading using SDL_RWFromFile. The
 * first argument is the path to the file, and the second argument defines how we will be opening it.
 * "r+b" means it is being opened for reading in binary.
 *
 * If the file does not exist that doesn't exactly mean an error. It could mean this is the
 * first time the program has run and the file has not been created yet. If the file does not exist
 * we prompt a warning and create a file by opening a file with "w+b". This will open a new file for
 * writing in binary.
 *
 * If a new file was created successfully, we then start writing the initialized data to it using
 * "SDL_RWwrite". The first argument is the file we're writing to, the second argument is the
 * address of the objects in memory we're writing, the third argument is the number of bytes per
 * object we're writing, and the last one is the number of objects we're writing. After we're done
 * writing out all the objects, we close the file for writing using "SDL_RWclose". If the file was
 * never created in the first place, we report an error to the console and set the success flag to
 * false.
 *
 * If our file loaded successfully on the first try, all we have to do is reading in the data using
 * SDL_RWread, which basically functions like SDL_RWwrite but in reverse. After the file is loaded,
 * we render the text textures that correspond with each of our data numbers. Our
 * "loadFromRenderedText" function only accepts strings, so we have to convert the integers to
 * strings.
 *
 * When we close the program, we open up the file again for writing and write out all the data.
 *
 * Before we go into the main loop we declare "currentData" to keep track of which of our data
 * integers we're altering. We also declare a plain text color and a highlight color for rendering
 * text.
 *
 * When we press up or down we want to rerender the the old current data in plain color, move to the
 * next data point (with some bounds checking), and rerender the new current data in the highlight
 * color. When we press left or right, we decrement or increment the current data and rerender the
 * texture associated with it.
 *
 * At the end of the main loop we render all the textures to the screen.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

/**************************************************************************************************
* Includes
***************************************************************************************************/

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

static const std::string FontPath("lazy.ttf");
static const std::string BinaryFileName("nums.bin");

static constexpr int TOTAL_DATA       = 10; // Number of data integers
static constexpr int WRITE_ONE_OBJECT = 1;  // Number of objects to write in SDL_RWwrite


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

// Globally used font
static TTF_Font *gFont = NULL;

// Scene textures
static LTexture gPromptTextTexture;
static LTexture gDataTextures[ TOTAL_DATA ];

// Data points
static Sint32 gData[ TOTAL_DATA ];


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

         // Initialize SDL_ttf
        if( TTF_Init() == -1 )
        {
          printf( "\nSDL_ttf could not initialize! SDL_ttf Error: \"%s\"\n", TTF_GetError() );
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
  // Text rendering color
  SDL_Color textColor      = { BLACK_R, BLACK_G, BLACK_B, BLACK_A };
  SDL_Color highlightColor = { RED_R, RED_G, RED_B, RED_A };

  // Loading success flag
  bool success = true;

  // Open the font
  gFont = TTF_OpenFont( FontPath.c_str(), 28 );

  if( gFont == NULL )
  {
    printf( "\nFailed to load lazy font! SDL_ttf Error: \"%s\"\n", TTF_GetError() );
    success = false;
  }
  else
  {
    printf( "\nLazy font loaded" );

    // Render the prompt
    if( !gPromptTextTexture.loadFromRenderedText( "Enter Data:", textColor ) )
    {
      printf( "\nFailed to render prompt text!\n" );
      success = false;
    }
    else
    {
      printf( "\nPrompt text rendered" );
    }
  }

  // Open file for reading in binary
  SDL_RWops* file = SDL_RWFromFile( BinaryFileName.c_str(), "r+b" );

  // File does not exist
  if( file == NULL )
  {
    // printf( "\nWarning: Unable to open file! SDL Error: %s", SDL_GetError() ); // Originale
    printf( "\nCreating new \"%s\" file...", BinaryFileName.c_str() );

    // Create file for writing
    file = SDL_RWFromFile( BinaryFileName.c_str(), "w+b" );
    if( file != NULL )
    {
      printf( "\nNew file created!" );

      // Initialize data
      for( int i = 0; i < TOTAL_DATA; ++i )
      {
        gData[ i ] = 0;
        SDL_RWwrite( file, &gData[ i ], sizeof(Sint32), WRITE_ONE_OBJECT );
      }

      // Close file handler
      SDL_RWclose( file );
    }
    else
    {
      printf( "\nError: Unable to create file! SDL Error: %s", SDL_GetError() );
      success = false;
    }
  }
  // File exists
  else
  {
    // Load data
    printf( "\nReading file..." );

    for( int i = 0; i < TOTAL_DATA; ++i )
    {
      SDL_RWread( file, &gData[ i ], sizeof(Sint32), 1 );
    }

    // Close file handler
    SDL_RWclose( file );
  }

  // Initialize data textures
  gDataTextures[ 0 ].loadFromRenderedText( std::to_string( gData[ 0 ] ), highlightColor );

  for( int i = 1; i < TOTAL_DATA; ++i )
  {
    gDataTextures[ i ].loadFromRenderedText( std::to_string( gData[ i ] ), textColor );
  }

  return success;
}


static void close(void)
{
  // Open data for writing
  SDL_RWops* file = SDL_RWFromFile( BinaryFileName.c_str(), "w+b" );

  if( file != NULL )
  {
    // Save data
    for( int i = 0; i < TOTAL_DATA; ++i )
    {
      SDL_RWwrite( file, &gData[ i ], sizeof(Sint32), 1 );
    }

    // Close file handler
    SDL_RWclose( file );
  }
  else
  {
    printf( "\nError: Unable to save file! %s", SDL_GetError() );
  }

  // Free loaded images
  gPromptTextTexture.free();

  for( int i = 0; i < TOTAL_DATA; ++i )
  {
    gDataTextures[ i ].free();
  }

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

      // Text rendering color
      SDL_Color textColor = { BLACK_R, BLACK_G, BLACK_B, BLACK_A };
      SDL_Color highlightColor = { RED_R, RED_G, RED_B, RED_A };

      // Current input point
      int currentData = 0;

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
          else if( e.type == SDL_KEYDOWN )
          {
            switch( e.key.keysym.sym )
            {
              // Previous data entry
              case SDLK_UP:
              // Rerender previous entry input point
              gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), textColor );
              --currentData;
              if( currentData < 0 )
              {
                currentData = TOTAL_DATA - 1;
              }

              // Rerender current entry input point
              gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), highlightColor );
              break;

              // Next data entry
              case SDLK_DOWN:
              // Rerender previous entry input point
              gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), textColor );
              ++currentData;
              if( currentData == TOTAL_DATA )
              {
                currentData = 0;
              }

              // Rerender current entry input point
              gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), highlightColor );
              break;

              // Decrement input point
              case SDLK_LEFT:
              --gData[ currentData ];
              gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), highlightColor );
              break;

              // Increment input point
              case SDLK_RIGHT:
              ++gData[ currentData ];
              gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), highlightColor );
              break;
            }
          }
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render text textures
        gPromptTextTexture.render( ( SCREEN_W - gPromptTextTexture.getWidth() ) / 2, 0 );

        for( int i = 0; i < TOTAL_DATA; ++i )
        {
          gDataTextures[ i ].render( ( SCREEN_W - gDataTextures[ i ].getWidth() ) / 2, gPromptTextTexture.getHeight() + gDataTextures[ 0 ].getHeight() * i );
        }

        // Update screen
        SDL_RenderPresent( gRenderer );
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