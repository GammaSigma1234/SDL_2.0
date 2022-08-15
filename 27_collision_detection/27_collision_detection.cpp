/**
 * @file 27_collision_detection.cpp
 *
 * https://lazyfoo.net/tutorials/SDL/27_collision_detection/index.php
 *
 * @brief In games, you often need to tell if two objects hit each other. For simple games, this is
 * usually done with bounding box collision detection. Collision boxes are a standard way to check
 * collision between two objects. Two polygons are in collision when they are not separated. This
 * form of collision detection is called the separating axis test. If there is no separating axis,
 * then the objects are colliding.
 *
 * We use the dot from the motion tutorial with some new features. The move function takes in a
 * rectangle that is the collision box for the wall and the dot has a data member called mCollider
 * to represent the collision box. We're also declaring a function to check collision between two
 * boxes. In the constructor, we should make sure the collider's dimensions are set.
 *
 * The new moving function now checks if we hit the wall. It works much like before, only now it
 * makes the dot move back if we go off the screen or hit the wall.
 *
 * First we move the dot along the x axis, but we also have to change the position of the collider.
 * Whenever we change the dot's position, the collider's position has to follow. Then we check if
 * the dot has gone off screen or hit the wall. If it does we move the dot back along the x axis.
 * Finally, we do this again for motion on the y axis.
 *
 * The collision detection happens in "checkCollision". The code calculates the top/bottom and
 * left/right of each of the collison boxes.
 *
 * Then we do our separating axis test. First we check the top and bottom of the boxes to see
 * whether they are separated along the y axis. Then we check the left/right to see whether they are
 * separated on the x axis. If there is any separation, then there is no collision and we return
 * false. If we cannot find any separation, then there is a collision and we return true.
 *
 * Note: SDL does have some built in collision detection functions, but for this tutorial set we'll
 * be hand rolling our own. Mainly because it's important to know how these work and secondly
 * because if you can roll your own you can use your collision detection with SDL rendering, OpenGL,
 * Direct3D, Mantle, Metal, or any other rendering API. Also remember that SDL uses a y axis that
 * points down as opposed to the cartesian coordinate which points upward. This means our bottom/top
 * comparisons are also going to be inverted.
 *
 * Before we enter the main loop, we declare the dot and define the postion and dimensions of the
 * wall.
 *
 * Then there is the main loop with the dot handling events, moving while checking for collision
 * against the wall and finally rendering the wall and the dot onto the screen.
 *
 * These next sections are for future reference. Odds are if you're reading this tutorial, you're a
 * beginner and this stuff is way too advanced. This is more for down the road when you need to use
 * more advanced collision detection.
 *
 * When you're starting out and just want to make something simple like tetris, this sort of
 * collision detection is fine. For something like a physics simulator things get much more
 * complicated.
 *
 * For something like a rigid body simulator, we have our logic do this every frame:
 *
 * 1) Apply all the forces to all the objects in the scene (gravity, wind, propulsion, etc).
 *
 * 2) Move the objects by applying the acceleration and velocity to the position.
 *
 * 3) Check collisions for all of the objects and create a set of contacts. A contact is a data
 *    structure that typically contains pointers to the two objects that are colliding, a normal
 *    vector from the first to the second object, and the amount the objects are penetrating.
 *
 * 4) Take the set of generated contacts and resolve the collisions. This typically involves
 *    checking for contacts again (within a limit) and resolving them.
 *
 * Now if you're barely learning collision detection, this is out of your league for now. This would
 * take an entire tutorial set (that I currently do not have time to make) to explain it. Not only
 * that it involves vector math and physics which is beyond the scope of these tutorials. Just keep
 * this in mind later on when you need games that have large amounts of colliding objects and are
 * wondering how the over all structure for a physics engine works.
 *
 * Another thing is that the boxes we have here are AABBs or axis aligned bounding boxes. This means
 * they have sides that are aligned with the x and y axis. If you want to have boxes that are
 * rotated, you can still use the separating axis test on OBBs (oriented bounding boxes). Instead of
 * projecting the corners on the x and y axis, you project all of the corners of the boxes on the I
 * and J axis for each of the boxes. You then check if the boxes are separated along each axis. You
 * can extend this further for any type of polygon by projecting all of the corners of each axis
 * along each of the polygon's axis to see if there is any separation. This all involves vector math
 * and this as mentioned before is beyond the scope of this tutorial set.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

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
  Dot(void);

  // Takes key presses and adjusts the dot's velocity
  void handleEvent( SDL_Event& );

  // Moves the dot and checks collision
  void move( SDL_Rect& Wall );

  // Shows the dot on the screen
  void render(void);

  private:

  // The dimensions of the dot
  static constexpr int DOT_WIDTH  = 20;
  static constexpr int DOT_HEIGHT = 20;

  // Maximum axis velocity of the dot
  static constexpr int DOT_VEL = 5;

  // The X and Y offsets of the dot
  int mPosX, mPosY;

  // The velocity of the dot
  int mVelX, mVelY;

  // Dot's collision box
  SDL_Rect mCollider;
};


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init(void);
static bool loadMedia(void);
static void close(void);
static void PressEnter(void);
static bool checkCollision( SDL_Rect a, SDL_Rect b ); // Box collision detector


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
  else
  {;} // Nothing to destroy
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
  else
  {;}

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


Dot::Dot(void)
{
  // Initialize the offsets
  mPosX = 0;
  mPosY = 0;

  // Set collision box dimension
  mCollider.w = DOT_WIDTH;
  mCollider.h = DOT_HEIGHT;

  // Initialize the velocity
  mVelX = 0;
  mVelY = 0;
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
  else
  {;}
}

void Dot::move( SDL_Rect& Wall )
{
  // Move the dot left or right
  mPosX      += mVelX;
  mCollider.x = mPosX;

  // If the dot collided or went too far to the left or right
  if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_W ) || checkCollision( mCollider, Wall ) )
  {
    // Move back
    mPosX      -= mVelX;
    mCollider.x = mPosX;
  }
  else { /* OK */ }

  // Move the dot up or down
  mPosY      += mVelY;
  mCollider.y = mPosY;

  // If the dot collided or went too far up or down
  if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_H ) || checkCollision( mCollider, Wall ) )
  {
    // Move back
    mPosY      -= mVelY;
    mCollider.y = mPosY;
  }
  else { /* OK */ }
}


