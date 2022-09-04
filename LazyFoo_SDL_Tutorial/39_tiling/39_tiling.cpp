/**
 * @file 39_tiling.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/39_tiling/index.php
 *
 * @brief Tiling is a way of making levels out of uniformly sized reusable pieces. In this tutorial
 * we'll be making a 1280x960 sized level of out only a 160x120 sized tile set.
 *
 * - Normally, it's a good idea to have position and collider separate when doing collision
 *   detection, but for the sake of simplicity we're using the collider to hold position.
 * - Here is the dot class yet again, now with the ability to check for collision against the tiles
 *   when moving.
 * - Our media loading function will also be initializing tiles, so it needs to take them in as an
 *   argument.
 * - We also have the "touchesWall" function that checks a collision box against every wall in a
 *   tile set which will be used when we need to check the dot against the whole tile set. Finally,
 *   the "setTiles" function loads and sets the tiles.
 * - When we render, we only want to show tiles that are in the camera's sight, so we check if the
 *   tile collides with the camera before rendering it. Notice also that we render the tile relative
 *   to the camera.
 * - When we move the dot, we check if it goes off the level or hits a wall tile. If it does, we
 *   correct it.
 * - The "Dot::setCamera" and "Dot::render" is the rendering code, largely lifted from the
 *   scrolling/camera tutorial.
 * - In our loading function we not only load the textures but also the tile set.
 * - Near the top of the "setTiles" function, we declare x/y offsets that define where we'll be
 *   placing the tiles. As we load in more tiles, we'll be shifting the x/y position left to right
 *   and top to bottom.
 * - We then open the lazy.map file. Using fstream we can read text from a file much like we would
 *   read keyboard input with iostream. Before we can continue, we have to check if the map loaded
 *   correctly. If it failed we abort, and if not we continue loading the file.
 * - If the file loaded successfully, we have a for loop that reads in all the numbers from the text
 *   file. We read a number into the tileType variable and then check if the read failed. If the
 *   read failed, we abort. If not, we then check if the tile type number is valid. If it is valid,
 *   we create a new tile of the given type, if not we print an error and stop loading tiles.
 * - After all the tiles are loaded we set the clip rectangles for the tile sprites. Finally we
 *   close the map file and return.
 * - The "touchesWall" function checks a given collision box against tiles of type TILE_CENTER,
 *   TILE_TOP, TILE_TOPRIGHT, TILE_RIGHT, TILE_BOTTOMRIGHT, TILE_BOTTOM, TILE_BOTTOMLEFT, TILE_LEFT,
 *   and TILE_TOPLEFT which are all wall tiles. If you check back when we defined these constants,
 *   you'll see that these are numbered right next to each other so all we have to do is check if
 *   the type is between TILE_CENTER and TILE_TOPLEFT. If the given collision box collides with any
 *   tile that is a wall, this function returns true.
 * - In the main function right before we load the media we declare our array of tile pointers.
 * - Our main loop is pretty much the same, with some adjustments. When we move the dot, we pass in
 *   the tile set and then set the camera over the dot after it moved. We then render the tile set
 *   and finally render the dot over the level.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022) and may not be
 * redistributed without written permission.
 **/


/**************************************************************************************************
* Includes
***************************************************************************************************/

// Using SDL, SDL_image, standard I/O, strings, and file streams
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <fstream>


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int FIRST_ONE = -1;

static constexpr int WINDOW_W = 800; // Screen's width
static constexpr int WINDOW_H = 600; // Screen's heigth

// The dimensions of the level
static constexpr int LEVEL_W = 1280;
static constexpr int LEVEL_H = 960;

// Colore bianco
static constexpr int WHITE_R = 0xFF; // Amount of red   needed to compose white
static constexpr int WHITE_G = 0xFF; // Amount of green needed to compose white
static constexpr int WHITE_B = 0xFF; // Amount of blue  needed to compose white
static constexpr int WHITE_A = 0xFF; // Alpha component

