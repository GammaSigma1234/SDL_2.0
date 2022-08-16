/**
 * @file 29_circular_collision_detection.cpp
 *
 * @brief https://lazyfoo.net/tutorials/SDL/29_circular_collision_detection/index.php
 *
 * Along with collision boxes, circles are the most common form of collider. Here we'll be checking
 * collision between two circles and a circle and a box.
 *
 * Checking collision between two circles is easy. All you have to do is check whether the distance
 * between the center of each circle is less than the sum of their radii.
 *
 * For box/circle collision, you have to find the point on the collision box that is closest to the
 * center of the circle. If that point is less than the radius of the circle, there is a collision.
 *
 * SDL has a built in rectangle structure, but we have to make our own circle structure with a
 * position and radius.
 *
 * The dot class is as from the previous collision detection tutorials with some more additons. The
 * move function takes in a circle and a rectangle to check collision against when moving. We also
 * now have a circle collider instead of a rectangle collider.
 *
 * For this tutorial we have our collision detection functions for circle/circle and
 * circle/rectangle collisions. We also have a function that calculates the distance between two
 * points squared. Using the distance squared instead of the distance is an optimization we'll go
 * into more detail later.
 *
 * The constructor takes in a position and initializes the colliders and velocity.
 *
 * As in previous collision detection tutorials, we move along the x axis, check collision against
 * the edges of the screen, and check against the other scene objects. If the dot hits something, we
 * move back. As always, whenever the dot moves, its colliders move with it. Then, we do this again
 * for the y axis.
 *
 * The rendering code is a little different. SDL_Rects have their position at the top left where our
 * circle structure has the position at the center. This means we need to offset the render position
 * to the top left of the circle by subtracting the radius from the x and y position.
 *
 * "checkCollision" is our (overloaded) circle to circle collision detector. It simply checks if the
 * distance squared between the centers is less than the sum of the radii squared. If it is, there
 * is a collison.
 *
 * We are using the distance squared as opposed to the plain distance because calculating the
 * distance involves a square root and calculating a square root is a relatively expensive
 * operation. Fortunately if x > y then x^2 > y^2, so we can save a square root operation by just
 * comparing the distance squared.
 *
 * "checkCollision" (other overload): to check if a box and circle collided, we need to find the
 * closest point on the box. If the circle's center is to the left of the box, the x position of the
 * closest point is on the left side of the box. If the circle's center is to the right of the box,
 * the x position of the closest point is on the right side of the box. If the circle's center is
 * inside of the box, the x position of the closest point is the same as the x position of the
 * circle. Then we find the closest y position much like we did the x position. If the distance
 * squared between the closest point on the box and the center of the circle is less than the
 * circle's radius squared, then there is a collision.
 *
 * The distance squared function is just a distance calculation ( squareRoot( x^2 + y^2 ) ) without
 * the square root.
 *
 * Before we enter the main loop we define the scene objects. Finally, in our main loop we handle
 * input, move the dot with collision check and render the scene objects to the screen.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022) and may not be
 * redistributed without written permission.
 **/

// Using SDL, SDL_image, standard IO, vectors, and strings
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

// Colore nero
static constexpr int BLACK_R = 0x00; // Amount of red   needed to compose black
static constexpr int BLACK_G = 0x00; // Amount of green needed to compose black
static constexpr int BLACK_B = 0x00; // Amount of blue  needed to compose black
static constexpr int BLACK_A = 0xFF; // Alpha component

// Muro
static constexpr int WALL_x = 300;
static constexpr int WALL_y = 40;
static constexpr int WALL_w = 40;
static constexpr int WALL_h = 400;

static const std::string FilePath("dot.bmp");


/***************************************************************************************************
* Classes
****************************************************************************************************/

// A circle stucture
struct Circle
{
  int x, y;
  int r;
};


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
 * @brief The dot that will move around on the screen.
 **/
class Dot
{
  public:

  // The dimensions of the dot
  static constexpr int DOT_WIDTH  = 20;
  static constexpr int DOT_HEIGHT = 20;

  // Initializes the variables
  Dot( int, int );

  // Takes key presses and adjusts the dot's velocity
  void handleEvent( SDL_Event& );

  // Moves the dot and checks collision
  void move( SDL_Rect&, Circle& );

  // Shows the dot on the screen
  void render(void);

  // Gets collision circle
  Circle& getCollider();

  private:

  // Maximum axis velocity of the dot
  static constexpr int DOT_VEL = 1;

  // The X and Y offsets of the dot
  int mPosX, mPosY;

  // The velocity of the dot
  int mVelX, mVelY;

  // Dot's collision circle
  Circle mCollider;

