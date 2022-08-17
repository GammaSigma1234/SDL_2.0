/**
 * @file 21_sound_effects_and_music.cpp
 *
 * @brief Up until now we've only been dealing with video and input. Most games made require some
 * sort of sound and here we'll be using SDL_mixer to play audio for us.
 *
 * SDL_mixer is a library we use to make audio playing easier (because it can get complicated). We
 * have to set it up just like we set up SDL_image. As before, it's just a matter of having the
 * header files, library files, and binary files in the right place with your compiler configured
 * to use them.
 *
 * The SDL_mixer data type for music is "Mix_Music", and one for short sounds is "Mix_Chunk". Here
 * we declare pointers for the music and sound effects we'll be using.
 *
 * Since we're using music and sound effects, we need to initialize audio along with video for this
 * demo when calling "SDL_Init".
 *
 * To initialize SDL_mixer we need to call "Mix_OpenAudio". The first argument sets the sound
 * frequency, and 44100 is a standard frequency that works on most systems. The second argument
 * determines the sample format, which again here we're using the default. The third argument is the
 * number of hardware channels, and here we're using 2 channels for stereo. The last argument is the
 * sample size, which determines the size of the chunks we use when playing sound. 2048 bytes (AKA 2
 * kilobyes) worked fine for me, but you may have to experiment with this value to minimize lag when
 * playing sounds. If there's any errors with SDL_mixer, they're reported with Mix_GetError.
 *
 * In "loadMedia" we load our splash texture and sound. To load music, we call "Mix_LoadMUS" and to
 * load sound effect we call "Mix_LoadWAV".
 * 
 * In "close", when we're done with audio and want to free it, we call "Mix_FreeMusic" to free music
 * and "Mix_FreeChunk" to free a sound effect. We call "Mix_Quit" to close down SDL_mixer.
 * 
 * In the event loop, we play a sound effect when the 1, 2, 3, or 4 keys are pressed. The way to
 * play a Mix_Chunk is by calling Mix_PlayChannel. The first argument is the channel you want to use
 * to play it. Since we don't care which channel it comes out of, we set the channel to negative 1
 * which will use the nearest available channel. The second argument is the sound effect and last
 * argument is the number of times to repeat the effect. We only want it to play once per button
 * press, so we have it repeat 0 times.
 * 
 * WARNING: in this case, a channel is NOT the same as a hardware channel that can represent the
 * left and right channel of a stereo system! Every sound effect that's played has a channel
 * associated with it. When you want to pause or stop an effect that is playing, you can halt its
 * channel.
 * 
 * For this demo, we want to play/pause the music on a 9 keypress and stop the music on a 0
 * keypress. When the 9 key pressed, we first check if the music is not playing with
 * Mix_PlayingMusic. If it isn't, we start the music with Mix_PlayMusic. The first argument is the
 * music we want to play and the last argument is the number of times to repeat it. Negative 1 is a
 * special value saying we want to loop it until it is stopped. If there is music playing, we check
 * if the music is paused using Mix_PausedMusic. If the music is paused, we resume it using
 * Mix_ResumeMusic. If the music is not paused we pause it using Mix_PauseMusic. When 0 is pressed,
 * we stop music if it's playing using Mix_HaltMusic.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

//  Using SDL, SDL_image, SDL_ttf, SDL_mixer, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>

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

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan

static constexpr int SAMPLING_FREQ   = 44100;
static constexpr int NUM_OF_CHANNELS = 2;
static constexpr int CHUNK_SIZE      = 2048;

static const std::string PromptPath ("prompt.png");
static const std::string BeatPath   ("beat.wav");
static const std::string ScratchPath("scratch.wav");
static const std::string HighPath   ("high.wav");
static const std::string MediumPath ("medium.wav");
static const std::string LowPath    ("low.wav");


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

static LTexture gPromptTexture; // Scene texture

static Mix_Music *gMusic = NULL; // The music that will be played

// The sound effects that will be used
static Mix_Chunk *gScratch = NULL;
static Mix_Chunk *gHigh    = NULL;
static Mix_Chunk *gMedium  = NULL;
static Mix_Chunk *gLow     = NULL;


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


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

static bool init(void)
{
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
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

         // Initialize SDL_mixer
        if( Mix_OpenAudio( SAMPLING_FREQ, MIX_DEFAULT_FORMAT, NUM_OF_CHANNELS, CHUNK_SIZE ) < 0 )
        {
          printf( "\nSDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError() );
          success = false;
        }
        else
        {
          printf( "\nSDL_mixer initialised" );
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

  // Load prompt texture
  if( !gPromptTexture.loadFromFile( PromptPath ) )
  {
    printf( "\nFailed to load prompt texture!" );
    success = false;
  }
  else
  {
    printf( "\nPrompt texture loaded" );;
  }

  // Load music
  gMusic = Mix_LoadMUS( BeatPath.c_str() );

  if( gMusic == NULL )
  {
    printf( "\nFailed to load beat music! SDL_mixer Error: %s", Mix_GetError() );
    success = false;
  }
  else
  {
    printf( "\nBeat music loaded" );
  }

  // Load sound effects
  gScratch = Mix_LoadWAV( ScratchPath.c_str() );

  if( gScratch == NULL )
  {
    printf( "\nFailed to load scratch sound effect! SDL_mixer Error: %s", Mix_GetError() );
    success = false;
  }
  else
  {
    printf( "\nScratch sound effect loaded" );
  }

  gHigh = Mix_LoadWAV( HighPath.c_str() );
  if( gHigh == NULL )
  {
    printf( "\nFailed to load high sound effect! SDL_mixer Error: %s", Mix_GetError() );
    success = false;
  }
  else
  {
    printf( "High sound effect loaded" );
  }

  gMedium = Mix_LoadWAV( MediumPath.c_str() );
  if( gMedium == NULL )
  {
    printf( "\nFailed to load medium sound effect! SDL_mixer Error: %s", Mix_GetError() );
    success = false;
  }
  else
  {
    printf( "Medium sound effect loaded" );
  }

  gLow = Mix_LoadWAV( LowPath.c_str() );
  if( gLow == NULL )
  {
    printf( "\nFailed to load low sound effect! SDL_mixer Error: %s", Mix_GetError() );
    success = false;
  }
  else
  {
    printf( "Low sound effect loaded" );
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gPromptTexture.free();

  // Free the sound effects
  Mix_FreeChunk( gScratch );
  Mix_FreeChunk( gHigh );
  Mix_FreeChunk( gMedium );
  Mix_FreeChunk( gLow );
  gScratch = NULL;
  gHigh = NULL;
  gMedium = NULL;
  gLow = NULL;

  // Free the music
  Mix_FreeMusic( gMusic );
  gMusic = NULL;

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gRenderer = NULL;
  gWindow   = NULL;

  // Quit SDL subsystems
  Mix_Quit();
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
          // Handle key press
          else if( e.type == SDL_KEYDOWN )
          {
            switch( e.key.keysym.sym )
            {
              // Play high sound effect
              case SDLK_1:
              Mix_PlayChannel( -1, gHigh, 0 ); // ATTENZIONE: in questo caso, il "canale" non è il canale fisico (sinistro / destro)!, bensì un concetto software!
              break;

              // Play medium sound effect
              case SDLK_2:
              Mix_PlayChannel( -1, gMedium, 0 );
              break;

              // Play low sound effect
              case SDLK_3:
              Mix_PlayChannel( -1, gLow, 0 );
              break;

              // Play scratch sound effect
              case SDLK_4:
              Mix_PlayChannel( -1, gScratch, 0 );
              break;

              case SDLK_9:
              // If there is no music playing
              if( Mix_PlayingMusic() == 0 )
              {
                // Play the music
                Mix_PlayMusic( gMusic, -1 );
              }
              // If music is being played
              else
              {
                // If the music is paused
                if( Mix_PausedMusic() == 1 )
                {
                  // Resume the music
                  Mix_ResumeMusic();
                }
                // If the music is playing
                else
                {
                  // Pause the music
                  Mix_PauseMusic();
                }
              }
              break;

              case SDLK_0:
              // Stop the music
              Mix_HaltMusic();
              break;
            }
          }
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        // Render prompt
        gPromptTexture.render( 0, 0 );

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