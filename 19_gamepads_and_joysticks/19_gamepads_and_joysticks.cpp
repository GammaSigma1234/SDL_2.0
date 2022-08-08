/**
 * @file 19_gamepads_and_joysticks.cpp
 *
 * @brief
 *
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

static constexpr int INIT_FIRST_ONE_AVAILABLE = -1;

static constexpr int SCREEN_W = 640; // Screen's width
static constexpr int SCREEN_H = 480; // Screen's heigth

// Analog joystick dead zone
static const int JOYSTICK_DEAD_ZONE = 8000;

// Colore bianco
static constexpr int WHITE_R = 0xFF; // Amount of red   needed to compose white
static constexpr int WHITE_G = 0xFF; // Amount of green needed to compose white
static constexpr int WHITE_B = 0xFF; // Amount of blue  needed to compose white
static constexpr int WHITE_A = 0xFF; // Alpha component

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan

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
    bool loadFromFile( const std::string& );

		#if defined(SDL_TTF_MAJOR_VERSION)
		// Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
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

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void PressEnter(void);


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer

static LTexture gArrowTexture; // Scene textures

static SDL_Joystick* gGameController = NULL; // Game Controller 1 handler


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
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			// Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		// Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
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
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	// Modulate texture rgb
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

static bool init(void)
{
	// Initialization flag
	bool success = true;

	// Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
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

		// Check for joysticks
		if( SDL_NumJoysticks() < 1 )
		{
			printf( "\nWarning: No joysticks connected!" );
		}
		else
		{
			// Load joystick
			gGameController = SDL_JoystickOpen( 0 );
			if( gGameController == NULL )
			{
				printf( "\nWarning: Unable to open game controller! SDL Error: %s", SDL_GetError() );
			}
		}

		// Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN );

		if( gWindow == NULL )
		{
      printf( "\nWindow could not be created! SDL Error: %s", SDL_GetError() );
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

	// Load arrow texture
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

	// Close game controller
	SDL_JoystickClose( gGameController );
	gGameController = NULL;

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

			// Normalized direction
			int xDir = 0;
			int yDir = 0;

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
					else if( e.type == SDL_JOYAXISMOTION )
					{
						// Motion on controller 0
						if( e.jaxis.which == 0 )
						{
							// X axis motion
							if( e.jaxis.axis == 0 )
							{
								// Left of dead zone
								if( e.jaxis.value < -JOYSTICK_DEAD_ZONE )
								{
									xDir = -1;
								}
								// Right of dead zone
								else if( e.jaxis.value > JOYSTICK_DEAD_ZONE )
								{
									xDir = 1;
								}
								else
								{
									xDir = 0;
								}
							}
							// Y axis motion
							else if( e.jaxis.axis == 1 )
							{
								// Below of dead zone
								if( e.jaxis.value < -JOYSTICK_DEAD_ZONE )
								{
									yDir = -1;
								}
								// Above of dead zone
								else if( e.jaxis.value > JOYSTICK_DEAD_ZONE )
								{
									yDir =  1;
								}
								else
								{
									yDir = 0;
								}
							}
						}
					}
				}

				// Clear screen
				SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
				SDL_RenderClear( gRenderer );

				// Calculate angle
				double joystickAngle = atan2( (double)yDir, (double)xDir ) * ( 180.0 / M_PI );

				// Correct angle
				if( xDir == 0 && yDir == 0 )
				{
					joystickAngle = 0;
				}
				else
				{;}

				// Render joystick 8 way angle
        int CENTERED_HORIZONTALLY = ( SCREEN_W - gArrowTexture.getWidth()  ) / 2;
        int CENTERED_VERTICALLY   = ( SCREEN_H - gArrowTexture.getHeight() ) / 2;
				gArrowTexture.render( CENTERED_HORIZONTALLY, CENTERED_VERTICALLY, NULL, joystickAngle );

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