void Dot::render(void)
{
    // Show the dot
  gDotTexture.render( mPosX, mPosY );
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

  // Load press texture
  if( !gDotTexture.loadFromFile( FilePath.c_str() ) )
  {
    printf( "Failed to load dot texture!\n" );
    success = false;
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

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
  bool Result = true;

  // The sides (vertices) of the rectangles
  int leftA  , leftB;
  int rightA , rightB;
  int topA   , topB;
  int bottomA, bottomB;

  // Calculate the sides (vertices) of rect A
  leftA   = a.x;
  rightA  = a.x + a.w;
  topA    = a.y;
  bottomA = a.y + a.h;

  // Calculate the sides (vertices) of rect B
  leftB   = b.x;
  rightB  = b.x + b.w;
  topB    = b.y;
  bottomB = b.y + b.h;

  // Separating axis test: check whether any of the sides (vertices) from A are outside of B
  if( bottomA <= topB )
  {
    return false;
  }
  else{;}

  if( topA >= bottomB )
  {
    return false;
  }
  else{;}

  if( rightA <= leftB )
  {
    return false;
  }
  else{;}

  if( leftA >= rightB )
  {
    return false;
  }
  else{;}

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
      Dot dot;

      // Set the wall
      SDL_Rect Wall;
      Wall.x = WALL_x;
      Wall.y = WALL_y;
      Wall.w = WALL_w;
      Wall.h = WALL_h;

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
          else { /* Ignore event */ }

          // Handle input for the dot
          dot.handleEvent( e );
        }

        // Move the dot and check collision
        dot.move( Wall );

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render wall
        SDL_SetRenderDrawColor( gRenderer, BLACK_R, BLACK_G, BLACK_B, BLACK_A);
        SDL_RenderDrawRect( gRenderer, &Wall );

        // Render dot
        dot.render();

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