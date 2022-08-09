/**
 * @file 17_mouse_events.cpp
 *
 * @brief As with key presses, SDL has event structures to handle mouse events such as mouse
 * motion, mouse button presses, and mouse button releasing. In this tutorial we'll make a bunch of
 * buttons we can interact with.
 *
 * For this tutorial we'll have 4 buttons on the screen. Depending on whether the mouse moved over,
 * clicked on, released on, or moved out of the button we'll display a different sprite. The
 * constants and enumeratives are there to define all this.
 *
 * We're making a slight modification to the texture class. For this tutorial we won't be using
 * SDL_ttf to render text. This means we don't need the loadFromRenderedText function. Rather than
 * deleting code we may need in the future, we're going to wrap it in "#if defined" statements, so
 * the compiler will ignore it if we do not include SDL_ttf. It checks if the SDL_TTF_MAJOR_VERSION
 * macro is defined. Like #include, #if is a macro which is used to talk to the compiler. In this
 * case it says if SDL_ttf is not defined, ignore this piece of code.
 *
 * To make sure our source compiles without SDL_ttf, we sandwich the loading from font function
 * "loadFromRenderedText" in another "#if defined" condition.
 * 
 * "LButton::handleEvent" is where we handle the mouse events. This function will be called in
 * the event loop and will handle an event taken from the event queue for an individual button.
 * 
 * First we check if the event coming in is a mouse event specifically, a mouse motion event (when
 * the mouse moves), a mouse button down event (when you click a mouse button), or a mouse button up
 * event (when you release a mouse click).
 * 
 * If one of these mouse events do occur, we check the mouse position using SDL_GetMouseState.
 * Depending on whether the mouse is over the button or not, we'll want to display different
 * sprites.
 * 
 * Then, we want to check if the mouse is inside the button or not. Since we use a different
 * coordinate system with SDL, the origin of the button is at the top left. This means every x
 * coordinate less than the x position is outside of the button and every y coordinate less than the
 * y position is too. Everything right of the button is the x position + the width and everything
 * below the button is the y position + the height. If the mouse position is in any way outside the
 * button, it marks the inside marker as false. Otherwise it remains the initial true value.
 * 
 * Finally, we set the button sprite depending on whether the mouse is inside the button and the
 * mouse event. If the mouse isn't inside the button, we set the mouse out sprite. If the mouse is
 * inside we set the sprite to mouse over on a mouse motion, mouse down on a mouse button press, and
 * mouse up on a mouse button release.
 * 
 * In the rendering function, we just render the current button sprite at the button position.
 * 
 * In the event loop (in the main loop), we handle the quit event and the events for all the
 * buttons. In the rendering section, all the buttons are rendered to the screen.
 *
 * There are also mouse wheel events which weren't covered here, but if you look at the
 * documentation and play around with it it shouldn't be too hard to figure out.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int INIT_FIRST_ONE_AVAILABLE = -1;

static constexpr int SCREEN_W = 640; // Screen's width
static constexpr int SCREEN_H = 480; // Screen's heigth

// Button constants
static constexpr int BUTTON_W      = 300;
static constexpr int BUTTON_H      = 200;
static constexpr int TOTAL_BUTTONS = 4;

// Colore bianco
static constexpr int WHITE_R = 0xFF; // Amount of red   needed to compose white
static constexpr int WHITE_G = 0xFF; // Amount of green needed to compose white
static constexpr int WHITE_B = 0xFF; // Amount of blue  needed to compose white
static constexpr int WHITE_A = 0xFF; // Alpha component

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan

static const std::string FilePath("button.png");


/***************************************************************************************************
* Private Types
****************************************************************************************************/

