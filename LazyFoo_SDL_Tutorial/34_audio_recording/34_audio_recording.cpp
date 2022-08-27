/**
 * @file 34_audio_recording.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/34_audio_recording/index.php
 *
 * NOTA: [[maybe_unused]] è un attributo inserito nel C++17 che permette di sopprimere gli "unused
 * parameter" warnings.
 *
 * An SDL_AudioSpec is an audio specification which basically defines how audio is recorded or
 * played back. When we open an audio device for recording or playing, we request a specification,
 * but we may not get what we requested back because the audio driver does not support it. This is
 * why we are going to store the specification we get back from the driver for recording and
 * playback.
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

static const int MAX_RECORDING_DEVICES    = 10; // Maximum number of supported recording devices
static const int MAX_RECORDING_SECONDS    = 5;  // Maximum recording time
static const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_SECONDS + 1; // Maximum recording time plus padding

// The various recording actions we can take
enum RecordingState
{
  SELECTING_DEVICE,
  STOPPED,
  RECORDING,
  RECORDED,
  PLAYBACK,
  ERROR
};


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

// Recording/playback callbacks
static void audioRecordingCallback( void* userdata, Uint8* stream, int len );
static void audioPlaybackCallback ( void* userdata, Uint8* stream, int len );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer

// Globally used font
static TTF_Font* gFont = NULL;
static SDL_Color gTextColor = { BLACK_R, BLACK_G, BLACK_B, BLACK_A };

static LTexture gPromptTexture; // Prompt texture
static LTexture gDeviceTextures[ MAX_RECORDING_DEVICES ]; // The text textures that specify recording device names

// Number of available devices
static int gRecordingDeviceCount = 0;

// Received audio spec
static SDL_AudioSpec gReceivedRecordingSpec;
static SDL_AudioSpec gReceivedPlaybackSpec;

static Uint8* gRecordingBuffer         = NULL; // Recording data buffer
static Uint32 gBufferSize_Bytes        = 0;    // Size of data buffer
static Uint32 gBufferPosition_Bytes    = 0;    // Position in data buffer
static Uint32 gBufferMaxPosition_Bytes = 0;    // Maximum position in data buffer for recording


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
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
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

    // Set starting prompt
    gPromptTexture.loadFromRenderedText( "Select your recording device:", gTextColor );

    // Get capture device count
    gRecordingDeviceCount = SDL_GetNumAudioDevices( SDL_TRUE );

    // No recording devices
    if( gRecordingDeviceCount < 1 )
    {
      printf( "\nUnable to get audio capture device! SDL Error: \"%s\"\n", SDL_GetError() );
      success = false;
    }
    // At least one device connected
    else
    {
      // Cap recording device count
      if( gRecordingDeviceCount > MAX_RECORDING_DEVICES )
      {
        gRecordingDeviceCount = MAX_RECORDING_DEVICES;
      }
      else { /* No capping necessary */ }

      // Render device names
      std::stringstream promptText;

      for( int i = 0; i != gRecordingDeviceCount; ++i )
      {
        // Get capture device name
        promptText.str( "" );
        promptText << i << ": " << SDL_GetAudioDeviceName( i, SDL_TRUE );

        // Set texture from name
        gDeviceTextures[ i ].loadFromRenderedText( promptText.str().c_str(), gTextColor );
      }
    }
  }

  return success;
}


