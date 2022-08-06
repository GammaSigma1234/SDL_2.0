/**
 * @file 15_rotation_and_flipping.cpp
 *
 * @brief SDL 2's hardware accelerated texture rendering also gives us the ability to to do fast
 * image flipping and rotation.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, standard I/O, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int INITIALISE_FIRST_ONE_AVAILABLE = -1;
static constexpr int WALKING_ANIMATION_FRAMES       = 4;
static constexpr int SLOWING_FACTOR                 = 5; // Fattore di rallentamento dell'animazione. Cambia sprite ogni SLOWING_FACTOR aggiornamenti dello schermo.

static constexpr int SCREEN_WIDTH       = 640;
static constexpr int SCREEN_HEIGHT      = 480;

// Colore bianco (Inizializzazione renderer)
static constexpr int WHITE_RED_COMPONENT = 0xFF;
static constexpr int WHITE_GRN_COMPONENT = 0xFF;
static constexpr int WHITE_BLU_COMPONENT = 0xFF;
static constexpr int WHITE_LFA_COMPONENT = 0xFF;

// Colore ciano
static constexpr int CYAN_RED_COMPONENT = 0x00;
static constexpr int CYAN_GRN_COMPONENT = 0xFF;
static constexpr int CYAN_BLU_COMPONENT = 0xFF;

static const std::string FilePath("arrow.png");


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
    bool loadFromFile( const std::string& path );

		// Deallocates texture
    void free(void);

		// Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		// Set blending
		void setBlendMode( SDL_BlendMode blending );

		// Set alpha modulation
		void setAlpha( Uint8 alpha );

		// Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

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

// Scene texture
LTexture gArrowTexture;


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
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, CYAN_RED_COMPONENT, CYAN_GRN_COMPONENT, CYAN_BLU_COMPONENT ) );

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
  else
  {;}

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
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

		if( gWindow == NULL )
		{
      printf( "\nWindow could not be created! SDL Error: %s", SDL_GetError() );
			success = false;
		}
		else
		{
      printf( "\nWindow created" );

			// Create accelerated and vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, INITIALISE_FIRST_ONE_AVAILABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

			if( gRenderer == NULL )
			{
        printf( "\nRenderer could not be created! SDL Error: %s", SDL_GetError() );
				success = false;
			}
			else
			{
        printf( "\nRenderer created" );

				// Initialize renderer color
        SDL_SetRenderDrawColor( gRenderer, WHITE_RED_COMPONENT, WHITE_GRN_COMPONENT, WHITE_BLU_COMPONENT, WHITE_LFA_COMPONENT );

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

	// Load arrow
	if( !gArrowTexture.loadFromFile( FilePath ) )
	{
		printf( "Failed to load arrow texture!\n" );
		success = false;
	}

	return success;
}


static void close(void)
{
	// Free loaded images
	gArrowTexture.free();

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

			// Angle of rotation
			double degrees = 0;

			// Flip type
			SDL_RendererFlip flipType = SDL_FLIP_NONE;

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
							case SDLK_a:
							degrees -= 15;
							break;

							case SDLK_d:
							degrees += 15;
							break;

							case SDLK_q:
							flipType = SDL_FLIP_HORIZONTAL;
							break;

							case SDLK_w:
							flipType = SDL_FLIP_NONE;
							break;

							case SDLK_e:
							flipType = SDL_FLIP_VERTICAL;
							break;
						}
					}
				}

				// Clear screen
				SDL_SetRenderDrawColor( gRenderer, WHITE_RED_COMPONENT, WHITE_GRN_COMPONENT, WHITE_BLU_COMPONENT, WHITE_LFA_COMPONENT );
				SDL_RenderClear( gRenderer );

				// Render arrow
				gArrowTexture.render( ( SCREEN_WIDTH - gArrowTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gArrowTexture.getHeight() ) / 2, NULL, degrees, NULL, flipType );

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