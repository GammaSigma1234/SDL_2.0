/**
 * @file 32_text_input_and_clipboard_handling.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/32_text_input_and_clipboard_handling/index.php
 *
 * Getting text input from the keyboard is a common task in games. Here we'll be getting text using
 * SDL 2's new text input and clipboard handling.
 *
 * Before we go into the main loop we declare a string to hold our text and render it to a texture.
 * We then call SDL_StartTextInput so the SDL text input functionality is enabled.
 *
 * We only want to update the input text texture when we need to, so we have the "renderText" flag
 * that keeps track of whether we need to update the texture.
 *
 * There are a couple special key presses we want to handle:
 *  - When the user presses backspace we want to remove the last character from the string.
 *  - When the user is holding control and presses c, we want to copy the current text to the clip
 *    board using SDL_SetClipboardText. You can check if the CTRL key is being held using
 *    SDL_GetModState.
 *  - When the user does CTRL + C, we want to get the text from the clip board using
 *    SDL_GetClipboardText. Also notice that whenever we alter the contents of the string we set the
 *    text update flag.
 *
 * With text input enabled, your key presses will also generate SDL_TextInputEvents, which
 * simplifies things like shift key and caps lock. Here we first want to check that we're not
 * getting a CTRL and C/V event, because we want to ignore those since they are already handled as
 * keydown events. If it isn't a copy or paste event, we append the character to our input string.
 *
 * If the text render update flag has been set, we rerender the texture. One little hack we have
 * here is if we have an empty string, we render a space because SDL_ttf will not render an empty
 * string.
 *
 * At the end of the main loop we render the prompt text and the input text. Once we're done with
 * text input we disable it, since enabling text input introduces some overhead.
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

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan
static constexpr int CYAN_A = 0xFF; // Alpha component

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


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer

// Globally used font
static TTF_Font *gFont = NULL;

// Scene textures
static LTexture gPromptTextTexture;
static LTexture gInputTextTexture;


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
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
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

    // Render the prompt
    SDL_Color textColor = { BLACK_R, BLACK_G, BLACK_B, BLACK_A };

    if( !gPromptTextTexture.loadFromRenderedText( "Enter Text:", textColor ) )
    {
      printf( "\nFailed to render prompt text!\n" );
      success = false;
    }
    else
    {
      printf( "\nPrompt text rendered\n" );
    }
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gPromptTextTexture.free();
  gInputTextTexture.free();

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
  printf("\nProgram started with %d additional arguments.", argc - 1); // Il primo argomento è il nome dell'eseguibile

  for (int i = 1; i != argc; ++i)
  {
    printf("\nArgument #%d: %s\n", i, args[i]);
  }

  // Start up SDL and create window
  if( !init() )
  {
    printf( "\nFailed to initialize!" );
  }
  else
  {
    printf( "\nAll systems initialised" );

    // Load media
    if( !loadMedia() )
    {
      printf( "\nFailed to load media!" );
    }
    else
    {
      printf( "\nAll media loaded" );

      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

      // Set text color as black
      SDL_Color textColor = { BLACK_R, BLACK_G, BLACK_B, BLACK_A };

      // The current input text
      std::string inputText = "Some Text";
      gInputTextTexture.loadFromRenderedText( inputText.c_str(), textColor );

      // Enable text input
      SDL_StartTextInput();

      // While application is running
      while( !quit )
      {
        // The rerender text flag
        bool renderText = false;

        // Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
          // User requests quit
          if( e.type == SDL_QUIT )
          {
            quit = true;
          }
          // Special key input
          else if( e.type == SDL_KEYDOWN )
          {
            // Handle backspace
            if( e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0 )
            {
              // lop off character
              inputText.pop_back();
              renderText = true;
            }
            // Handle copy
            else if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
            {
              SDL_SetClipboardText( inputText.c_str() );
            }
            // Handle paste
            else if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
            {
              inputText = SDL_GetClipboardText();
              renderText = true;
            }
          }
          // Special text input event
          else if( e.type == SDL_TEXTINPUT )
          {
            // Not copy or pasting
            if( !( SDL_GetModState() & KMOD_CTRL && ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' || e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) ) )
            {
              // Append character
              inputText += e.text.text;
              renderText = true;
            }
          }
        }

        // Rerender text if needed
        if( renderText )
        {
          // Text is not empty
          if( inputText != "" )
          {
            // Render new text
            gInputTextTexture.loadFromRenderedText( inputText.c_str(), textColor );
          }
          // Text is empty
          else
          {
            // Render space texture
            gInputTextTexture.loadFromRenderedText( " ", textColor );
          }
        }
        else { /* No need to render text */ }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render text textures
        gPromptTextTexture.render( ( SCREEN_W - gPromptTextTexture.getWidth() ) / 2, 0 );
        gInputTextTexture.render(  ( SCREEN_W - gInputTextTexture.getWidth()  ) / 2, gPromptTextTexture.getHeight() );

        // Update screen
        SDL_RenderPresent( gRenderer );
      }

      // Disable text input
      SDL_StopTextInput();

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