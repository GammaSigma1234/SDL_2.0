/**
 * @file 38_particle_engines.cpp
 *
 * @brief Particles are just mini-animations. What we're going to do is take these animations and
 * spawn them around a dot to create a trail of colored shimmering particles.
 *
 * Sunto:
 * - In questo tutorial si torna al renderer globale, a differenza dei precedenti in cui veniva
 *   creato da una classe finestra "LWindow"
 * - Inoltre, si abbandona l'uso della classe finestra, tornando alla versione globale
 *   degli esempi precedenti al 36.
 * - Pertanto, l'init privato torna ad essere molto simile agli init precedenti al 36.
 * - Si torna a far uso della classe LTexture.
 * - LoadMedia carica tutte le textures necessarie, e assegna il valore di alpha mediante il metodo
 *   "setAlpha", che chiama "SDL_SetTextureAlphaMod". To give our particles a semi transparent look,
 *   we set their alpha to 192. Ogni texture è caricata mediante il metodo "loadFromFile", identico
 *   ai precedenti.
 * - Viene creata una nuova classe "Particle", per gestire l'effetto scia. We have a constructor to
 *   set the position, a function to render it, and a function to tell if the particle is dead. In
 *   terms of data members we have a position, a frame of animation, and a texture we'll render
 *   with. Randomizzazione di mFrame (numero usato per decidere quando eliminare una particella) per
 *   migliorare l'effetto scintillio.
 * - Ritorna la classe "Dot", with an array of particles and a function to render the particles on
 *   the dot. The constructor/destructor now have to allocate/deallocate the particles we render over
 *   the dot.
 * - Le particelle vengono gestite mediante un array di puntatori di tipo Particle "mParticles".
 *   Quando un nuovo Dot viene creato, le particelle vengono create su memoria dinamica e l'array
 *   viene riempito.
 * - For our particle constructor, we initialize the position around the given position with some
 *   randomness to it. We then initialize the frame of animation with some randomness so the
 *   particles will have varying life. Finally, we pick the type of texture we'll use for the
 *   particle also at random.
 * - The dot's rendering function now calls our particle rendering function. The particle rendering
 *   function checks if there are any particles that are dead and replaces them. After the dead
 *   particles are replaced we render all the current particles to the screen.
 * - Again, since our code is well encapsulated the code in the main loop hardly changes.
 *
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/


/**************************************************************************************************
* Includes
***************************************************************************************************/

// Using SDL, SDL_image, standard I/O, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector> // Aggiunto da GS


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int FIRST_ONE = -1;

static constexpr int WINDOW_W = 640; // Screen's width
static constexpr int WINDOW_H = 480; // Screen's heigth

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

static constexpr Uint8 ALPHA = (Uint8)192;

// Aggiunto da GS
enum class Paths
{
  dot,
  red,
  green,
  blue,
  shimmer
};

// Aggiunto da GS
static const std::vector<std::string> Paths_Vec
{
  "dot.bmp",
  "red.bmp",
  "green.bmp",
  "blue.bmp",
  "shimmer.bmp",
};

// Particle count
static constexpr int TOTAL_PARTICLES = 20;


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


class Particle
{
  public:

  // Initialize position and animation
  Particle( int, int );

  // Shows the particle
  void render(void);

  // Checks if particle is dead
  bool isDead(void);

  private:

  // Offsets
  int mPosX, mPosY;

  // Current frame of animation
  int mFrame;

  // Type of particle
  LTexture* mTexture;
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

  // Initializes the variables and allocates particles
  Dot(void);

  // Deallocates particles
  ~Dot(void);

  // Takes key presses and adjusts the dot's velocity
  void handleEvent( SDL_Event& );

  // Moves the dot
  void move(void);

  // Shows the dot on the screen
  void render(void);

  private:
  // The particles
  Particle* mParticles[ TOTAL_PARTICLES ];

  // Shows the particles
  void renderParticles(void);

  // The X and Y offsets of the dot
  int mPosX, mPosY;