static void close(void)
{
  // Free textures
  gPromptTexture.free();
  for( int i = 0; i != MAX_RECORDING_DEVICES; ++i )
  {
    gDeviceTextures[ i ].free();
  }

  // Free global font
  TTF_CloseFont( gFont );
  gFont = NULL;

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gRenderer = NULL;
  gWindow   = NULL;

  // Free playback audio
  if( gRecordingBuffer != NULL )
  {
    delete[] gRecordingBuffer;
    gRecordingBuffer = NULL;
  }
  else
  { /* Nothing to free */ }

  // Quit SDL subsystems
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

void audioRecordingCallback([[maybe_unused]] void* userdata, Uint8* stream, int len )
{
  // Copy audio from stream
  memcpy( &gRecordingBuffer[ gBufferPosition_Bytes ], stream, len );

  // Move along buffer
  gBufferPosition_Bytes += len;
}


void audioPlaybackCallback([[maybe_unused]] void* userdata, Uint8* stream, int len )
{
  // Copy audio to stream
  memcpy( stream, &gRecordingBuffer[ gBufferPosition_Bytes ], len );

  // Move along buffer
  gBufferPosition_Bytes += len;
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
    HasProgramSucceeded = false;
  }
  else
  {
    printf( "\nAll systems initialised" );

    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!" );
      HasProgramSucceeded = false;
    }
    else
    {
      printf( "\nAll media loaded" );

      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

      // Set the default recording state
      RecordingState currentState = SELECTING_DEVICE;

      // Audio device IDs
      SDL_AudioDeviceID recordingDeviceId = 0;
      SDL_AudioDeviceID playbackDeviceId = 0;

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

          // Do current state event handling
          switch( currentState )
          {
            // User is selecting recording device
            case SELECTING_DEVICE:

              // On key press
              if( e.type == SDL_KEYDOWN )
              {
                // Handle key press from 0 to 9
                if( e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9 )
                {
                  // Get selection index
                  int index = e.key.keysym.sym - SDLK_0;

                  // Index is valid
                  if( index != gRecordingDeviceCount )
                  {
                    // Default audio spec
                    SDL_AudioSpec desiredRecordingSpec;
                    SDL_zero(desiredRecordingSpec);
                    desiredRecordingSpec.freq     = 44100;
                    desiredRecordingSpec.format   = AUDIO_F32;
                    desiredRecordingSpec.channels = 2;
                    desiredRecordingSpec.samples  = 4096;
                    desiredRecordingSpec.callback = audioRecordingCallback;

                    // Open recording device
                    recordingDeviceId = SDL_OpenAudioDevice( SDL_GetAudioDeviceName( index, SDL_TRUE ), SDL_TRUE, &desiredRecordingSpec, &gReceivedRecordingSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE );

                    // Device failed to open
                    if( recordingDeviceId == 0 )
                    {
                      // Report error
                      printf( "\nFailed to open recording device! SDL Error: \"%s\"", SDL_GetError() );
                      gPromptTexture.loadFromRenderedText( "Failed to open recording device!", gTextColor );
                      currentState = ERROR;
                      HasProgramSucceeded = false;
                    }
                    // Device opened successfully
                    else
                    {
                      // Default audio spec
                      SDL_AudioSpec desiredPlaybackSpec;
                      SDL_zero(desiredPlaybackSpec);
                      desiredPlaybackSpec.freq     = 44100;
                      desiredPlaybackSpec.format   = AUDIO_F32;
                      desiredPlaybackSpec.channels = 2;
                      desiredPlaybackSpec.samples  = 4096;
                      desiredPlaybackSpec.callback = audioPlaybackCallback;

                      // Open playback device
                      playbackDeviceId = SDL_OpenAudioDevice( NULL, SDL_FALSE, &desiredPlaybackSpec, &gReceivedPlaybackSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE );

                      // Device failed to open
                      if( playbackDeviceId == 0 )
                      {
                        // Report error
                        printf( "\nFailed to open playback device! SDL Error: \"%s\"", SDL_GetError() );
                        gPromptTexture.loadFromRenderedText( "Failed to open playback device!", gTextColor );
                        currentState = ERROR;
                        HasProgramSucceeded = false;
                      }
                      // Device opened successfully
                      else
                      {
                        // Calculate per sample bytes
                        int bytesPerSample = gReceivedRecordingSpec.channels * ( SDL_AUDIO_BITSIZE( gReceivedRecordingSpec.format ) / 8 );

                        // Calculate bytes per second
                        int bytesPerSecond = gReceivedRecordingSpec.freq * bytesPerSample;

                        // Calculate buffer size
                        gBufferSize_Bytes = RECORDING_BUFFER_SECONDS * bytesPerSecond;

                        // Calculate max buffer use
                        gBufferMaxPosition_Bytes = MAX_RECORDING_SECONDS * bytesPerSecond;

                        // Allocate and initialize byte buffer
                        gRecordingBuffer = new Uint8[ gBufferSize_Bytes ];
                        memset( gRecordingBuffer, 0, gBufferSize_Bytes );

                        // Go on to next state
                        gPromptTexture.loadFromRenderedText("Press 1 to record for 5 seconds.", gTextColor);
                        currentState = STOPPED;
                      }
                    }
                  }
                  else
                  {
                    printf( "\nError: unrecognised command" );
                    return EXIT_FAILURE;
                  }
                }
              }
              break;

            // User getting ready to record
            case STOPPED:

              // On key press
              if( e.type == SDL_KEYDOWN )
              {
                // Start recording
                if( e.key.keysym.sym == SDLK_1 )
                {
                  // Go back to beginning of buffer
                  gBufferPosition_Bytes = 0;

                  // Start recording
                  SDL_PauseAudioDevice( recordingDeviceId, SDL_FALSE );

                  // Go on to next state
                  gPromptTexture.loadFromRenderedText( "Recording...", gTextColor );
                  currentState = RECORDING;
                }
                else {;}
              }
              else {;}
              break;

            // User has finished recording
            case RECORDED:

              // On key press
              if( e.type == SDL_KEYDOWN )
              {
                // Start playback
                if( e.key.keysym.sym == SDLK_1 )
                {
                  // Go back to beginning of buffer
                  gBufferPosition_Bytes = 0;

                  // Start playback
                  SDL_PauseAudioDevice( playbackDeviceId, SDL_FALSE );

                  // Go on to next state
                  gPromptTexture.loadFromRenderedText( "Playing...", gTextColor );
                  currentState = PLAYBACK;
                }
                // Record again
                if( e.key.keysym.sym == SDLK_2 )
                {
                  // Reset the buffer
                  gBufferPosition_Bytes = 0;
                  memset( gRecordingBuffer, 0, gBufferSize_Bytes );

                  // Start recording
                  SDL_PauseAudioDevice( recordingDeviceId, SDL_FALSE );

                  // Go on to next state
                  gPromptTexture.loadFromRenderedText( "Recording...", gTextColor );
                  currentState = RECORDING;
                }
              }
              break;

            case RECORDING:
            case PLAYBACK:
            case ERROR:
            default:
            HasProgramSucceeded = false;
            /* Error: not managed in this switch-case */
          }
        }

        // Updating recording
        if( currentState == RECORDING )
        {
          // Lock callback
          SDL_LockAudioDevice( recordingDeviceId );

          // Finished recording
          if( gBufferPosition_Bytes > gBufferMaxPosition_Bytes )
          {
            // Stop recording audio
            SDL_PauseAudioDevice( recordingDeviceId, SDL_TRUE );

            // Go on to next state
            gPromptTexture.loadFromRenderedText( "Press 1 to play back. Press 2 to record again.", gTextColor );
            currentState = RECORDED;
          }
          else { /* Not finished recording yet */ }

          // Unlock callback
          SDL_UnlockAudioDevice( recordingDeviceId );
        }
        // Updating playback
        else if( currentState == PLAYBACK )
        {
          // Lock callback
          SDL_LockAudioDevice( playbackDeviceId );

          // Finished playback
          if( gBufferPosition_Bytes > gBufferMaxPosition_Bytes )
          {
            // Stop playing audio
            SDL_PauseAudioDevice( playbackDeviceId, SDL_TRUE );

            // Go on to next state
            gPromptTexture.loadFromRenderedText( "Press 1 to play back. Press 2 to record again.", gTextColor );
            currentState = RECORDED;
          }
          else { /* Still playing back */ }

          // Unlock callback
          SDL_UnlockAudioDevice( playbackDeviceId );
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render prompt centered at the top of the screen
        gPromptTexture.render( ( SCREEN_W - gPromptTexture.getWidth() ) / 2, 0 );

        // User is selecting
        if( currentState == SELECTING_DEVICE )
        {
          // Render device names
          int yOffset = gPromptTexture.getHeight() * 2;

          for( int i = 0; i != gRecordingDeviceCount; ++i )
          {
            gDeviceTextures[ i ].render( 0, yOffset );
            yOffset += gDeviceTextures[ i ].getHeight() + 1;
          }
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