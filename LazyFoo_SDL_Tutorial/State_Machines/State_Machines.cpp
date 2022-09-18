/**
 * @file 06_state_machines.cpp
 *
 * @brief https://lazyfoo.net/articles/article06/index.php
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

// Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include "colours.hpp"

// Screen attributes
static constexpr int WINDOW_W   = 800;
static constexpr int WINDOW_H   = 600;
static constexpr int SCREEN_BPP = 32;
static constexpr int FRAMES_PER_SECOND = 60;

static const std::string DotPath      ("dot.bmp");
static const std::string LazyFontPath ("lazy.ttf");
static const std::string BGPath       ("introbg.png");
static const std::string TitlePath    ("titlebg.png");
static const std::string GreenOWPath  ("greenoverworld.png");
static const std::string BluePath     ("blue.bmp");
static const std::string RedPath      ("red.bmp");
static const std::string RedRoomPath  ("redroom.png");
static const std::string BlueRoomPath ("blueroom.png");


/***************************************************************************************************
* Classes
****************************************************************************************************/

/**
 * @brief Texture wrapper class
 **/
class LTexture
{
public:
  // Initializes variables
  LTexture(void);

  // Deallocates memory
  ~LTexture(void);

  // Loads image at specified path
  bool loadFromFile( std::string path );

#if defined(SDL_TTF_MAJOR_VERSION)
  // Creates image from font string
  bool loadFromRenderedText( std::string, SDL_Color );
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
  void setAsRenderTarget( void );

  // Gets image dimensions
  int getWidth  ( void ) const;
  int getHeight ( void ) const;

  // Pixel manipulators
  bool lockTexture  ( void );
  bool unlockTexture( void );
  void* getPixels   ( void );
  void copyPixels   ( void* );
  int getPitch      ( void );
  Uint32 getPixel32( unsigned int, unsigned int );

private:
  // The actual hardware texture
  SDL_Texture* mTexture;
  void*        mPixels;
  int          mPitch;

  // Image dimensions
  int mWidth;
  int mHeight;
};


/**
 * @brief The dot that will move around on the screen
 **/
class Dot
{
public:
  // The dimensions of the dot
  static constexpr int DOT_WIDTH  = 20;
  static constexpr int DOT_HEIGHT = 20;

  // Maximum axis velocity of the dot
  static const int DOT_MAX_VEL = 10;

  // Initializes the variables
  Dot(void);

  // Set dot position
  void set( int, int );

  // Takes key presses and adjusts the dot's velocity
  void handleEvent( SDL_Event& );

  // Moves the dot
  void move( int, int );

  // Shows the dot on the screen relative to the camera
  void render( SDL_Rect );

  // Gets the collision box
  SDL_Rect getCollider(void);

private:
  // Got collision box
  SDL_Rect mBox;

  // The velocity of the dot
  int mVelX, mVelY;
};


class House
{
public:
  // The house dimensions
  static constexpr int HOUSE_WIDTH  = 40;
  static constexpr int HOUSE_HEIGHT = 40;

  // Initializes variables
  House(void);

  // Sets the house's position/graphic
  void set( int, int, LTexture* );

  // Renders house relative to the camera
  void render( SDL_Rect );

  // Gets the collision box
  SDL_Rect getCollider(void);

private:
  // The house graphic
  LTexture* mHouseTexture;

  // The collision box of the house
  SDL_Rect mBox;
};


class Door
{
public:
  // The door dimensions
  static constexpr int DOOR_WIDTH  = 20;
  static constexpr int DOOR_HEIGHT = 40;

  // Initializes variables
  Door(void);

  // Sets the door position
  void set( int, int );

  // Shows the door
  void render(void);

  // Gets the collision box
  SDL_Rect getCollider(void);

private:
  // The area of the door
  SDL_Rect mBox;
};


/**
 * @brief Game state base (abstract) class
 **/