  // The velocity of the dot
  int mVelX, mVelY;
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

// Scene textures
LTexture gDotTexture;
LTexture gRedTexture;
LTexture gGreenTexture;
LTexture gBlueTexture;
LTexture gShimmerTexture;


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


Particle::Particle( int x, int y )
{
  // Set offsets
  mPosX = x - 5 + ( rand() % 25 );
  mPosY = y - 5 + ( rand() % 25 );

  // Initialize animation (randomizzazione di mFrame per migliorare l'effetto scintillio)
  mFrame = rand() % 5;

  // Set type
  switch( rand() % 3 )
  {
    case 0: mTexture = &gRedTexture;   break;
    case 1: mTexture = &gGreenTexture; break;
    case 2: mTexture = &gBlueTexture;  break;
  }
}


void Particle::render(void)
{
  // Show image
  mTexture->render( mPosX, mPosY );

  // Show shimmer
  if( mFrame % 2 == 0 )
  {
    gShimmerTexture.render( mPosX, mPosY );
  }
  else { /* No shimmer to show */ }

  // Animate
  mFrame++;
}


/**
 * @brief Once the particle has rendered for a max of 10 frames, we mark it as dead.
 **/
bool Particle::isDead(void)
{
  return mFrame > 10;
}


Dot::Dot(void)
{
  // Initialize the offsets
  mPosX = 0;
  mPosY = 0;

  // Initialize the velocity
  mVelX = 0;
  mVelY = 0;

  // Initialize particles
  for( int i = 0; i != TOTAL_PARTICLES; ++i )
  {
    mParticles[ i ] = new Particle( mPosX, mPosY );
  }
}


Dot::~Dot(void)
{
  // Delete particles
  for( int i = 0; i != TOTAL_PARTICLES; ++i )
  {
    delete mParticles[ i ];
  }
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


void Dot::move(void)
{
  // Move the dot left or right
  mPosX += mVelX;

  // If the dot went too far to the left or right
  if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > WINDOW_W ) )
  {
    // Move back
    mPosX -= mVelX;
  }

  // Move the dot up or down
  mPosY += mVelY;

  // If the dot went too far up or down
  if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > WINDOW_H ) )
  {
    // Move back
    mPosY -= mVelY;
  }
}


void Dot::render(void)
{
  // Show the dot
  gDotTexture.render( mPosX, mPosY );

  // Show particles on top of dot
  renderParticles();
}


void Dot::renderParticles(void)
{
  // Go through particles
  for( int i = 0; i != TOTAL_PARTICLES; ++i )
  {
    // Delete and replace dead particles
    if( mParticles[ i ]->isDead() )
    {
      delete mParticles[ i ];
      mParticles[ i ] = new Particle( mPosX, mPosY );
    }
    else { /* Particle not dead yet */ }
  }

  // Show particles
  for( int i = 0; i != TOTAL_PARTICLES; ++i )
  {
    mParticles[ i ]->render();
  }
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
static bool loadMedia(void)
{
  // Loading success flag
  bool success = true;

  constexpr size_t DotPath( static_cast<size_t>(Paths::dot) 		);
  constexpr size_t RedPath( static_cast<size_t>(Paths::red) 		);
  constexpr size_t GrnPath( static_cast<size_t>(Paths::green) 	);
  constexpr size_t BluPath( static_cast<size_t>(Paths::blue) 		);
  constexpr size_t ShmPath( static_cast<size_t>(Paths::shimmer) );

  // Load dot texture
  if( !gDotTexture.loadFromFile( Paths_Vec[DotPath]) )
  {
    printf( "Failed to load dot texture!\n" );
    success = false;
  }

  // Load red texture
  if( !gRedTexture.loadFromFile( Paths_Vec[RedPath]) )
  {
    printf( "Failed to load red texture!\n" );
    success = false;
  }

  // Load green texture
  if( !gGreenTexture.loadFromFile( Paths_Vec[GrnPath]) )
  {
    printf( "Failed to load green texture!\n" );
    success = false;
  }

  // Load blue texture
  if( !gBlueTexture.loadFromFile( Paths_Vec[BluPath]) )
  {
    printf( "Failed to load blue texture!\n" );
    success = false;
  }

  // Load shimmer texture
  if( !gShimmerTexture.loadFromFile( Paths_Vec[ShmPath]) )
  {
    printf( "Failed to load shimmer texture!\n" );
    success = false;
  }

  // Set texture transparency
  gRedTexture.setAlpha( ALPHA );
  gGreenTexture.setAlpha( ALPHA );
  gBlueTexture.setAlpha( ALPHA );
  gShimmerTexture.setAlpha( ALPHA );

  return success;
}


static void close(void)
{
  // Free loaded images
  gDotTexture.free();
  gRedTexture.free();
  gGreenTexture.free();
  gBlueTexture.free();
  gShimmerTexture.free();

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
  gRenderer = NULL;

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

      // The dot that will be moving around on the screen
      Dot dot;

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

          // Handle input for the dot
          dot.handleEvent( e );
        }

        // Move the dot
        dot.move();

        // Clear screen
        SDL_SetRenderDrawColor( gRenderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( gRenderer );

        // Render objects
        dot.render();

        // Update screen
        SDL_RenderPresent( gRenderer );
      }
    }
  }

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