  // Moves the collision circle relative to the dot's offset
  void shiftColliders();
};


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool   init           ( void );
static bool   loadMedia      ( void );
static void   close          ( void );
static void   PressEnter     ( void );
static bool   checkCollision ( Circle&, Circle& );   // Circle/Circle collision detector
static bool   checkCollision ( Circle&, SDL_Rect& ); // Circle/Box    collision detector
static double distanceSquared( int x1, int y1, int x2, int y2 ); // Calculates distance squared between two points


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


Dot::Dot( int x, int y )
{
  // Initialize the offsets
  mPosX = x;
  mPosY = y;

  // Set collision circle size
  mCollider.r = DOT_WIDTH / 2;

  // Initialize the velocity
  mVelX = 0;
  mVelY = 0;

  // Move collider relative to the circle
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
 * @brief Move object - Check for collision - Undo move if collision detected
 *
 * @param square Square to check collision against
 * @param circle Circle to check collision against
 **/
void Dot::move( SDL_Rect& square, Circle& circle )
{
  // Move the dot left or right
  mPosX += mVelX;
  shiftColliders();

  // If the dot collided or went too far to the left or right
  if( ( mPosX - mCollider.r < 0 ) || ( mPosX + mCollider.r > SCREEN_W ) || checkCollision( mCollider, square ) || checkCollision( mCollider, circle ) )
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
  if( ( mPosY - mCollider.r < 0 ) || ( mPosY + mCollider.r > SCREEN_H ) || checkCollision( mCollider, square ) || checkCollision( mCollider, circle ) )
  {
    // Move back
    mPosY -= mVelY;
    shiftColliders();
  }
  else { /* No collision detected */ }
}


void Dot::render(void)
{
  /* SDL_Rects have their position at the top left where our circle structure has the position at
   * the center. This means we need to offset the render position to the top left of the circle by
   * subtracting the radius from the x and y position. */

  // Show the dot
  gDotTexture.render( mPosX - mCollider.r, mPosY - mCollider.r );
}


void Dot::shiftColliders(void)
{
  // Align collider to center of dot
  mCollider.x = mPosX;
  mCollider.y = mPosY;
}


Circle& Dot::getCollider(void)
{
  return mCollider;
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
 * @brief Circle-to-circle collision check.
 *
 * @param a Circle
 * @param b Circle
 * @return true if collision happened; false otherwise
 **/
bool checkCollision( Circle& a, Circle& b )
{
  // Calculate total radius squared
  int totalRadiusSquared = a.r + b.r;
      totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

  // If the distance between the centers of the circles is less than the sum of their radii
  if( distanceSquared( a.x, a.y, b.x, b.y ) < ( totalRadiusSquared ) )
  {
    // The circles have collided
    return true;
  }
  else { /* No collision detected */ }

  // If not
  return false;
}


/**
 * @brief Circle-to-Rect collision check.
 *
 * @param a Circle
 * @param b Rect
 * @return true if collision happened; false otherwise
 **/
bool checkCollision( Circle& a, SDL_Rect& b )
{
  // Closest point on collision box
  int cX, cY;

  // Find closest x offset
  if ( a.x < b.x )
  {
    cX = b.x;
  }
  else if ( a.x > b.x + b.w )
  {
    cX = b.x + b.w;
  }
  else
  {
    cX = a.x;
  }

  // Find closest y offset
  if ( a.y < b.y )
  {
    cY = b.y;
  }
  else if ( a.y > b.y + b.h )
  {
    cY = b.y + b.h;
  }
  else
  {
    cY = a.y;
  }

  // If the closest point is inside the circle
  if ( distanceSquared( a.x, a.y, cX, cY ) < ( a.r * a.r ) )
  {
    // This box and the circle have collided
    return true;
  }
  else { /* No collision detected */ }

  // If the shapes have not collided
  return false;
}


double distanceSquared( int x1, int y1, int x2, int y2 )
{
  int deltaX = x2 - x1;
  int deltaY = y2 - y1;

  return deltaX * deltaX + deltaY * deltaY;
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
      Dot dot( Dot::DOT_WIDTH / 2, Dot::DOT_HEIGHT / 2 );
      Dot otherDot( SCREEN_W / 4, SCREEN_H / 4 );

      // Set the wall
      SDL_Rect wall;
      wall.x = 300;
      wall.y = 40;
      wall.w = 40;
      wall.h = 400;

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
        dot.move( wall, otherDot.getCollider() );

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render wall
        SDL_SetRenderDrawColor( gRenderer, BLACK_R, BLACK_G, BLACK_B, BLACK_A );
        SDL_RenderDrawRect( gRenderer, &wall );

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