/**
 * @file 16_true_type_fonts.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/16_true_type_fonts/index.php
 *
 * @brief One way to render text with SDL is with the extension library SDL_ttf. SDL_ttf allows you
 * to create images from TrueType fonts which we'll use here to create textures from font text.
 *
 * We're adding another function to the texture class, called loadFromRenderedText. The way SDL_ttf
 * works is that you create a new image from a font and color. For our texture class all that means
 * is that we're going to be loading our image from text rendered by SDL_ttf instead of a file.
 *
 * For this and future tutorials, we'll be using a global font for our text rendering. In SDL_ttf,
 * the data type for fonts is TTF_Font. We also have a texture which will be generated from the
 * font.
 *
 * Just like SDL_image, we have to initialize SDL_ttf, or the font loading and rendering functions
 * won't work properly. We start up SDL_ttf using TTF_init. We can check for errors using
 * TTF_GetError().
 *
 * loadFromRenderedText is where we actually create the text texture we're going to render from the
 * font. This function takes in the string of text we want to render and the color we want to use to
 * render it. After that, this function pretty much works like loading from a file does, only this
 * time we're using a SDL_Surface created by SDL_ttf instead of a file.
 *
 * After freeing any preexisting textures, we load a surface using TTF_RenderText_Solid. This
 * creates a solid color surface from the font, text, and color given. If the surface was created
 * successfully, we create a texture out of it just like we did before when loading a surface from a
 * file. After the text texture is created, we can render with it just like any other texture.
 *
 * There are other ways to render text that are smoother or blended. Experiment with the different
 * types of rendering outlined in the SDL_ttf documentation. Shaded/Blended rendering might work
 * better for different text sizes.
 *
 * In our loading function, we load our font using TTF_OpenFont. This takes in the path to the font
 * file and the point size we want to render at.
 *
 * If the font loaded successfully, we want to load a text texture using our loading method. As a
 * general rule, you want to minimize the number of times you render text. Only re-render it when
 * you need to and since we're using the same text surface for this whole program, we only want to
 * render once.
 *
 * After we create the text texture, we can render it just like any other texture.
 *
 * In our clean up function, we want to free the font using TTF_CloseFont. We also want to quit the
 * SDL_ttf library with TTF_Quit to complete the clean up.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, SDL_ttf, standard I/O, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int FIRST_ONE = -1;

static constexpr int WINDOW_W = 640; // Screen's width
static constexpr int WINDOW_H = 480; // Screen's heigth

// Colore bianco (Inizializzazione renderer)
static constexpr int WHITE_R = 0xFF; // Amount of red   needed to compose white
static constexpr int WHITE_G = 0xFF; // Amount of green needed to compose white
static constexpr int WHITE_B = 0xFF; // Amount of blue  needed to compose white
static constexpr int WHITE_A = 0xFF; // Alpha component

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan

static const std::string FontPath("lazy.ttf");


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

    // Creates image from font string
    bool loadFromRenderedText( const std::string&, SDL_Color );

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

static SDL_Window*   gWindow        = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer      = NULL; // The window renderer
static TTF_Font*     gFont          = NULL; // Globally used font
static LTexture      gTextTexture;          // Rendered texture


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

void LTexture::free(void)
{
  // Free texture if it exists
  if( mTexture != NULL )
  {
    SDL_DestroyTexture( mTexture );
    mTexture = NULL;
    mWidth   = 0;
    mHeight  = 0;
  }
  else
  {;}
}

bool LTexture::loadFromRenderedText( const std::string& textureText, SDL_Color textColor )
{
  // Get rid of preexisting texture
  free();

  // Render text surface
  SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );

  if( textSurface == NULL )
  {
    printf( "\nUnable to render text surface! SDL_ttf Error: %s", TTF_GetError() );
  }
  else
  {
    printf( "\nText surface \"%s\" loaded", textureText.c_str() );

    // Create texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );

    if( mTexture == NULL )
    {
      printf( "\nUnable to create texture from rendered text! SDL Error: %s", SDL_GetError() );
    }
    else
    {
      printf( "\nTexture created from \"%s\"", textureText.c_str() );

      // Get image dimensions
      mWidth  = textSurface->w;
      mHeight = textSurface->h;
    }

    // Get rid of old surface
    SDL_FreeSurface( textSurface );
  }

  // Return success
  return mTexture != NULL;
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
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN );

    if( gWindow == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: %s", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nWindow created" );

      // Create accelerated and vsynced renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, FIRST_ONE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

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
          printf( "\nSDL_image could not initialize! SDL_mage Error: %s", IMG_GetError() );
          success = false;
        }
        else
        {
          printf( "\nSDL_image initialised" );
        }

        // Initialize SDL_ttf
        if( TTF_Init() == -1 )
        {
          printf( "\nSDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError() );
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
  // Loading success flag
  bool success = true;

  // Open the font
  gFont = TTF_OpenFont( FontPath.c_str(), 28 );

  if( gFont == NULL )
  {
    printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
    success = false;
  }
  else
  {
    printf( "Lazy font loaded" );

    // Render text
    SDL_Color textColor = { 0, 0, 0 };

    if( !gTextTexture.loadFromRenderedText( "The quick brown fox jumps over the lazy dog", textColor ) )
    {
      printf( "\nFailed to render text texture!" );
      success = false;
    }
    else
    {
      printf( "\nText texture loaded" );
    }
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gTextTexture.free();

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
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render current frame
        int CENTERED_HORIZONTALLY = ( WINDOW_W - gTextTexture.getWidth()  ) / 2;
        int CENTERED_VERTICALLY   = ( WINDOW_H - gTextTexture.getHeight() ) / 2;
        gTextTexture.render( CENTERED_HORIZONTALLY, CENTERED_VERTICALLY );

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