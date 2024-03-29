/**
 * @file 41_bitmap_fonts.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/41_bitmap_fonts/index.php
 *
 * Sometimes TTF fonts are flexible enough. Since rendering text is just rendering images of
 * characters, we can use bitmap fonts to render text.
 *
 * Sunto:
 *  - Pitch: occupazione in memoria della texture (in bytes).
 *  - "LTexture::getPixel32" aggiunta per identificare la composizione di un singolo pixel in un
 *    determinato punto (x, y).
 *  - Promemoria: i pixel di una texture bidimensionale vengono memorizzati come un'unica, singola
 *    sequenza di bytes. Quindi, è necessario "trasformare" una matrice bidimensionale in un vettore
 *    monodimensionale.
 *  - La classe "LBitmapFont" rappresenta una sorta di sprite sheet di glifi da cui costruire i
 *    font.
 *  - We did the color keying externally in the previous tutorial (in "loadMedia"), and here we're doing it
 *    internally in the texture loading function ("loadFromFile").
 *  - "LBitmapFont::buildFont": goes through the bitmap font and defines all the clip rectangles for
 *    all the sprites. To do that we'll have to lock the texture to access its pixels.
 *  - La ricerca del fondo del carattere 'A' (Find Bottom of 'A') serve per definire la baseline su
 *    cui basare il posizionamento geometrico di tutti gli altri caratteri.
 *  - Il metodo "renderText" renderizza il testo a partire da una
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/**************************************************************************************************
* Includes
***************************************************************************************************/

// Using SDL, SDL_image, SDL_ttf, standard I/O, math, strings, and string streams
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int FIRST_ONE = -1;

static constexpr int WINDOW_W = 800; // Screen's width
static constexpr int WINDOW_H = 600; // Screen's heigth

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

static constexpr int PADDING_px      = 10; // Amount of padding (lateral spacing) between characters
static constexpr int BYTES_PER_PIXEL = 4;
static constexpr int GLYPHS_PER_ROW  = 16; // Numero di glifi contenuti in ogni riga    dello sprite sheet
static constexpr int GLYPHS_PER_COL  = 16; // Numero di glifi contenuti in ogni colonna dello sprite sheet

/* Paths */

static const std::string g_LazyFontPath( "lazyfont.png" );


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
  int getWidth ( void ) const;
  int getHeight( void ) const;

  // Pixel manipulators
  bool   lockTexture   ( void );
  bool   unlockTexture ( void );
  void*  getPixels     ( void ) const;
  int    getPitch      ( void ) const;
  Uint32 getPixel32    ( unsigned int, unsigned int );

private:

  // The actual hardware texture
  SDL_Texture* mTexture;
  void*        mPixels;
  int          mPitch;

  // Image dimensions
  int mWidth;
  int mHeight;
};


/**
 * @brief Our bitmap font (wrapper for a sprite sheet of glyphs)
 **/
class LBitmapFont
{
    public:
    // The default constructor
    LBitmapFont(void);

    // Generates the font
    bool buildFont( LTexture* );

    // Shows the text
    void renderText( int x, int y, const std::string& );

    private:
    // The font texture
    LTexture* mBitmap;

    // The individual characters in the surface
    SDL_Rect mChars[ 256 ];

    // Spacing Variables
    int mNewLine, mSpace;
};


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init      (void);
static bool loadMedia (void);
static void close     (void);


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer

// Scene textures
static LTexture    gBitmapTexture;
static LBitmapFont gBitmapFont;


/***************************************************************************************************
* Methods definitions
****************************************************************************************************/

