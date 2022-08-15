/**
 * @file 28_per-pixel_collision_detection.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/28_per-pixel_collision_detection/index.php
 *
 * @brief Once you know how to check collision between two rectangles, you can check collision
 * between any two images since all images are made out of rectangles. Images are made out of
 * pixels, which are squares, which are rectangles. To do per-pixel collision detection, all we have
 * to do is have each object have a set of box colliders and check collision of one set of collision
 * boxes against another like so:
 *
 * Our dot now has per-pixel collision detection. Its velocity is reduced to 1 pixel per frame to
 * make the collision easier to see. The move function now takes in a vector of collision boxes so
 * we can check two sets against each other. Since we're going to have two dots colliding, we need
 * to be able to get the colliders, so we have a function for that.
 *
 * Instead of having a single collision box, we have a vector of colliders. We also have an internal
 * function to shift the colliders to match the position of the dot. The new collision detector
 * checks sets of collision boxes against each other.
 *
 * Just as before, we have to set the collider dimensions in the constructor. Only difference here
 * is that we have multiple collision boxes to set.
 *
 * The "move" function is pretty much the same as before. Whenever we move the dot, we move the
 * collider(s) with it. After we move the dot, we check if it went off screen or hit something. If
 * it did, we move the dot back and move its colliders with it.
 *
 * Don't worry too much about how shiftColliders works. It's a short hand way of doing mColliders[ 0
 * ].x = ..., mColliders[ 1 ].x = ..., etc., and it works for this specific case. For your own
 * per-pixel objects, you'll have your own placing functions. And after shiftColliders, have an
 * accessor function to get the colliders.
 *
 * In our collision detection function we have a for loop that calculates the top/bottom/left/right
 * of each collision box in object A. We then calculate the top/bottom/left/right of each collision
 * box in object B. We then check if there is no separating axis. If there no separating axis, we
 * return true. If we get through both sets without a collision, we return false.
 * ATTENZIONE: la logica della verifica della separazione non è banale, e va compresa con degli
 * esempi visivi.
 *
 * Before we go into the main loop, we declare our dot and the other dot we'll be colliding against.
 *
 * Once again, in the main loop we handle events for the dot, move with collision check for the dot,
 * and then finally we render our objects.
 *
 * A questions I get asked a lot is how to make a function that loads an image and auto generates
 * the set of collision boxes for per pixel collision detection. The answer is simple: don't. In
 * most games, you don't want 100% accuracy. The more collision boxes you have, the more collision
 * checks you have and the slower it is. What most games go for is close enough.
 *
 * Also there's one optimization we could have done here. We could have had a bounding box for the
 * dot that encapsulates all the other collision boxes and then checks that one first before getting
 * to the per-pixel collison boxes. This does add one more collision detection, but since it is much
 * more likely that two objects do not collide it will more likely save us additional collision
 * detection. In games, this is usually done with a tree structure that has different levels of
 * detail to allow for early outs to prevent unneeded checks at the per-pixel level. Like in
 * previous tutorials, tree structures are outside the scope of these tutorials.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

// Using SDL, SDL_image, standard IO, vectors, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>

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
static constexpr int CYAN_A = 0xFF; // Alpha component

// Muro
static constexpr int WALL_x = 300;
static constexpr int WALL_y = 40;
static constexpr int WALL_w = 40;
static constexpr int WALL_h = 400;

static const std::string FilePath("dot.bmp");


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


// The dot that will move around on the screen
class Dot
{
  public:

  // Initializes the variables
  Dot( int, int );

  // Takes key presses and adjusts the dot's velocity
  void handleEvent( SDL_Event& );

  // Moves the dot and checks collision
  void move( const std::vector<SDL_Rect>& otherColliders );

  // Shows the dot on the screen
  void render(void);

  // Gets the collision boxes
  std::vector<SDL_Rect>& getColliders();

  private:

  // The dimensions of the dot
  static constexpr int DOT_WIDTH = 20;
  static constexpr int DOT_HEIGHT = 20;

  // Maximum axis velocity of the dot
  static constexpr int DOT_VEL = 1;

  // The X and Y offsets of the dot
  int mPosX, mPosY;

  // The velocity of the dot
  int mVelX, mVelY;

  // Dot's collision boxes
  std::vector<SDL_Rect> mColliders;

  // Moves the collision boxes relative to the dot's offset
  void shiftColliders();
};


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void PressEnter(void);
static bool checkCollision( const std::vector<SDL_Rect>& a, const std::vector<SDL_Rect>& b ); // Box set collision detector


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   gWindow   = NULL; // The window we'll be rendering to
static SDL_Renderer* gRenderer = NULL; // The window renderer

// Scene textures
static LTexture gDotTexture;


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
  else {;} // Nothing to destroy
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
 * @brief Anziché utilizzare "reserve" ed effettuare il push back dei colliders nel vettore,
 * l'autore preferisce chiamare "resize", che crea degli elementi con valori value-initialised, e
 * poi modificare a mano gli elementi.
 *
 * @param x
 * @param y
 **/