// Colore ciano
static constexpr int CYAN_R = 0x00; // Amount of red   needed to compose cyan
static constexpr int CYAN_G = 0xFF; // Amount of green needed to compose cyan
static constexpr int CYAN_B = 0xFF; // Amount of blue  needed to compose cyan
static constexpr int CYAN_A = 0xFF; // Alpha component

// Tile constants
static constexpr int TILE_W = 80;
static constexpr int TILE_H = 80;
static constexpr int TOTAL_TILES = 192;
static constexpr int TOTAL_TILE_SPRITES = 12;

// The different tile sprites
enum TileTypes
{
  TILE_RED = 0,
  TILE_GREEN,
  TILE_BLUE,
  TILE_CENTER,
  TILE_TOP,
  TILE_TOPRIGHT,
  TILE_RIGHT,
  TILE_BOTTOMRIGHT,
  TILE_BOTTOM,
  TILE_BOTTOMLEFT,
  TILE_LEFT,
  TILE_TOPLEFT
};

/* Paths */

static const std::string g_DotPath  ( "dot.bmp" );
static const std::string g_TilesPath( "tiles.png" );
static const std::string g_LazyMap  ( "lazy.map" );


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


/**
 * @brief The tile
 **/
class Tile
{
    public:

    Tile( int, int, int );

    // Shows the tile
    void render( SDL_Rect& );

    // Get the tile type
    int getType(void) const;

    // Get the collision box
    SDL_Rect getBox(void) const;

    private:

    // The attributes of the tile
    SDL_Rect mBox;

    // The tile type
    int mType;
};


// The dot that will move around on the screen
class Dot
{
  public:

  // The dimensions of the dot
  static constexpr int DOT_WIDTH  = 20;
  static constexpr int DOT_HEIGHT = 20;

  // Maximum axis velocity of the dot
  static constexpr int DOT_MAX_VEL = 10;

  // Initializes the variables
  Dot(void);

  // Takes key presses and adjusts the dot's velocity
  void handleEvent( SDL_Event& );

  void move( Tile* [] );

  void setCamera( SDL_Rect& );

  void render( SDL_Rect& );

  private:

  // Collision box of the dot
  SDL_Rect mBox;

  // The velocity of the dot
  int mVelX, mVelY;
};


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init          ( void );
static bool loadMedia     ( Tile* [] );
static void close         ( Tile* [] );
static bool checkCollision( SDL_Rect, SDL_Rect );
static bool touchesWall   ( SDL_Rect, Tile* [] );
static bool setTiles      ( Tile *[] );


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer

// Scene textures
static LTexture gDotTexture;
static LTexture gTileTexture;
static SDL_Rect gTileClips[ TOTAL_TILE_SPRITES ];


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
    printf( "\nOK: image \"%s\" loaded", path.c_str() );

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
      printf( "\nOK: texture created from \"%s\"", path.c_str() );

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
 * @brief Initializes position and type
 *
 * @param x
 * @param y
 * @param tileType
 **/
Tile::Tile( int x, int y, int tileType )
{
  // Get the offsets
  mBox.x = x;
  mBox.y = y;

  // Set the collision box
  mBox.w = TILE_W;
  mBox.h = TILE_H;

  // Get the tile type
  mType = tileType;
}


void Tile::render( SDL_Rect& camera )
{
  // If the tile is on screen
  if( checkCollision( camera, mBox ) )
  {
      // Show the tile
      gTileTexture.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips[ mType ] );
  }
  else { /* Tile not on screen */ }
}


int Tile::getType(void) const
{
  return mType;
}


SDL_Rect Tile::getBox(void) const
{
  return mBox;
}


Dot::Dot(void)
{
  // Initialize the collision box
  mBox.x = 0;
  mBox.y = 0;
  mBox.w = DOT_WIDTH;
  mBox.h = DOT_HEIGHT;

  // Initialize the velocity
  mVelX = 0;
  mVelY = 0;
}


