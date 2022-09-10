/**
 * @file 46_multithreading.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/46_multithreading/index.php
 *
 * Multithreading can be used to make your program execute two things at once and take advantage of
 * multithreaded architectures. Here we'll make a simple program that outputs to the console while
 * the main thread runs.
 *
 * Just like with callback functions, thread functions need to be declared a certain way. They need
 * to take in a void pointer as an argument and return an integer.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/**************************************************************************************************
* Includes
***************************************************************************************************/

// Using SDL, SDL Threads, SDL_image, standard I/O, and, strings
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "colours.hpp"


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int FIRST_ONE       = -1;
static constexpr int WINDOW_W        = 640; // Screen's width
static constexpr int WINDOW_H        = 480; // Screen's heigth
static constexpr int TRANSPARENCY    = 0x00;
static constexpr int BYTES_PER_PIXEL = 4;

static std::string Path("splash.png");

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

	// Creates blank texture
	bool createBlank( int, int, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING );

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

	// Set self as render target
	void setAsRenderTarget(void);

	// Gets image dimensions
	int getWidth ( void ) const;
	int getHeight( void ) const;

	// Pixel manipulators
	bool   lockTexture   ( void  );
	bool   unlockTexture ( void  );
	void*  getPixels     ( void  ) const;
	void   copyPixels    ( void* );
	int    getPitch      ( void  ) const;
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


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init      (void);
static bool loadMedia (void);
static void close     (void);

static int threadFunction( void* );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer

// Scene textures
LTexture gSplashTexture;


/***************************************************************************************************
* Methods definitions
****************************************************************************************************/

LTexture::LTexture(void)
{
	// Initialize
	mTexture = NULL;
  mWidth   = 0;
  mHeight  = 0;
  mPixels  = NULL;
  mPitch   = 0;
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
        Uint32 transparent = SDL_MapRGBA( formattedSurface->format, CYAN_R, CYAN_G, CYAN_B, TRANSPARENCY );

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


bool LTexture::createBlank( int width, int height, SDL_TextureAccess access )
{
	// Create uninitialized texture
	mTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height );

	if( mTexture == NULL )
	{
    printf( "\nUnable to create blank texture! SDL Error: \"%s\"", SDL_GetError() );
	}
	else
	{
    printf( "\nOK: blank texture created" );
		mWidth = width;
		mHeight = height;
	}

	return mTexture != NULL;
}


void LTexture::free( void )
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


void LTexture::setAsRenderTarget(void)
{
	// Make self render target
	SDL_SetRenderTarget( gRenderer, mTexture );
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
		if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
		{
      printf( "\nUnable to lock texture! %s\n", SDL_GetError() );
			success = false;
		}
		else {;}
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


/**
 * @brief Copy the pixels from the stream into the texture
 *
 * @param pixels
 **/
void LTexture::copyPixels( void* pixels )
{
	// Texture is locked
	if( mPixels != NULL )
	{
		// Copy to locked pixels
		memcpy( mPixels, pixels, mPitch * mHeight );
	}
  else { /* Cannot copy */ }
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
    return pixels[ ( y * ( mPitch / 4 ) ) + x ];
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
		srand( SDL_GetTicks() );

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

	// Load splash texture
	if( !gSplashTexture.loadFromFile( Path ) )
	{
    printf( "\nFailed to load splash texture!" );
		success = false;
  }
	else
	{
		printf( "\nOK: splash texture created" );
	}

	return success;
}


static void close(void)
{
	// Free loaded images
	gSplashTexture.free();

	// Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow	 ( gWindow );
	gRenderer = NULL;
	gWindow   = NULL;

	// Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}


/**
 * @brief Our test thread function.
 *
 * @param data
 * @return int
 **/
static int threadFunction( void* data )
{
	// Print incoming data

	// printf( "Running thread with value = %d\n", (int)data );
	printf( "\nRunning thread with value = %llu", (intptr_t)data );

	return 0;
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

			// Run the thread
			int 				data 		 = 101;
			SDL_Thread* threadID = SDL_CreateThread( threadFunction, "LazyThread", (void*)data );

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
				SDL_RenderClear( gRenderer );

				// Render prompt
				gSplashTexture.render( 0, 0 );

				// Update screen
				SDL_RenderPresent( gRenderer );
			}

			// Make sure the thread finishes before the application closes
			SDL_WaitThread( threadID, NULL );
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