Dot::Dot( int x, int y )
{
  // Initialize the offsets
  mPosX = x;
  mPosY = y;

  // Create the necessary SDL_Rects
  mColliders.resize( 11 );

  // Initialize the velocity
  mVelX = 0;
  mVelY = 0;

  // Initialize the collision boxes' width and height
  mColliders[ 0 ].w = 6;
  mColliders[ 0 ].h = 1;

  mColliders[ 1 ].w = 10;
  mColliders[ 1 ].h = 1;

  mColliders[ 2 ].w = 14;
  mColliders[ 2 ].h = 1;

  mColliders[ 3 ].w = 16;
  mColliders[ 3 ].h = 2;

  mColliders[ 4 ].w = 18;
  mColliders[ 4 ].h = 2;

  mColliders[ 5 ].w = 20;
  mColliders[ 5 ].h = 6;

  mColliders[ 6 ].w = 18;
  mColliders[ 6 ].h = 2;

  mColliders[ 7 ].w = 16;
  mColliders[ 7 ].h = 2;

  mColliders[ 8 ].w = 14;
  mColliders[ 8 ].h = 1;

  mColliders[ 9 ].w = 10;
  mColliders[ 9 ].h = 1;

  mColliders[ 10 ].w = 6;
  mColliders[ 10 ].h = 1;

  // Initialize colliders relative to position
  shiftColliders();
}


void Dot::handleEvent( SDL_Event& e )
{
  // If a key was pressed (e.key.repeat == 0 serve per ignorare la ripetizione automatica in caso di
  // tasto tenuto premuto)
  if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
  {
    // Adjust the velocity
    switch( e.key.keysym.sym )
    {
      case SDLK_UP   : mVelY -= DOT_VEL; break;
      case SDLK_DOWN : mVelY += DOT_VEL; break;
      case SDLK_LEFT : mVelX -= DOT_VEL; break;
      case SDLK_RIGHT: mVelX += DOT_VEL; break;
      default        :                   break;
    }
  }
  // If a key was released
  else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
  {
    // Adjust the velocity
    switch( e.key.keysym.sym )
    {
      case SDLK_UP   : mVelY += DOT_VEL; break;
      case SDLK_DOWN : mVelY -= DOT_VEL; break;
      case SDLK_LEFT : mVelX += DOT_VEL; break;
      case SDLK_RIGHT: mVelX -= DOT_VEL; break;
      default        :                   break;
    }
  }
  else { /* Ignore this event */ }
}


/**
 * @brief Calculates new coordinates of the dot based on the current velocity. Whenever we move the
 * dot, we also move the collider(s) with it. After we move the dot, we check if it went off screen
 * or hit something. If it did, we move the dot back and move its colliders with it.
 * 
 * @param otherColliders 
 **/