LTexture::LTexture(void)
{
  // Initialize
  mTexture = NULL;
  mWidth = 0;
  mHeight = 0;
  mPixels = NULL;
  mPitch = 0;
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

    // Convert surface to display format
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0 );

    if( formattedSurface == NULL )
    {
      printf( "\nUnable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError() );
    }
    else
    {
      // Create blank streamable texture
      newTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );

      if( newTexture == NULL )
      {
        printf( "\nUnable to create blank texture! SDL Error: %s\n", SDL_GetError() );
      }
      else
      {
        // Enable blending on texture
        SDL_SetTextureBlendMode( newTexture, SDL_BLENDMODE_BLEND );

        // Lock texture for manipulation
        SDL_LockTexture( newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch );

        // Copy loaded/formatted surface pixels
        memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

        // Get image dimensions
        mWidth  = formattedSurface->w;
        mHeight = formattedSurface->h;

        // Get pixel data in editable format
        Uint32* pixels = (Uint32*)mPixels;
        int pixelCount = ( mPitch / BYTES_PER_PIXEL ) * mHeight;

        // Map colors
        Uint32 colorKey    = SDL_MapRGB ( formattedSurface->format, CYAN_R, CYAN_G, CYAN_B );
        Uint32 transparent = SDL_MapRGBA( formattedSurface->format, CYAN_R, CYAN_G, CYAN_B, 0x00 );

        // Color key pixels
        for( int i = 0; i != pixelCount; ++i )
        {
          if( pixels[ i ] == colorKey )
          {
            pixels[ i ] = transparent;
          }
          else { /* Skip this pixel */ }
        }

        // Unlock texture to update
        SDL_UnlockTexture( newTexture );
        mPixels = NULL;
      }

      // Get rid of old formatted surface
      SDL_FreeSurface( formattedSurface );
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
    mWidth   = 0;
    mHeight  = 0;
    mPixels  = NULL;
    mPitch   = 0;
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


bool LTexture::lockTexture(void)
{
  bool success = true;

  // Texture is already locked
  if( mPixels != NULL )
  {
    printf( "\nTexture is already locked!" );
    success = false;
  }
  // Lock texture
  else
  {
    printf( "\nLocking texture..." );

    if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
    {
      printf( "\nUnable to lock texture! %s\n", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nOK: texture locked" );
    }
  }

  return success;
}


bool LTexture::unlockTexture(void)
{
  bool success = true;

  // Texture is not locked
  if( mPixels == NULL )
  {
    printf( "\nTexture is not locked!" );
    success = false;
  }
  // Unlock texture
  else
  {
    printf( "\nUnlocking texture..." );

    SDL_UnlockTexture( mTexture );
    mPixels = NULL;
    mPitch = 0;
  }

  return success;
}


void* LTexture::getPixels(void) const
{
  return mPixels;
}


int LTexture::getPitch(void) const
{
  return mPitch;
}


/**
 * @brief I pixel di una texture bidimensionale vengono memorizzati come un'unica, singola sequenza
 * di bytes. Quindi, è necessario "trasformare" una matrice bidimensionale in un vettore
 * monodimensionale.
 *
 * @param x Coordinata x del pixel da ricavare
 * @param y Coordinata y del pixel da ricavare
 * @return Uint32
 **/
Uint32 LTexture::getPixel32( unsigned int x, unsigned int y )
{
  // Convert the pixels to 32 bit
  Uint32* pixels = (Uint32*)mPixels;

  // Get the pixel requested
  return pixels[ ( y * ( mPitch / BYTES_PER_PIXEL ) ) + x ];
}


LBitmapFont::LBitmapFont(void)
  : mBitmap(NULL), mNewLine(0), mSpace(0)
{;}


/**
 * @brief Generate a font from a bitmap texture.
 *
 * @param bitmap Source bitmap texture
 **/
bool LBitmapFont::buildFont( LTexture* bitmap )
{
  bool success = true;

  // Lock pixels for access
  if( !bitmap->lockTexture() )
  {
    printf( "\nUnable to lock bitmap font texture!" );
    success = false;
  }
  else
  {
    printf( "\nOK: bitmap font texture locked" );

    // Set the background color
    Uint32 bgColor = bitmap->getPixel32( 0, 0 ); // Il pixel in posizione (0, 0) è di colore ciano

    // Set the cell dimensions
    int cellW = bitmap->getWidth()  / GLYPHS_PER_ROW;
    int cellH = bitmap->getHeight() / GLYPHS_PER_COL;

    // New line variables
    int top   = cellH;
    int baseA = cellH;

    // The current character we're setting
    int currentChar = 0;

    // Go through the cell rows
    for( int rows = 0; rows != GLYPHS_PER_ROW; ++rows )
    {
      // Go through the cell columns
      for( int cols = 0; cols != GLYPHS_PER_COL; ++cols )
      {
        // Set the character initial offset
        mChars[ currentChar ].x = cellW * cols;
        mChars[ currentChar ].y = cellH * rows;

        // Set the initial dimensions of the character
        mChars[ currentChar ].w = cellW;
        mChars[ currentChar ].h = cellH;

        /* Restringimento delle dimensioni del clip da utilizzare per ricavare dallo sprite sheet il
        carattere attualmente analizzato ("currentChar") */

        // Find Left Side
        // Go through pixel columns. ATTENZIONE: l'ordine di ricerca qui utilizzato (prima cols e
        // poi rows) è rovesciato rispetto al consueto (prima rows e poi cols)
        for( int pCol = 0; pCol < cellW; ++pCol )
        {
          // Go through pixel rows
          for( int pRow = 0; pRow < cellH; ++pRow )
          {
            // Get the pixel offsets
            int pX = ( cellW * cols ) + pCol;
            int pY = ( cellH * rows ) + pRow;

            // If a non colorkey pixel is found
            if( bitmap->getPixel32( pX, pY ) != bgColor )
            {
              // Set the x offset
              mChars[ currentChar ].x = pX;

              // Break the loops
              pCol = cellW;
              pRow = cellH;
            }
            else { /* Colorkey pixel found */ }
          }
        }

        // Find Right Side
        // Go through pixel columns
        for( int pColW = cellW - 1; pColW >= 0; --pColW )
        {
          // Go through pixel rows
          for( int pRowW = 0; pRowW < cellH; ++pRowW )
          {
            // Get the pixel offsets
            int pX = ( cellW * cols ) + pColW;
            int pY = ( cellH * rows ) + pRowW;

            // If a non colorkey pixel is found
            if( bitmap->getPixel32( pX, pY ) != bgColor )
            {
              // Set the width
              mChars[ currentChar ].w = ( pX - mChars[ currentChar ].x ) + 1;

              // Break the loops
              pColW = -1;
              pRowW = cellH;
            }
            else { /* Colorkey pixel found */ }
          }
        }

        // Find Top
        // Go through pixel rows
        for( int pRow = 0; pRow < cellH; ++pRow )
        {
          // Go through pixel columns
          for( int pCol = 0; pCol < cellW; ++pCol )
          {
            // Get the pixel offsets
            int pX = ( cellW * cols ) + pCol;
            int pY = ( cellH * rows ) + pRow;

            // If a non colorkey pixel is found
            if( bitmap->getPixel32( pX, pY ) != bgColor )
            {
              // If new top is found
              if( pRow < top )
              {
                top = pRow;
              }

              // Break the loops
              pCol = cellW;
              pRow = cellH;
            }
            else { /* Colorkey pixel found */ }
          }
        }

        // Find Bottom of 'A'. Serve per definire la baseline dei caratteri
        if( currentChar == 'A' )
        {
          // Go through pixel rows
          for( int pRow = cellH - 1; pRow >= 0; --pRow )
          {
            // Go through pixel columns
            for( int pCol = 0; pCol < cellW; ++pCol )
            {
              // Get the pixel offsets
              int pX = ( cellW * cols ) + pCol;
              int pY = ( cellH * rows ) + pRow;

              // If a non colorkey pixel is found
              if( bitmap->getPixel32( pX, pY ) != bgColor )
              {
                // Bottom of a is found
                baseA = pRow;

                // Break the loops
                pCol = cellW;
                pRow = -1;
              }
              else { /* Colorkey pixel found */ }
            }
          }
        }

        // Go to the next character
        ++currentChar;
      }
    }

    /* Defining space & new line + post-processing */

    // Calculate space
    mSpace = cellW / 2;

    // Calculate new line
    mNewLine = baseA - top;

    // Lop off excess top pixels
    for( int i = 0; i != 256; ++i )
    {
      mChars[ i ].y += top;
      mChars[ i ].h -= top;
    }

    bitmap->unlockTexture();
    mBitmap = bitmap;
  }

  return success;
}


void LBitmapFont::renderText( int x, int y, const std::string& text )
{
  // If the font has been built
  if( mBitmap != NULL )
  {
    // Temp offsets
    int curX = x, curY = y;

    // Go through the text
    for( size_t i = 0; i != text.length(); ++i )
    {
      // If the current character is a space
      if( text[ i ] == ' ' )
      {
        // Move over
        curX += mSpace;
      }
      // If the current character is a newline
      else if( text[ i ] == '\n' )
      {
        // Move down
        curY += mNewLine;

        // Move back
        curX = x;
      }
      else
      {
        // Get the ASCII value of the character
        int ascii = (unsigned char)text[ i ];

        // Show the character
        mBitmap->render( curX, curY, &mChars[ ascii ] );

        // Move over the width of the character with padding
        curX += mChars[ ascii ].w + PADDING_px;
      }
    }
  }
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

    // Seed random
    std::srand( static_cast<unsigned int>( SDL_GetTicks64() ) );

    // Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial"         ,
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                WINDOW_W               , WINDOW_H               ,
                                SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nOK: window created" );

      // Create renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, FIRST_ONE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

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
          printf( "\nSDL_image could not initialise! SDL_image Error: %s", IMG_GetError() );
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
static bool loadMedia( void )
{
  // Loading success flag
  bool success = true;

  // Load font texture
  if( !gBitmapTexture.loadFromFile( g_LazyFontPath ) )
  {
    printf( "\nFailed to load corner texture!" );
    success = false;
  }
  else
  {
    printf( "\nOK: corner texture loaded" );

    // Build font from texture
    gBitmapFont.buildFont( &gBitmapTexture );
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gBitmapTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow  ( gWindow );
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
    printf( "\nFailed to initialise!" );
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
          else { /* Event not managed here */ }
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear       ( gRenderer );

        // Render test text
        gBitmapFont.renderText( 0, 0, "Bitmap Font:\nABDCEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789" );

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