enum LButtonSprite
{
  BUTTON_SPRITE_MOUSE_OUT         = 0,
  BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
  BUTTON_SPRITE_MOUSE_DOWN        = 2,
  BUTTON_SPRITE_MOUSE_UP          = 3,
  BUTTON_SPRITE_TOTAL             = 4
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


/**
 * @brief This class represents a button. It has a constructor to initialize, a position setter, an
 * event handler for the event loop, and a rendering function. It also has a position and a sprite
 * enumeration, so we know which sprite to render for the button.
 **/
class LButton
{
  public:
    // Initializes internal variables
    LButton(void);

    // Sets top left position
    void setPosition( int, int );

    // Handles mouse event
    void handleEvent( SDL_Event* );

    // Shows button sprite
    void render(void);

  private:
    // Top left position
    SDL_Point mPosition;

    // Currently used global sprite
    LButtonSprite mCurrentSprite;
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

// Mouse button sprites
static SDL_Rect gSpriteClips[ BUTTON_SPRITE_TOTAL ];
static LTexture gButtonSpriteSheetTexture;

// Buttons objects
static LButton gButtons[ TOTAL_BUTTONS ];


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
  else
  {;}
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


LButton::LButton(void)
{
  mPosition.x = 0;
  mPosition.y = 0;

  mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}


void LButton::setPosition( int x, int y )
{
  mPosition.x = x;
  mPosition.y = y;
}


void LButton::handleEvent( SDL_Event* e )
{
  // If mouse event happened
  if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
  {
    // Get mouse position
    int x, y;
    SDL_GetMouseState( &x, &y );

    // Check if mouse is inside button
    bool inside = true;

    // Mouse is left of the button
    if( x < mPosition.x )
    {
      inside = false;
    }
    // Mouse is right of the button
    else if( x > mPosition.x + BUTTON_W )
    {
      inside = false;
    }
    // Mouse above the button
    else if( y < mPosition.y )
    {
      inside = false;
    }
    // Mouse below the button
    else if( y > mPosition.y + BUTTON_H )
    {
      inside = false;
    }
    else
    {;} // Mouse is inside the button

    // Mouse is outside button
    if( !inside )
    {
      mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
    }
    // Mouse is inside button
    else
    {
      // Set mouse over sprite
      switch( e->type )
      {
        case SDL_MOUSEMOTION:
        mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
        break;

        case SDL_MOUSEBUTTONDOWN:
        mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
        break;

        case SDL_MOUSEBUTTONUP:
        mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
        break;
      }
    }
  }
  else
  {;} // No mouse event happened
}


void LButton::render(void)
{
  // Show current button sprite
  gButtonSpriteSheetTexture.render( mPosition.x, mPosition.y, &gSpriteClips[ mCurrentSprite ] );
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

  // Load sprites
  if( !gButtonSpriteSheetTexture.loadFromFile( FilePath ) )
  {
    printf( "Failed to load button sprite texture!\n" );
    success = false;
  }
  else
  {
    // Set sprites (by clipping the full sprite sheet - GS)
    for( int i = 0; i < BUTTON_SPRITE_TOTAL; ++i )
    {
      gSpriteClips[ i ].x = 0;
      gSpriteClips[ i ].y = i * BUTTON_H;
      gSpriteClips[ i ].w = BUTTON_W;
      gSpriteClips[ i ].h = BUTTON_H;
    }

    // Set buttons in corners
    gButtons[ 0 ].setPosition( 0                  , 0                   );
    gButtons[ 1 ].setPosition( SCREEN_W - BUTTON_W, 0                   );
    gButtons[ 2 ].setPosition( 0                  , SCREEN_H - BUTTON_H );
    gButtons[ 3 ].setPosition( SCREEN_W - BUTTON_W, SCREEN_H - BUTTON_H );
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gButtonSpriteSheetTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gRenderer = NULL;
  gWindow   = NULL;

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

          // Handle button events
          for( int i = 0; i < TOTAL_BUTTONS; ++i )
          {
            gButtons[ i ].handleEvent( &e );
          }
        }

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render buttons
        for( int i = 0; i < TOTAL_BUTTONS; ++i )
        {
          gButtons[ i ].render();
        }

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