void Dot::handleEvent( SDL_Event& e )
{
  // If a key was pressed
  if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
  {
    // Adjust the velocity
    switch( e.key.keysym.sym )
    {
      case SDLK_UP:     mVelY -= DOT_MAX_VEL; break;
      case SDLK_DOWN:   mVelY += DOT_MAX_VEL; break;
      case SDLK_LEFT:   mVelX -= DOT_MAX_VEL; break;
      case SDLK_RIGHT:  mVelX += DOT_MAX_VEL; break;
    }
  }
  // If a key was released
  else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
  {
    // Adjust the velocity
    switch( e.key.keysym.sym )
    {
      case SDLK_UP:     mVelY += DOT_MAX_VEL; break;
      case SDLK_DOWN:   mVelY -= DOT_MAX_VEL; break;
      case SDLK_LEFT:   mVelX += DOT_MAX_VEL; break;
      case SDLK_RIGHT:  mVelX -= DOT_MAX_VEL; break;
    }
  }
}


/**
 * @brief Moves the dot and check collision against tiles
 *
 * @param tiles
 **/
void Dot::move( Tile *tiles[] )
{
  // Move the dot left or right
  mBox.x += mVelX;

  // If the dot went too far to the left or right or touched a wall
  if( ( mBox.x < 0 ) || ( mBox.x + DOT_WIDTH > LEVEL_W ) || touchesWall( mBox, tiles ) )
  {
    // move back
    mBox.x -= mVelX;
  }
  else { /* Movement OK */ }

  // Move the dot up or down
  mBox.y += mVelY;

  // If the dot went too far up or down or touched a wall
  if( ( mBox.y < 0 ) || ( mBox.y + DOT_HEIGHT > LEVEL_H ) || touchesWall( mBox, tiles ) )
  {
    // move back
    mBox.y -= mVelY;
  }
  else { /* Movement OK */ }
}


/**
 * @brief Centers the camera over the dot
 *
 * @param camera
 **/
void Dot::setCamera( SDL_Rect& camera )
{
  // Center the camera over the dot
  camera.x = ( mBox.x + DOT_WIDTH  / 2 ) - WINDOW_W / 2;
  camera.y = ( mBox.y + DOT_HEIGHT / 2 ) - WINDOW_H / 2;

  // Keep the camera in bounds
  if( camera.x < 0 )
  {
    camera.x = 0;
  }
  else {;}

  if( camera.y < 0 )
  {
    camera.y = 0;
  }
  else {;}

  if( camera.x > LEVEL_W - camera.w )
  {
    camera.x = LEVEL_W - camera.w;
  }
  else {;}

  if( camera.y > LEVEL_H - camera.h )
  {
    camera.y = LEVEL_H - camera.h;
  }
  else {;}
}


/**
 * @brief Shows the dot on the screen
 *
 * @param camera
 **/
