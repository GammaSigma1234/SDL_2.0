/**
 * @file 13_alpha_blending.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/13_alpha_blending/index.php
 *
 * @brief Thanks to new hardware accelerated rendering, transparency is much faster in SDL 2.0. Here
 * we'll use alpha modulation (which works much like color modulation) to control the transparency
 * of a texture.
 *
 * "setAlpha" is similar to "setColor" in the color modulation tutorial. "setBlendMode" controls how
 * the texture is blended. In order to get blending to work properly, you must set the blend mode on
 * the texture.
 *
 * In the texture loading function we're loading the front texture we're going to alpha blend, and a
 * background texture. As the front texture gets more transparent, we'll be able to see more of the
 * back texture. After we load the front texture successfully we set the SDL BlendMode to blend, so
 * blending is enabled. Since the background isn't going to be transparent, we don't have to set the
 * blending on it.
 *
 * Alpha is opacity, and the lower the opacity the more we can see through it. Like red, green, or
 * blue color components, it goes from 0 to 255 when modulating it. 100% opacity (255) = completely
 * visible. 0% opacity (0) = completely invisible.
 *
 * "SDL_SetTextureBlendMode" in "setBlendMode" allows us to enable blending and
 * "SDL_SetTextureAlphaMod" allows us to set the amount of alpha for the whole texture.
 *
 * Right before entering the main loop, we declare a variable to control how much alpha the texture
 * has. It is initialized to 255 so the front texture starts out completely opaque.
 *
 * At the end of the main loop we do our rendering. After clearing the screen we render the
 * background first and then we render the front modulated texture over it. Right before rendering
 * the front texture, we set its alpha value.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
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

// Colore bianco (Inizializzazione renderer)
static constexpr int WHITE_R = 0xFF;
static constexpr int WHITE_G = 0xFF;
static constexpr int WHITE_B = 0xFF;
static constexpr int WHITE_A = 0xFF;

// Colore ciano
static constexpr int CYAN_R = 0x00;
static constexpr int CYAN_G = 0xFF;
static constexpr int CYAN_B = 0xFF;

static constexpr Uint8 INCREMENT = static_cast<Uint8>(8);
static constexpr Uint8 MAX_ALPHA = static_cast<Uint8>(255);

static const std::string FrontTexture("fadeout.png");
static const std::string BackTexture("fadein.png");


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

		// Set blending
		void setBlendMode( SDL_BlendMode );

		// Set alpha modulation
		void setAlpha( Uint8 );

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

static SDL_Window*   gWindow   = NULL;   // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL;   // The window renderer
static LTexture      gModulatedTexture;  // Scene texture
static LTexture      gBackgroundTexture; // Scene texture


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

	// Load front alpha texture
	if( !gModulatedTexture.loadFromFile( FrontTexture ) )
	{
		printf( "\nFailed to load front texture!" );
		success = false;
	}
	else
	{
		printf( "\nFront texture loaded. Setting blend mode..." );

		// Set standard alpha blending
		gModulatedTexture.setBlendMode( SDL_BLENDMODE_BLEND );
	}

	// Load background texture
	if( !gBackgroundTexture.loadFromFile( BackTexture ) )
	{
		printf( "Failed to load background texture!\n" );
		success = false;
	}
	else
	{
		printf( "\nBack texture loaded" );
	}

	return success;
}


static void close(void)
{
	// Free loaded images
	gModulatedTexture.free();
	gBackgroundTexture.free();

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

			// Alpha modulation component
			Uint8 a = 255;

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
					// Handle key presses
					else if( e.type == SDL_KEYDOWN )
					{
						// Increase alpha on w
						if( e.key.keysym.sym == SDLK_w )
						{
							// Cap if over 255
							if( a + INCREMENT > MAX_ALPHA )
							{
								a = MAX_ALPHA;
							}
							// Increment otherwise
							else
							{
								a = static_cast<Uint8>( a + INCREMENT );
							}
						}
						// Decrease alpha on s
						else if( e.key.keysym.sym == SDLK_s )
						{
							// Cap if below 0
							if( a - INCREMENT < 0 )
							{
								a = static_cast<Uint8>(0);
							}
							// Decrement otherwise
							else
							{
								a = static_cast<Uint8>( a - INCREMENT );
							}
						}
					}
					else
					{;}
				}

				// Clear screen
				SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
				SDL_RenderClear( gRenderer );

				// Render background
				gBackgroundTexture.render( 0, 0 );

				// Render front blended
				gModulatedTexture.setAlpha( a );
				gModulatedTexture.render( 0, 0 );

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