class GameState
{
public:
  // State transitions
  virtual bool enter( void ) = 0;
  virtual bool exit ( void ) = 0;

  // Main loop functions
  virtual void handleEvent( SDL_Event& ) = 0;
  virtual void update     ( void )       = 0;
  virtual void render     ( void )       = 0;

  // Make sure to call child destructors
  virtual ~GameState(){};
};


class IntroState : public GameState
{
public:
  // Static accessor
  static IntroState* get( void );

  // Transitions
  bool enter( void ) override;
  bool exit ( void ) override;

  // Main loop functions
  void handleEvent( SDL_Event& ) override;
  void update     ( void )       override;
  void render     ( void )       override;

private:
  // Static instance
  static IntroState sIntroState;

  // Private constructor
  IntroState( void );

  // Intro background
  LTexture mBackgroundTexture;

  // Intro message
  LTexture mMessageTexture;
};


class TitleState : public GameState
{
public:
  // Static accessor
  static TitleState* get( void );

  // Transitions
  bool enter( void ) override;
  bool exit ( void ) override;

  // Main loop functions
  void handleEvent( SDL_Event& ) override;
  void update     ( void )       override;
  void render     ( void )       override;

private:
  // Static instance
  static TitleState sTitleState;

  // Private constructor
  TitleState( void );

  // Intro background
  LTexture mBackgroundTexture;

  // Intro message
  LTexture mMessageTexture;
};


class OverWorldState : public GameState
{
public:
  // Static accessor
  static OverWorldState* get();

  // Transitions
  bool enter( void ) override;
  bool exit ( void ) override;

  // Main loop functions
  void handleEvent( SDL_Event& ) override;
  void update     ( void )         override;
  void render     ( void )         override;

private:
  // Level dimensions
  static constexpr int LEVEL_W = WINDOW_W * 2;
  static constexpr int LEVEL_H = WINDOW_H * 2;

  // Static instance
  static OverWorldState sOverWorldState;

  // Private constructor
  OverWorldState( void );

  // Overworld textures
  LTexture mBackgroundTexture;
  LTexture mRedHouseTexture;
  LTexture mBlueHouseTexture;

  // Game objects
  House mRedHouse;
  House mBlueHouse;
};


class RedRoomState : public GameState
{
public:
  // Static accessor
  static RedRoomState* get( void );

  // Transitions
  bool enter( void ) override;
  bool exit ( void ) override;

  // Main loop functions
  void handleEvent( SDL_Event& ) override;
  void update( void )            override;
  void render( void )            override;

private:
  // Level dimensions
  const static int LEVEL_W = WINDOW_W;
  const static int LEVEL_H = WINDOW_H;

  // Static instance
  static RedRoomState sRedRoomState;

  // Private constructor
  RedRoomState( void );

  // Room textures
  LTexture mBackgroundTexture;

  // Game objects
  Door mExitDoor;
};

class BlueRoomState : public GameState
{
public:
  // Static accessor
  static BlueRoomState* get();

  // Transitions
  bool enter( void ) override;
  bool exit ( void ) override;

  // Main loop functions
  void handleEvent( SDL_Event& ) override;
  void update     ( void )         override;
  void render     ( void )         override;

private:
  // Level dimensions
  static constexpr int LEVEL_W = WINDOW_W;
  static constexpr int LEVEL_H = WINDOW_H;

  // Static instance
  static BlueRoomState sBlueRoomState;

  // Private constructor
  BlueRoomState( void );

  // Room textures
  LTexture mBackgroundTexture;

  // Game objects
  Door mExitDoor;
};


class ExitState : public GameState
{
public:
  // Static accessor
  static ExitState* get( void );

  // Transitions
  bool enter( void ) override;
  bool exit ( void ) override;

  // Main loop functions
  void handleEvent( SDL_Event& ) override;
  void update     ( void )         override;
  void render     ( void )         override;

private:
  // Static instance
  static ExitState sExitState;