void Dot::render( SDL_Rect& camera )
{
    // Show the dot
  gDotTexture.render( mBox.x - camera.x, mBox.y - camera.y );
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
          printf( "\nSDL_image could not initialize! SDL_image Error: %s", IMG_GetError() );
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
static bool loadMedia( Tile* tiles[] )
{
  // Loading success flag
  bool success = true;

  // Load dot texture
  if( !gDotTexture.loadFromFile( g_DotPath ) )
  {
    printf( "\nFailed to load dot texture!" );
    success = false;
  }
  else
  { /* Texture loaded correctly */ }

  // Load tile texture
  if( !gTileTexture.loadFromFile( g_TilesPath ) )
  {
    printf( "\nFailed to load tile set texture!" );
    success = false;
  }
  else
  { /* Texture loaded correctly */ }

  // Load tile map
  if( !setTiles( tiles ) )
  {
    printf( "\nFailed to load tile set!" );
    success = false;
  }
  else
  { /* Tiles loaded correctly */ }

  return success;
}


static void close( Tile* tiles[] )
{
  // Deallocate tiles
  for( int i = 0; i < TOTAL_TILES; ++i )
  {
     if( tiles[ i ] != NULL )
     {
      delete tiles[ i ];
      tiles[ i ] = NULL;
     }
  }

  // Free loaded images
  gDotTexture.free();
  gTileTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
  gRenderer = NULL;

  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}


/**
 * @brief Box collision detector
 *
 * @param a
 * @param b
 * @return true
 * @return false
 **/
bool checkCollision( SDL_Rect a, SDL_Rect b )
{
  // The sides of the rectangles
  int leftA, leftB;
  int rightA, rightB;
  int topA, topB;
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
    return false;
  }
  else {;}

  if( topA >= bottomB )
  {
    return false;
  }
  else {;}

  if( rightA <= leftB )
  {
    return false;
  }
  else {;}

  if( leftA >= rightB )
  {
    return false;
  }
  else {;}

  // If none of the sides from A are outside B
  return true;
}


/**
 * @brief Sets tiles from tile map
 *
 * @param tiles
 * @return true
 * @return false
 **/
bool setTiles( Tile* tiles[] )
{
  // Success flag
  bool tilesLoaded = true;

  // The tile offsets
  int x = 0, y = 0;

  // Open the map
  std::ifstream MapFile( g_LazyMap );

  // If the map couldn't be loaded
  if( MapFile.fail() )
  {
    printf( "\nUnable to load map file!" );
    tilesLoaded = false;
  }
  else
  {
    // Initialize the tiles
    for( int i = 0; i != TOTAL_TILES; ++i )
    {
      // Determines what kind of tile will be made
      int tileType = -1;

      // Read tile from map file
      MapFile >> tileType;

      // If the was a problem in reading the map
      if( MapFile.fail() )
      {
        // Stop loading map
        printf( "\nError loading map: Unexpected end of file!\n" );
        tilesLoaded = false;
        break;
      }
      else
      { /* Reading was OK */ }

      // If the number is a valid tile number
      if( ( tileType >= 0 ) && ( tileType < TOTAL_TILE_SPRITES ) )
      {
        tiles[ i ] = new Tile( x, y, tileType );
      }
      // If we don't recognize the tile type
      else
      {
        // Stop loading map
        printf( "\nError loading map: Invalid tile type at %d!\n", i );
        tilesLoaded = false;
        break;
      }

      // Move to next tile spot
      x += TILE_W;

      // If we've gone too far
      if( x >= LEVEL_W )
      {
        // Move back
        x = 0;

        // Move to the next row
        y += TILE_H;
      }
      else
      { /* Position was OK */ }
    }

    // Clip the sprite sheet
    if( tilesLoaded )
    {
      gTileClips[ TILE_RED ].x = 0;
      gTileClips[ TILE_RED ].y = 0;
      gTileClips[ TILE_RED ].w = TILE_W;
      gTileClips[ TILE_RED ].h = TILE_H;

      gTileClips[ TILE_GREEN ].x = 0;
      gTileClips[ TILE_GREEN ].y = 80;
      gTileClips[ TILE_GREEN ].w = TILE_W;
      gTileClips[ TILE_GREEN ].h = TILE_H;

      gTileClips[ TILE_BLUE ].x = 0;
      gTileClips[ TILE_BLUE ].y = 160;
      gTileClips[ TILE_BLUE ].w = TILE_W;
      gTileClips[ TILE_BLUE ].h = TILE_H;

      gTileClips[ TILE_TOPLEFT ].x = 80;
      gTileClips[ TILE_TOPLEFT ].y = 0;
      gTileClips[ TILE_TOPLEFT ].w = TILE_W;
      gTileClips[ TILE_TOPLEFT ].h = TILE_H;

      gTileClips[ TILE_LEFT ].x = 80;
      gTileClips[ TILE_LEFT ].y = 80;
      gTileClips[ TILE_LEFT ].w = TILE_W;
      gTileClips[ TILE_LEFT ].h = TILE_H;

      gTileClips[ TILE_BOTTOMLEFT ].x = 80;
      gTileClips[ TILE_BOTTOMLEFT ].y = 160;
      gTileClips[ TILE_BOTTOMLEFT ].w = TILE_W;
      gTileClips[ TILE_BOTTOMLEFT ].h = TILE_H;

      gTileClips[ TILE_TOP ].x = 160;
      gTileClips[ TILE_TOP ].y = 0;
      gTileClips[ TILE_TOP ].w = TILE_W;
      gTileClips[ TILE_TOP ].h = TILE_H;

      gTileClips[ TILE_CENTER ].x = 160;
      gTileClips[ TILE_CENTER ].y = 80;
      gTileClips[ TILE_CENTER ].w = TILE_W;
      gTileClips[ TILE_CENTER ].h = TILE_H;

      gTileClips[ TILE_BOTTOM ].x = 160;
      gTileClips[ TILE_BOTTOM ].y = 160;
      gTileClips[ TILE_BOTTOM ].w = TILE_W;
      gTileClips[ TILE_BOTTOM ].h = TILE_H;

      gTileClips[ TILE_TOPRIGHT ].x = 240;
      gTileClips[ TILE_TOPRIGHT ].y = 0;
      gTileClips[ TILE_TOPRIGHT ].w = TILE_W;
      gTileClips[ TILE_TOPRIGHT ].h = TILE_H;

      gTileClips[ TILE_RIGHT ].x = 240;
      gTileClips[ TILE_RIGHT ].y = 80;
      gTileClips[ TILE_RIGHT ].w = TILE_W;
      gTileClips[ TILE_RIGHT ].h = TILE_H;

      gTileClips[ TILE_BOTTOMRIGHT ].x = 240;
      gTileClips[ TILE_BOTTOMRIGHT ].y = 160;
      gTileClips[ TILE_BOTTOMRIGHT ].w = TILE_W;
      gTileClips[ TILE_BOTTOMRIGHT ].h = TILE_H;
    }
    else{ /* Error loading tiles */ }
  }

  // Close the file
  MapFile.close();

  // If the map was loaded fine
  return tilesLoaded;
}