void Dot::move( const std::vector<SDL_Rect>& otherColliders )
{
  // Move the dot left or right
  mPosX += mVelX;
  shiftColliders();

  // If the dot collided or went too far to the left or right
  if ( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_W ) || checkCollision( mColliders, otherColliders ) )
  {
    // Move back
    mPosX -= mVelX;
    shiftColliders();
  }
  else { /* No collision detected */ }

  // Move the dot up or down
  mPosY += mVelY;
  shiftColliders();

  // If the dot collided or went too far up or down
  if ( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_H ) || checkCollision( mColliders, otherColliders ) )
  {
    // Move back
    mPosY -= mVelY;
    shiftColliders();
  }
  else { /* No collision detected */ }
}


void Dot::render(void)
{
  // Show the dot
  gDotTexture.render( mPosX, mPosY );
}


void Dot::shiftColliders(void)
{
  // The row offset
  int r = 0;

  // Go through the dot's collision boxes
  for( int set = 0; set != static_cast<int>(mColliders.size()); ++set )
  {
    // Center the collision box
    mColliders[ set ].x = mPosX + ( DOT_WIDTH - mColliders[ set ].w ) / 2;

    // Set the collision box at its row offset
    mColliders[ set ].y = mPosY + r;

    // Move the row offset down the height of the collision box
    r += mColliders[ set ].h;
  }
}


std::vector<SDL_Rect>& Dot::getColliders(void)
{
  return mColliders;
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

  // Load dot texture
  if( !gDotTexture.loadFromFile( FilePath.c_str() ) )
  {
    printf( "\nFailed to load dot texture!" );
    success = false;
  }
  else
  {
    printf( "\nDot texture loaded" );
  }

  return success;
}


static void close(void)
{
  // Free loaded images
  gDotTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gRenderer = NULL;
  gWindow   = NULL;

  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}


/**
 * @brief A for loop calculates the top/bottom/left/right of each collision box in object a.
 * 
 * @param a The oject to check with b.
 * @param b The oject to check with a.
 * @return true if a collision has happened; false otherwise
 **/
bool checkCollision( const std::vector<SDL_Rect>& a, const std::vector<SDL_Rect>& b )
{
  bool Result = false;

  // The sides of the rectangles
  int leftA  , leftB;
  int rightA , rightB;
  int topA   , topB;
  int bottomA, bottomB;

  // Go through the A boxes
  for( size_t Abox = 0; Abox != a.size(); Abox++ )
  {
    // Calculate the sides of rect A
    leftA   = a[ Abox ].x;
    rightA  = a[ Abox ].x + a[ Abox ].w;
    topA    = a[ Abox ].y;
    bottomA = a[ Abox ].y + a[ Abox ].h;

    // Go through the B boxes
    for( size_t Bbox = 0; Bbox != b.size(); Bbox++ )
    {
      // Calculate the sides of rect B
      leftB   = b[ Bbox ].x;
      rightB  = b[ Bbox ].x + b[ Bbox ].w;
      topB    = b[ Bbox ].y;
      bottomB = b[ Bbox ].y + b[ Bbox ].h;

      // If no sides from A are outside of B
      if( ( ( bottomA <= topB ) || ( topA >= bottomB ) || ( rightA <= leftB ) || ( leftA >= rightB ) ) == false )
      {
        // A collision is detected
        Result = true;
      }
      else { /* No collision detected */ }
    }
  }

  // If neither set of collision boxes touched
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

      // The dot that will be moving around on the screen
      Dot dot( 0, 0 );

      // The dot that will be collided against
      Dot otherDot( SCREEN_W / 10, SCREEN_H / 10 );

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
          else { /* ignore event */ }

          // Handle input for the dot
          dot.handleEvent( e );
        }

        // Move the dot and check collision
        dot.move( otherDot.getColliders() );

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render dots
        dot.render();
        otherDot.render();

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