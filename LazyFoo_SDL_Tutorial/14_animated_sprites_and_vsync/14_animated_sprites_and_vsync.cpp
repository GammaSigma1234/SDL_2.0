/**
 * @file 14_animated_sprites_and_vsync.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/14_animated_sprites_and_vsync/index.php
 *
 * @brief Animation in a nutshell is just showing one image after another to create the illusion of
 * motion. Here we'll be showing different sprites to animate a stick figure.
 *
 * For this (and future tutorials), we want to use Vertical Sync. VSync allows the rendering to
 * update at the same time as when your monitor updates during vertical refresh. For this tutorial
 * it will make sure the animation doesn't run too fast. Most monitors run at about 60 frames per
 * second and that's the assumption we're making here. If you have a different monitor refresh rate,
 * that would explain why the animation is running too fast or slow.
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

static constexpr int INITIALISE_FIRST_ONE_AVAILABLE = -1;
static constexpr int WALKING_ANIMATION_FRAMES       =  4;
static constexpr int SLOWING_FACTOR                 =  5; // Fattore di rallentamento dell'animazione. Cambia sprite ogni SLOWING_FACTOR aggiornamenti dello schermo.

static constexpr int WINDOW_W = 640;
static constexpr int WINDOW_H = 480;

// Colore bianco
static constexpr int WHITE_R = 0xFF;
static constexpr int WHITE_G = 0xFF;
static constexpr int WHITE_B = 0xFF;
static constexpr int WHITE_A = 0xFF;

// Colore ciano
static constexpr int CYAN_R = 0x00;
static constexpr int CYAN_G = 0xFF;
static constexpr int CYAN_B = 0xFF;

static const std::string SpriteSheetPath("foo.png");


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
		void render( int x, int y, SDL_Rect* clip = NULL );

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

// Walking animation
static SDL_Rect gSpriteClips[ WALKING_ANIMATION_FRAMES ]; // Ogni sprite è largo 64 px e alto 205 px
static LTexture gSpriteSheetTexture;


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

	// Load sprite sheet texture
	if( !gSpriteSheetTexture.loadFromFile( SpriteSheetPath ) )
	{
		printf( "\nFailed to load walking animation texture!" );
		success = false;
	}
	else
	{
    printf( "\nWalking animation texture loaded" );

		// Set sprite clips
		gSpriteClips[ 0 ].x =   0;
		gSpriteClips[ 0 ].y =   0;
		gSpriteClips[ 0 ].w =  64;
		gSpriteClips[ 0 ].h = 205;

		gSpriteClips[ 1 ].x =  64;
		gSpriteClips[ 1 ].y =   0;
		gSpriteClips[ 1 ].w =  64;
		gSpriteClips[ 1 ].h = 205;

		gSpriteClips[ 2 ].x = 128;
		gSpriteClips[ 2 ].y =   0;
		gSpriteClips[ 2 ].w =  64;
		gSpriteClips[ 2 ].h = 205;

		gSpriteClips[ 3 ].x = 192;
		gSpriteClips[ 3 ].y =   0;
		gSpriteClips[ 3 ].w =  64;
		gSpriteClips[ 3 ].h = 205;
	}

	return success;
}


static void close(void)
{
	// Free loaded images
	gSpriteSheetTexture.free();

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
  printf("\nProgram started with %d additional arguments.", argc - 1); // Il primo argomento è il nome dell'eseguibile

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

			// Current animation frame
			int CurrentFrame = 0;

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
          {;}
				}

				// Clear screen
				SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
				SDL_RenderClear( gRenderer );

				// Render current frame
				SDL_Rect* currentClip     = &gSpriteClips[ CurrentFrame / SLOWING_FACTOR ];
        int CENTERED_HORIZONTALLY = ( WINDOW_W  - currentClip->w ) / 2;
        int CENTERED_VERTICALLY   = ( WINDOW_H - currentClip->h ) / 2;
				gSpriteSheetTexture.render( CENTERED_HORIZONTALLY, CENTERED_VERTICALLY, currentClip );

				// Update screen
				SDL_RenderPresent( gRenderer );

				// Go to next frame
				++CurrentFrame;

				// Cycle animation
				if( CurrentFrame / SLOWING_FACTOR >= WALKING_ANIMATION_FRAMES )
				{
					CurrentFrame = 0;
				}
        else
        {;}
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