  // Private constructor
  ExitState( void );
};


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/


static bool init          ( void );
static bool loadMedia     ( void );
static void close         ( void );
static bool HasCollisionHappened( SDL_Rect, SDL_Rect );
static void setNextState  ( GameState* );
static void changeState   ( void );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

// The window we'll be rendering to
static SDL_Window* gWindow = NULL;

// The window renderer
static SDL_Renderer* gRenderer = NULL;

// Global assets
static LTexture  gDotTexture;
static TTF_Font* gFont = NULL;

// Global game objects
static Dot gDot;

// Game state object
static GameState* gCurrentState = NULL;
static GameState* gNextState = NULL;


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


bool LTexture::loadFromFile( std::string path )
{
  // Get rid of preexisting texture
  free();

  // The final texture
  SDL_Texture* newTexture = NULL;

  // Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
  if( loadedSurface == NULL )
  {
    SDL_Log( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
  }
  else
  {
    SDL_Log( "OK: image \"%s\" loaded\n", path.c_str() );

    // Convert surface to display format
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0 );

    if( formattedSurface == NULL )
    {
      SDL_Log( "Unable to convert loaded surface to display format! %s\n", SDL_GetError() );
    }
    else
    {
      // Create blank streamable texture
      newTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
      if( newTexture == NULL )
      {
        SDL_Log( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
      }
      else
      {
        SDL_Log( "OK: blank texture created\n" );

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
        int pixelCount = ( mPitch / 4 ) * mHeight;

        // Map colors
        Uint32 colorKey    = SDL_MapRGB( formattedSurface->format, CYAN_R, CYAN_G, CYAN_B );
        Uint32 transparent = SDL_MapRGBA( formattedSurface->format, CYAN_R, CYAN_G, CYAN_B, ALPHA_MIN );

        // Color key pixels
        for( int i = 0; i != pixelCount; ++i )
        {
          if( pixels[ i ] == colorKey )
          {
            pixels[ i ] = transparent;
          }
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
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
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
      SDL_Log( "Unable to create texture from rendered text! SDL Error: \"%s\"\n", SDL_GetError() );
    }
    else
    {
      SDL_Log( "OK: texture created from rendered text\n" );

      // Get image dimensions
      mWidth  = textSurface->w;
      mHeight = textSurface->h;
    }

    // Get rid of old surface
    SDL_FreeSurface( textSurface );
  }
  else
  {
    SDL_Log( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
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
    SDL_Log( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
  }
  else
  {
    SDL_Log( "OK: blank texture created\n" );

    mWidth  = width;
    mHeight = height;
  }

  return mTexture != NULL;
}

void LTexture::free()
{
  // Free texture if it exists
  if( mTexture != NULL )
  {
  SDL_DestroyTexture( mTexture );
  mTexture = NULL;
  mWidth = 0;
  mHeight = 0;
  mPixels = NULL;
  mPitch = 0;
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


void LTexture::setAsRenderTarget()
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
  SDL_Log( "Texture is already locked!\n" );
  success = false;
  }
  // Lock texture
  else
  {
  if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
  {
    SDL_Log( "Unable to lock texture! %s\n", SDL_GetError() );
    success = false;
  }
  }

  return success;
}

bool LTexture::unlockTexture()
{
  bool success = true;

  // Texture is not locked
  if( mPixels == NULL )
  {
  SDL_Log( "Texture is not locked!\n" );
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

void* LTexture::getPixels()
{
  return mPixels;
}

void LTexture::copyPixels( void* pixels )
{
  // Texture is locked
  if( mPixels != NULL )
  {
  // Copy to locked pixels
  memcpy( mPixels, pixels, mPitch * mHeight );
  }
}

int LTexture::getPitch()
{
  return mPitch;
}

Uint32 LTexture::getPixel32( unsigned int x, unsigned int y )
{
  // Convert the pixels to 32 bit
  Uint32 *pixels = (Uint32*)mPixels;

  // Get the pixel requested
  return pixels[ ( y * ( mPitch / 4 ) ) + x ];
}


Dot::Dot()
{
  // Initialize the offsets
  mBox.x = 0;
  mBox.y = 0;
  mBox.w = DOT_WIDTH;
  mBox.h = DOT_HEIGHT;

  // Initialize the velocity
  mVelX = 0;
  mVelY = 0;
}

void Dot::set( int x, int y )
{
  // Set position
  mBox.x = x;
  mBox.y = y;
}

void Dot::handleEvent( SDL_Event& e )
{
  // If a key was pressed
  if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
  {
  // Adjust the velocity
  switch( e.key.keysym.sym )
  {
    case SDLK_UP: mVelY -= DOT_MAX_VEL; break;
    case SDLK_DOWN: mVelY += DOT_MAX_VEL; break;
    case SDLK_LEFT: mVelX -= DOT_MAX_VEL; break;
    case SDLK_RIGHT: mVelX += DOT_MAX_VEL; break;
  }
  }
  // If a key was released
  else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
  {
  // Adjust the velocity
  switch( e.key.keysym.sym )
  {
    case SDLK_UP: mVelY += DOT_MAX_VEL; break;
    case SDLK_DOWN: mVelY -= DOT_MAX_VEL; break;
    case SDLK_LEFT: mVelX += DOT_MAX_VEL; break;
    case SDLK_RIGHT: mVelX -= DOT_MAX_VEL; break;
  }
  }
}

void Dot::move( int levelWidth, int levelHeight )
{
  // Move the dot left or right
  mBox.x += mVelX;

  // If the dot went too far to the left or right
  if( ( mBox.x < 0 ) || ( mBox.x + DOT_WIDTH > levelWidth ) )
  {
  // Move back
  mBox.x -= mVelX;
  }

  // Move the dot up or down
  mBox.y += mVelY;

  // If the dot went too far up or down
  if( ( mBox.y < 0 ) || ( mBox.y + DOT_HEIGHT > levelHeight ) )
  {
  // Move back
  mBox.y -= mVelY;
  }
}

void Dot::render( SDL_Rect camera )
{
  // Show the dot relative to the camera
  gDotTexture.render( mBox.x - camera.x, mBox.y - camera.y );
}

SDL_Rect Dot::getCollider()
{
  // Get collision box
  return mBox;
}

House::House()
{
  // Initialize position
  mBox.x = 0;
  mBox.y = 0;
  mBox.w = HOUSE_WIDTH;
  mBox.h = HOUSE_HEIGHT;

  // Initialize texture
  mHouseTexture = NULL;
}

void House::set( int x, int y, LTexture* houseTexture )
{
  // Initialize position
  mBox.x = x;
  mBox.y = y;

  // Initialize texture
  mHouseTexture = houseTexture;
}

void House::render( SDL_Rect camera )
{
  // Show the house relative to the camera
  mHouseTexture->render( mBox.x - camera.x, mBox.y - camera.y );
}

SDL_Rect House::getCollider()
{
  // Get collision box
  return mBox;
}

Door::Door()
{
  // Initialize position
  mBox.x = 0;
  mBox.y = 0;
  mBox.w = DOOR_WIDTH;
  mBox.h = DOOR_HEIGHT;
}

void Door::set( int x, int y )
{
  // Initialize position
  mBox.x = x;
  mBox.y = y;
}

void Door::render()
{
  // Draw rectangle for door
  SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
  SDL_RenderFillRect( gRenderer, &mBox );
}

SDL_Rect Door::getCollider()
{
  // Get collision box
  return mBox;
}


IntroState* IntroState::get()
{
  // Get static instance
  return &sIntroState;
}

bool IntroState::enter()
{
  // Loading success flag
  bool success = true;

  // Load background
  if( !mBackgroundTexture.loadFromFile( BGPath ) )
  {
  printf( "Failed to intro background!\n" );
  success = false;
  }

  // Load text
  SDL_Color textColor = { 0x00, 0x00, 0x00, 0xFF };
  if( !mMessageTexture.loadFromRenderedText( "Lazy Foo' Productions Presents...", textColor ) )
  {
  printf( "Failed to render intro text!\n" );
  success = false;
  }

  return success;
}

bool IntroState::exit()
{
  // Free background and text
  mBackgroundTexture.free();
  mMessageTexture.free();

  return true;
}

void IntroState::handleEvent( SDL_Event& e )
{
  // If the user pressed enter
  if( ( e.type == SDL_KEYDOWN ) && ( e.key.keysym.sym == SDLK_RETURN ) )
  {
  // Move onto title state
  setNextState( TitleState::get() );
  }
}

void IntroState::update()
{

}

void IntroState::render()
{
  // Show the background
  mBackgroundTexture.render( 0, 0 );

  // Show the message
  mMessageTexture.render( ( WINDOW_W - mMessageTexture.getWidth() ) / 2, ( WINDOW_H - mMessageTexture.getHeight() ) / 2 );
}

// Declare static instance
IntroState IntroState::sIntroState;

IntroState::IntroState()
{
  // No public instantiation
}


TitleState* TitleState::get()
{
  // Get static instance
  return &sTitleState;
}

bool TitleState::enter()
{
  // Loading success flag
  bool success = true;

  // Load background
  if( !mBackgroundTexture.loadFromFile( TitlePath ) )
  {
  printf( "Failed to title background!\n" );
  success = false;
  }

  // Load text
  SDL_Color textColor = { 0x00, 0x00, 0x00, 0xFF };
  if( !mMessageTexture.loadFromRenderedText( "A State Machine Demo", textColor ) )
  {
  printf( "Failed to render title text!\n" );
  success = false;
  }

  return success;
}

bool TitleState::exit()
{
  // Free background and text
  mBackgroundTexture.free();
  mMessageTexture.free();

  return true;
}

void TitleState::handleEvent( SDL_Event& e )
{
  // If the user pressed enter
  if( ( e.type == SDL_KEYDOWN ) && ( e.key.keysym.sym == SDLK_RETURN ) )
  {
  // Move to overworld
  setNextState( OverWorldState::get() );
  }
}

void TitleState::update()
{

}

void TitleState::render()
{
  // Show the background
  mBackgroundTexture.render( 0, 0 );

  // Show the message
  mMessageTexture.render( ( WINDOW_W - mMessageTexture.getWidth() ) / 2, ( WINDOW_H - mMessageTexture.getHeight() ) / 2 );
}

// Declare static instance
TitleState TitleState::sTitleState;

TitleState::TitleState()
{
  // No public instantiation
}


OverWorldState* OverWorldState::get()
{
  // Get static instance
  return &sOverWorldState;
}

bool OverWorldState::enter()
{
  // Loading success flag
  bool success = true;

  // Load background
  if( !mBackgroundTexture.loadFromFile( GreenOWPath ) )
  {
  printf( "Failed to load overworld background!\n" );
  success = false;
  }

  // Load house texture
  if( !mBlueHouseTexture.loadFromFile( BluePath ) )
  {
  printf( "Failed to load blue house texture!\n" );
  success = false;
  }

  // Load house texture
  if( !mRedHouseTexture.loadFromFile( RedPath ) )
  {
  printf( "Failed to load red house texture!\n" );
  success = false;
  }

  // Position houses with graphics
  mRedHouse.set( 0, 0, &mRedHouseTexture );
  mBlueHouse.set( LEVEL_W - House::HOUSE_WIDTH, LEVEL_H - House::HOUSE_HEIGHT, &mBlueHouseTexture );

  // Came from red room state
  if( gCurrentState == RedRoomState::get() )
  {
  // Position below red house
  gDot.set( mRedHouse.getCollider().x + ( House::HOUSE_WIDTH - Dot::DOT_WIDTH ) / 2, mRedHouse.getCollider().y + mRedHouse.getCollider().h + Dot::DOT_HEIGHT );
  }
  // Came from blue room state
  else if( gCurrentState == BlueRoomState::get() )
  {
  // Position above blue house
  gDot.set( mBlueHouse.getCollider().x + ( House::HOUSE_WIDTH - Dot::DOT_WIDTH ) / 2, mBlueHouse.getCollider().y - Dot::DOT_HEIGHT * 2 );
  }
  // Came from other state
  else
  {
  // Position middle of overworld
  gDot.set( ( LEVEL_W - Dot::DOT_WIDTH ) / 2, ( LEVEL_H - Dot::DOT_HEIGHT ) / 2 );
  }

  return success;
}

bool OverWorldState::exit()
{
  // Free textures
  mBackgroundTexture.free();
  mRedHouseTexture.free();
  mBlueHouseTexture.free();

  return true;
}

void OverWorldState::handleEvent( SDL_Event& e )
{
  // Handle dot input
  gDot.handleEvent( e );
}

void OverWorldState::update()
{
  // Move dot
  gDot.move( LEVEL_W, LEVEL_H );

  // On red room collision
  if( HasCollisionHappened( gDot.getCollider(), mRedHouse.getCollider() ) )
  {
  // Got to red room
  setNextState( RedRoomState::get() );
  }
  // On blue room collision
  else if( HasCollisionHappened( gDot.getCollider(), mBlueHouse.getCollider() ) )
  {
  // Go to blue room
  setNextState( BlueRoomState::get() );
  }
}

void OverWorldState::render()
{
  // Center the camera over the dot
  SDL_Rect camera =
  {
  ( gDot.getCollider().x + Dot::DOT_WIDTH / 2 ) - WINDOW_W / 2,
  ( gDot.getCollider().y + Dot::DOT_HEIGHT / 2 ) - WINDOW_H / 2,
  WINDOW_W,
  WINDOW_H
  };

  // Keep the camera in bounds
  if( camera.x < 0 )
  {
  camera.x = 0;
  }
  if( camera.y < 0 )
  {
  camera.y = 0;
  }
  if( camera.x > LEVEL_W - camera.w )
  {
  camera.x = LEVEL_W - camera.w;
  }
  if( camera.y > LEVEL_H - camera.h )
  {
  camera.y = LEVEL_H - camera.h;
  }

  // Render background
  mBackgroundTexture.render( 0, 0, &camera );

  // Render objects
  mRedHouse.render( camera );
  mBlueHouse.render( camera );
  gDot.render( camera );
}

// Declare static instance
OverWorldState OverWorldState::sOverWorldState;

OverWorldState::OverWorldState()
{
  // No public instantiation
}


RedRoomState* RedRoomState::get()
{
  // Get static instance
  return &sRedRoomState;
}

bool RedRoomState::enter()
{
  // Loading success flag
  bool success = true;

  // Load background
  if( !mBackgroundTexture.loadFromFile( RedRoomPath ) )
  {
  printf( "Failed to load blue room background!\n" );
  success = false;
  }

  // Place game objects
  mExitDoor.set( ( LEVEL_W - Door::DOOR_WIDTH ) / 2, LEVEL_H - Door::DOOR_HEIGHT );
  gDot.set( ( LEVEL_W - Dot::DOT_WIDTH ) / 2, LEVEL_H - Door::DOOR_HEIGHT - Dot::DOT_HEIGHT * 2 );

  return success;
}

bool RedRoomState::exit()
{
  // Free background
  mBackgroundTexture.free();

  return true;
}

void RedRoomState::handleEvent( SDL_Event& e )
{
  // Handle dot input
  gDot.handleEvent( e );
}

void RedRoomState::update()
{
  // Move dot
  gDot.move( LEVEL_W, LEVEL_H );

  // On exit collision
  if( HasCollisionHappened( gDot.getCollider(), mExitDoor.getCollider() ) )
  {
  // Go back to overworld
  setNextState( OverWorldState::get() );
  }
}

void RedRoomState::render()
{
  // Center the camera over the dot
  SDL_Rect camera = { 0, 0, LEVEL_W, LEVEL_H };

  // Render background
  mBackgroundTexture.render( 0, 0, &camera );

  // Render objects
  mExitDoor.render();
  gDot.render( camera );
}

// Declare static instance
RedRoomState RedRoomState::sRedRoomState;

RedRoomState::RedRoomState()
{
  // No public instantiation
}


BlueRoomState* BlueRoomState::get()
{
  // Get static instance
  return &sBlueRoomState;
}

bool BlueRoomState::enter()
{
  // Loading success flag
  bool success = true;

  // Load background
  if( !mBackgroundTexture.loadFromFile( BlueRoomPath ) )
  {
  printf( "Failed to load blue room background!\n" );
  success = false;
  }

  // Position game objects
  mExitDoor.set( ( LEVEL_W - Door::DOOR_WIDTH ) / 2, 0 );
  gDot.set( ( LEVEL_W - Dot::DOT_WIDTH ) / 2, Door::DOOR_HEIGHT + Dot::DOT_HEIGHT * 2 );

  return success;
}

bool BlueRoomState::exit()
{
  // Free background
  mBackgroundTexture.free();

  return true;
}

void BlueRoomState::handleEvent( SDL_Event& e )
{
  // Handle dot input
  gDot.handleEvent( e );
}

void BlueRoomState::update()
{
  // Move dot
  gDot.move( LEVEL_W, LEVEL_H );

  // On exit collision
  if( HasCollisionHappened( gDot.getCollider(), mExitDoor.getCollider() ) )
  {
  // Back to overworld
  setNextState( OverWorldState::get() );
  }
}

void BlueRoomState::render()
{
  // Center the camera over the dot
  SDL_Rect camera = { 0, 0, LEVEL_W, LEVEL_H };

  // Render background
  mBackgroundTexture.render( 0, 0, &camera );

  // Render objects
  mExitDoor.render();
  gDot.render( camera );
}

// Declare static instance
BlueRoomState BlueRoomState::sBlueRoomState;

BlueRoomState::BlueRoomState(void)
{
  // No public instantiation
}

// Hollow exit state
ExitState* ExitState::get(void)
{
  return &sExitState;
}

bool ExitState::enter(void)
{
  return true;
}

bool ExitState::exit(void)
{
  return true;
}

void ExitState::handleEvent( [[maybe_unused]] SDL_Event& e )
{

}

void ExitState::update(void)
{

}

void ExitState::render(void)
{

}

ExitState ExitState::sExitState;

ExitState::ExitState(void)
{

}


/**
 * @brief Starts up SDL and creates window
 *
 * @return true
 * @return false
 **/
static bool init(void)
{
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
  printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
  success = false;
  }
  else
  {
    // Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
      printf( "Warning: Linear texture filtering not enabled!" );
    }

    // Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN );
    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
      success = false;
    }
    else
    {
      // Create vsynced renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
      if( gRenderer == NULL )
      {
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        success = false;
      }
      else
      {
        // Initialize renderer color
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, ALPHA_MAX );

        // Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;

        if( !( IMG_Init( imgFlags ) & imgFlags ) )
        {
          printf( "SDL_image could not initialize! SDL_image Error: \"%s\"\n", IMG_GetError() );
          success = false;
        }
        else
        {
          printf( "OK: SDL_image initialised\n" );
        }

        // Initialize SDL_ttf
        if( TTF_Init() == -1 )
        {
          printf( "SDL_ttf could not initialize! SDL_ttf Error: \"%s\"\n", TTF_GetError() );
          success = false;
        }
        else
        {
          printf( "OK: SDL_ttf initialised\n" );
        }
      }
    }
  }

  return success;
}


/**
 * @brief Loads cross state assets
 *
 * @return true
 * @return false
 **/
static bool loadMedia(void)
{
  // Loading success flag
  bool success = true;

  // Load dot texture
  if( !gDotTexture.loadFromFile( DotPath ) )
  {
  printf( "Failed to load dot texture! SDL Error: %s\n", SDL_GetError() );
  success = false;
  }

  // Open the font
  gFont = TTF_OpenFont( LazyFontPath.c_str(), 28 );
  if( gFont == NULL )
  {
  printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
  success = false;
  }

  return success;
}


/**
 * @brief Frees media and shuts down SDL
 **/
static void close(void)
{
  // Free the surfaces
  gDotTexture.free();

  // Close the font that was used
  TTF_CloseFont( gFont );
  gFont = NULL;

  // Destroy windows
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
  gRenderer = NULL;

  // Quit SDL subsystems
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}


/**
 * @brief State managers
 *
 * @param newState
 **/
static void setNextState( GameState* newState )
{
  // If the user doesn't want to exit
  if( gNextState != ExitState::get() )
  {
    // Set the next state
    gNextState = newState;
  }
}


/**
 * @brief Change state
 **/
static void changeState(void)
{
  // If the state needs to be changed

  if( gNextState != NULL )
  {
    gCurrentState->exit();
    gNextState->enter();

    // Change the current state ID

    gCurrentState = gNextState;
    gNextState    = NULL;
  }
  else { /* No need to change state */ }
}


/**
 * @brief Check collisions
 *
 * @param a
 * @param b
 * @return true
 * @return false
 **/
static bool HasCollisionHappened( SDL_Rect a, SDL_Rect b )
{
  bool Result = true;

  // The sides of the rectangles
  int leftA  , leftB;
  int rightA , rightB;
  int topA   , topB;
  int bottomA, bottomB;

  // Calculate the sides of rect A
  leftA   = a.x;
  rightA  = a.x + a.w;
  topA    = a.y;
  bottomA = a.y + a.h;

  // Calculate the sides of rect B
  leftB   = b.x;
  rightB  = b.x + b.w;
  topB    = b.y;
  bottomB = b.y + b.h;

  // If any of the sides from A are outside of B
  if( bottomA <= topB )
  {
    Result = false;
  }
  else {;}

  if( topA >= bottomB )
  {
    Result = false;
  }
  else {;}

  if( rightA <= leftB )
  {
    Result = false;
  }
  else {;}

  if( leftA >= rightB )
  {
    Result = false;
  }
  else {;}

  // If none of the sides from A are outside B
  return Result;
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
  printf("\nProgram started with %d additional arguments.", argc - 1); //  Il primo argomento è il nome dell'eseguibile

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
      printf( "Failed to load media!\n" );
    }
    else
    {
      printf( "\nOK: all media loaded" );

      // Event handler
      SDL_Event e;

      // Set the current game state object
      gCurrentState = IntroState::get();
      gCurrentState->enter();

      // While the user hasn't quit
      while( gCurrentState != ExitState::get() )
      {
        // Do state event handling
        while( SDL_PollEvent( &e ) != 0 )
        {
          // Handle state events
          gCurrentState->handleEvent( e );

          // Exit on quit
          if( e.type == SDL_QUIT )
          {
            setNextState( ExitState::get() );
          }
          else { /*  */ }
        }

        // Do state logic
        gCurrentState->update();

        // Change state if needed
        changeState();

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, ALPHA_MAX );
        SDL_RenderClear( gRenderer );

        // Do state rendering
        gCurrentState->render();

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