/**
 * @brief Checks collision box against set of tiles
 *
 * @param box
 * @param tiles
 * @return true
 * @return false
 **/
bool touchesWall( SDL_Rect box, Tile* tiles[] )
{
  // Go through the tiles
  for( int i = 0; i != TOTAL_TILES; ++i )
  {
    // If the tile is a wall type tile
    if( ( tiles[ i ]->getType() >= TILE_CENTER ) && ( tiles[ i ]->getType() <= TILE_TOPLEFT ) )
    {
      // If the collision box touches the wall tile
      if( checkCollision( box, tiles[ i ]->getBox() ) )
      {
        return true;
      }
      else { /* No collision */ }
    }
    else { /* Tile is not a wall type */ }
  }

  // If no wall tiles were touched
  return false;
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
    printf( "\nOK: all systems initialised" );

    // The level tiles
    Tile* tileSet[ TOTAL_TILES ];

    // Load media
    if( !loadMedia( tileSet ) )
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

      // The dot that will be moving around on the screen
      Dot dot;

      // Level camera
      SDL_Rect camera = { 0, 0, WINDOW_W, WINDOW_H };

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

          // Handle input for the dot
          dot.handleEvent( e );
        }

        // Move the dot
        dot.move( tileSet );
        dot.setCamera( camera );

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render level
        for( int i = 0; i != TOTAL_TILES; ++i )
        {
          tileSet[ i ]->render( camera );
        }

        // Render dot
        dot.render( camera );

        // Update screen
        SDL_RenderPresent( gRenderer );
      }
    }

    // Free resources and close SDL
    close( tileSet );
  }

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