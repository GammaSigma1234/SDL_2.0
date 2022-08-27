/**
 * @file 12_color_modulation.cpp
 *
 * @brief Come funziona la modulazione RGB? Vedi link qui sotto per una spiegazione.
 *
 * https://lazyfoo.net/tutorials/SDL/12_color_modulation/index.php
 *
 * Color modulation allows you to alter the color of your rendered textures. Here we're going to
 * modulate a texture using various colors.
 *
 * We're adding a function to the texture wrapper class that will allow the texture modulation to be
 * set. All it does is take in a red, green, and blue color components.
 *
 * Setting texture modulation is as easy as making a call to SDL_SetTextureColorMod. You just pass
 * in the texture you want to modulate and the color you want to modulate with.
 *
 * SDL_SetTextureColorMod accepts Uint8 as arguments for the color components. An Uint8 is just an
 * integer that is Unsigned and 8bit. This means it goes from 0 to 255. 128 is about halfway between
 * 0 and 255, so when you modulate green to 128 it halves the green component for any pixel on the
 * texture. The red and blue squares don't get affected because they have no green in them, but the
 * green becomes half as bright and the white turns a light magenta (magenta is red 255, green 0,
 * blue 255). Color modulation is just a way to multiply a color throughout the whole texture.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022) and may not be
 * redistributed without written permission.
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, standard I/O, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int FIRST_ONE = -1;

static constexpr int WINDOW_W = 640;
static constexpr int WINDOW_H = 480;

// Colore bianco
static constexpr int WHITE_R = 0xFF;
static constexpr int WHITE_G = 0xFF;
static constexpr int WHITE_B = 0xFF;
static constexpr int WHITE_A = 0xFF;

static constexpr Uint8 INCREMENT = static_cast<Uint8>(16);

static const std::string FilePath("colors.png");


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

    // Deallocates texture
    void free(void);

    // Set color modulation
    void setColor( Uint8, Uint8, Uint8 );

    // Get color modulation
    void getColor( Uint8&, Uint8&, Uint8& );

    // Renders texture at given point
    void render( int x, int y, SDL_Rect* = NULL );

    // Gets image dimensions
    int getWidth(void) const;
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

static SDL_Window*   gWindow   = NULL;  // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL;  // The window renderer
static LTexture      gModulatedTexture; // Scene texture


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
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

    if( newTexture == NULL )
    {
      printf( "\nUnable to create texture from \"%s\"! SDL Error: %s", path.c_str(), SDL_GetError() );
    }
    else
    {
      printf( "\nTexture create from \"%s\"", path.c_str() );

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
    mWidth = 0;
    mHeight = 0;
  }
}


/**
 * @brief Set texture's colour modulation
 *
 * @param red
 * @param green
 * @param blue
 **/
void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
  // Modulate texture
  SDL_SetTextureColorMod( mTexture, red, green, blue );
}


/**
 * @brief Get texture's colour modulation
 *
 * @param red
 * @param green
 * @param blue
 **/
void LTexture::getColor( Uint8& red, Uint8& green, Uint8& blue )
{
  // Modulate texture
  SDL_GetTextureColorMod( mTexture, (Uint8*)&red, (Uint8*)&green, (Uint8*)&blue );
}


/**
 * @brief Renders texture at given (x, y) point. Accepts a rectangle defining which portion of the texture
 * we want to render. Default argument of NULL to render the whole texture.
 *
 * @param x x coordinate of the rendering point.
 * @param y y coordinate of the rendering point.
 * @param clip Rectangle defining which portion of the texture we want to render. NULL to render the
 * whole texture.
 **/
void LTexture::render( int x, int y, SDL_Rect* clip )
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
  SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );
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

      // Create renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, FIRST_ONE, SDL_RENDERER_ACCELERATED );

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

  // Load texture
  if( !gModulatedTexture.loadFromFile( FilePath ) )
  {
    printf( "\nFailed to load sprite sheet texture \"%s\"!", FilePath.c_str() );
    success = false;
  }
  else
  {
    printf( "\nSprite sheet texture \"%s\" loaded", FilePath.c_str() );
    printf( "\nSetting sprite sheet clips" );
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gModulatedTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
  gRenderer = NULL;

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
  printf("\nProgram \"%s\" started with %d additional arguments.", args[0], argc - 1); // Il primo argomento è il nome dell'eseguibile

  for (int i = 1; i != argc; ++i)
  {
    printf("\nArgument #%d: %s\n", i, args[i]);
  }

  // Start up SDL and create window
  if( !init() )
  {
    printf( "Failed to initialize!\n" );
  }
  else
  {
    // Load media
    if( !loadMedia() )
    {
      printf( "Failed to load media!\n" );
    }
    else
    {
      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

      // Modulation components
      Uint8 r = static_cast<Uint8>(255);
      Uint8 g = static_cast<Uint8>(255);
      Uint8 b = static_cast<Uint8>(255);

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
          // On keypress change rgb values
          else if( e.type == SDL_KEYDOWN )
          {
            switch( e.key.keysym.sym )
            {
              // Increase red
              case SDLK_q:
                r = static_cast<Uint8>(r + INCREMENT);
                printf("\nRed increased by %d", INCREMENT);
                break;

              // Increase green
              case SDLK_w:
                g = static_cast<Uint8>(g + INCREMENT);
                printf("\nGreen increased by %d", INCREMENT);
                break;

              // Increase blue
              case SDLK_e:
                b = static_cast<Uint8>(b + INCREMENT);
                printf("\nBlue increased by %d", INCREMENT);
                break;

              // Decrease red
              case SDLK_a:
                r = static_cast<Uint8>(r - INCREMENT);
                printf("\nRed decreased by %d", INCREMENT);
                break;

              // Decrease green
              case SDLK_s:
                g = static_cast<Uint8>(r - INCREMENT);
                printf("\nGreen decreased by %d", INCREMENT);
                break;

              // Decrease blue
              case SDLK_d:
                b = static_cast<Uint8>(r - INCREMENT);
                printf("\nBlue decreased by %d", INCREMENT);
                break;

              // Get current values - Si può riciclare r, g, b senza bisogno di dicharare altre variabili
              case SDLK_x:
                gModulatedTexture.getColor(r, g, b);
                printf("\nColours modulation:");
                printf("\n  - Red:   %u", static_cast<unsigned int>(r));
                printf("\n  - Green: %u", static_cast<unsigned int>(g));
                printf("\n  - Blue:  %u", static_cast<unsigned int>(b));
                break;

              // Reset values
              case SDLK_z:
                r = static_cast<Uint8>(255);
                g = static_cast<Uint8>(255);
                b = static_cast<Uint8>(255);
                printf("\nColours restored");
                break;

              default:
                break;
            }
          }
          else
          {;} // Continue main loop
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Modulate and render texture
        gModulatedTexture.setColor( r, g, b );
        gModulatedTexture.render( 0, 0 );

        // Update screen
        SDL_RenderPresent( gRenderer );
      }
    }
  }

  // Free resources and close SDL
  close();

  //  Integrity check
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