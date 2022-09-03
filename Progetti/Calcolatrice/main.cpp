/**
 * @file main.cpp
 *
 * @brief
 **/

// Using SDL, SDL_image, standard IO, vectors, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>
#include "colours.hpp"


/**************************************************************************************************
* Private constants
***************************************************************************************************/

static constexpr int INIT_F1 = -1; // Initialise the first rendering driver supporting the requested flags

static constexpr int WINDOW_W = 800; // Window's width
static constexpr int WINDOW_H = 600; // Window's heigth

static constexpr SDL_Rect SCREEN  {20, 20, 500, 50}; // The calculator's screen
static /* constexpr */ SDL_Rect BUTTON_1{0 , 0 , 100, 100};

// static const std::string DotPath  ("dot.bmp");  // Dot        texture's path
// static const std::string BGPath   ("bg.png");   // Background texture's path
static const std::string FontPath       ("RachelBrown.ttf");
static const std::string SpriteSheetPath("SpriteSheet.png");


/***************************************************************************************************
* Classes
****************************************************************************************************/

/**
 * @brief A circle stucture. Defines the centre (x, y) and the radius (r) of the circle.
 **/
// struct Circle
// {
//   int x, y;
//   int r;
// };


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

  void render( int = 0, int = 0, SDL_Rect* = NULL, double = 0.0, SDL_Point* = NULL, SDL_RendererFlip = SDL_FLIP_NONE );

  // Gets image dimensions
  int getWidth (void) const;
  int getHeight(void) const;

  private:
  // The actual hardware texture
  SDL_Texture* m_Texture;

  // Image dimensions
  int m_Width;
  int m_Height;
};


/**
 * @brief The dot that will move around on the screen.
 **/
// class Dot
// {
//   public:

//   // Initializes the variables
//   Dot(void);

//   // Takes key presses and adjusts the dot's velocity
//   void handleEvent( SDL_Event& );

//   void ProcessMovement(void);

//   // Shows the dot on the screen relative to the camera
//   void render( int, int );

//   // Position accessors
//   int getPosX(void) const;
//   int getPosY(void) const;

//   // Velocity accessors
//   double getVelX_Debug(void) const;
//   double getVelY_Debug(void) const;

//   // Number of decimal digits used in the acceleration DOT_A
//   static constexpr size_t NUM_OF_DIGITS = 1;

//   private:

//   // The dimensions (width and height) of the dot
//   static constexpr int DOT_W = 20;
//   static constexpr int DOT_H = 20;

//   /**
//    * @brief Acceleration directions
//    **/
//   enum class ACCEL_DIR
//   {
//     UP,
//     DOWN,
//     LEFT,
//     RIGHT,

//     HOW_MANY
//   };

//   void AccelReq  ( ACCEL_DIR );
//   void AccelStop ( ACCEL_DIR );

//   // Maximum axis velocity of the dot
//   static constexpr int DOT_MAX_VEL = 10;

//   // Acceleration addition
//   static constexpr double DOT_A = 0.4;

//   // The X and Y offsets of the dot
//   int m_PosX, m_PosY;

//   // The velocity of the dot
//   double m_VelX, m_VelY;

//   // Used to set the direction of the acceleration
//   bool m_IsAccelUp, m_IsAccelDown, m_IsAccelLeft, m_IsAccelRight;
// };


/***************************************************************************************************
* Private global variables
****************************************************************************************************/

static SDL_Window*   g_Window   = NULL; // The window we'll be rendering to
static SDL_Renderer* g_Renderer = NULL; // The window renderer

// Globally used font
static TTF_Font* g_Font = NULL;
// static SDL_Color g_TextColorBlack = { BLACK_R, BLACK_G, BLACK_B, BLACK_A };

// Textures
static LTexture g_SpriteSheet;
// static LTexture g_BGTexture;
// static LTexture g_xPosText_Debug;
// static LTexture g_yPosText_Debug;
// static LTexture g_xVelText_Debug;
// static LTexture g_yVelText_Debug;


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static bool init      ( void );
static bool loadMedia ( void );
static void close     ( void );


/***************************************************************************************************
* Methods definitions
****************************************************************************************************/

LTexture::LTexture(void)
  : m_Texture(NULL), m_Width(0), m_Height(0)
{ /* Initialize members */ }


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
    newTexture = SDL_CreateTextureFromSurface( g_Renderer, loadedSurface );

    if( newTexture == NULL )
    {
      printf( "\nUnable to create texture from \"%s\"! SDL Error: %s", path.c_str(), SDL_GetError() );
    }
    else
    {
      printf( "\nTexture created from \"%s\"", path.c_str() );

      // Get image dimensions
      m_Width  = loadedSurface->w;
      m_Height = loadedSurface->h;
    }

    // Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
  }

  // Return success
  m_Texture = newTexture;

  return m_Texture != NULL;
}


#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( const std::string& textureText, SDL_Color textColor )
{
  // Get rid of preexisting texture
  free();

  // Render text surface
  SDL_Surface* textSurface = TTF_RenderText_Solid( g_Font, textureText.c_str(), textColor );

  if( textSurface != NULL )
  {
    // Create texture from surface pixels
    m_Texture = SDL_CreateTextureFromSurface( g_Renderer, textSurface );

    if( m_Texture == NULL )
    {
      printf( "\nUnable to create texture from rendered text! SDL Error: %s", SDL_GetError() );
    }
    else
    {
      // Get image dimensions
      m_Width  = textSurface->w;
      m_Height = textSurface->h;
    }

    // Get rid of old surface
    SDL_FreeSurface( textSurface );
  }
  else
  {
    printf( "\nUnable to render text surface! SDL_ttf Error: %s", TTF_GetError() );
  }

  // Return success
  return m_Texture != NULL;
}
#endif


void LTexture::free(void)
{
  // Free texture if it exists
  if( m_Texture != NULL )
  {
    SDL_DestroyTexture( m_Texture );
    m_Texture = NULL;
    m_Width = 0;
    m_Height = 0;
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
  SDL_SetTextureColorMod( m_Texture, red, green, blue );
}


/**
 * @brief Set blending function
 *
 * @param blending
 **/
void LTexture::setBlendMode( SDL_BlendMode blending )
{
  SDL_SetTextureBlendMode( m_Texture, blending );
}


/**
 * @brief Modulate texture alpha
 *
 * @param alpha
 **/
void LTexture::setAlpha( Uint8 alpha )
{
  SDL_SetTextureAlphaMod( m_Texture, alpha );
}


/**
 * @brief Renders the texture.
 *
 * @param x x position on the window where the texture will be rendered.
 * @param y y position on the window where the texture will be rendered.
 * @param clip used to render only a portion of the texture. Defaults to NULL (renders the whole texture).
 * @param angle the rotation angle of the texture around the centre. Defaults to 0.0 (no rotation).
 * @param centre centre of rotation. Defaults to NULL (renders around 0, 0).
 * @param flip flips the image around the vertical or horizontal axis. Defaults to SDL_flip_NONE (no flipping).
 **/
void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* centre, SDL_RendererFlip flip )
{
  // Set rendering space and render to screen
  SDL_Rect renderQuad = { x, y, m_Width, m_Height };

  // Set clip rendering dimensions
  if( clip != NULL )
  {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }
  else { /* Render the whole texture */ }

  // Render to screen
  SDL_RenderCopyEx( g_Renderer, m_Texture, clip, &renderQuad, angle, centre, flip );
}


int LTexture::getWidth(void) const
{
  return m_Width;
}


int LTexture::getHeight(void) const
{
  return m_Height;
}


// Dot::Dot(void)
//   : m_PosX(0), m_PosY(0), m_VelX(0.0), m_VelY(0.0),
//     m_IsAccelUp(false), m_IsAccelDown(false), m_IsAccelLeft(false), m_IsAccelRight(false)
// { /* Initialise the offsets, the velocity, and the acceleration */ }


// void Dot::handleEvent( SDL_Event& e )
// {
//   // If a key was pressed (e.key.repeat == 0 serve per ignorare la ripetizione automatica in caso di
//   // tasto tenuto premuto)
//   if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
//   {
//     // Adjust the velocity
//     switch( e.key.keysym.sym )
//     {
//       case SDLK_UP   : AccelReq(Dot::ACCEL_DIR::UP);    break;
//       case SDLK_DOWN : AccelReq(Dot::ACCEL_DIR::DOWN);  break;
//       case SDLK_LEFT : AccelReq(Dot::ACCEL_DIR::LEFT);  break;
//       case SDLK_RIGHT: AccelReq(Dot::ACCEL_DIR::RIGHT); break;
//       default        :                                  break;
//     }
//   }
//   // If a key was released
//   else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
//   {
//     // Adjust the velocity
//     switch( e.key.keysym.sym )
//     {
//       case SDLK_UP   : AccelStop(Dot::ACCEL_DIR::UP);    break;
//       case SDLK_DOWN : AccelStop(Dot::ACCEL_DIR::DOWN);  break;
//       case SDLK_LEFT : AccelStop(Dot::ACCEL_DIR::LEFT);  break;
//       case SDLK_RIGHT: AccelStop(Dot::ACCEL_DIR::RIGHT); break;
//       default        :                                   break;
//     }
//   }
//   else { /* Ignore this event */ }
// }


// /**
//  * @brief Processes the movement of the dot.
//  **/
// void Dot::ProcessMovement(void)
// {
//  /******************************************************************
//  * Process the acceleration of the dot, based on the user's request
//  *******************************************************************/

//   if ( m_IsAccelUp )
//   {
//     m_VelY = m_VelY - DOT_A;
//   }
//   else { /* No upwards acceleration requested */ }

//   if ( m_IsAccelDown )
//   {
//     m_VelY = m_VelY + DOT_A;
//   }
//   else { /* No downwards acceleration requested */ }

//   if ( m_IsAccelLeft )
//   {
//     m_VelX = m_VelX - DOT_A;
//   }
//   else { /* No upwards acceleration requested */ }

//   if ( m_IsAccelRight )
//   {
//     m_VelX = m_VelX + DOT_A;
//   }
//   else { /* No downwards acceleration requested */ }


//   /****************************
//   * Speed check and saturation
//   *****************************/

//   if ( m_VelY < -DOT_MAX_VEL )
//   {
//     m_VelY = -DOT_MAX_VEL;
//   }
//   else { /* Max speed on negative y axis not yet reached */ }

//   if ( m_VelY > DOT_MAX_VEL )
//   {
//     m_VelY = DOT_MAX_VEL;
//   }
//   else { /* Max speed on positive y axis not yet reached */ }

//   if ( m_VelX > DOT_MAX_VEL )
//   {
//     m_VelX = DOT_MAX_VEL;
//   }
//   else { /* Max speed on positive x axis not yet reached */ }

//   if ( m_VelX < -DOT_MAX_VEL )
//   {
//     m_VelX = -DOT_MAX_VEL;
//   }
//   else { /* Max speed on negative x axis not yet reached */ }


//  /****************************
//  * Move the dot left or right
//  *****************************/

//   m_PosX += static_cast<int>(m_VelX);


//  /**********************************************
//  * If the dot went too far to the left
//  ***********************************************/

//   if( m_PosX < 0 )
//   {
//     // Move back
//     m_IsAccelLeft  = false;
//     m_IsAccelRight = false;
//     m_PosX = 0;
//     m_VelX = 0;
//   }
//   else { /* Movement was OK */ }


//  /**********************************************
//  * If the dot went too far to the right
//  ***********************************************/

//   if( m_PosX + DOT_W > LEVEL_W )
//   {
//     // Move back
//     m_IsAccelLeft  = false;
//     m_IsAccelRight = false;
//     m_PosX = LEVEL_W - DOT_W;
//     m_VelX = 0;
//   }
//   else { /* Movement was OK */ }


//  /*************************
//  * Move the dot up or down
//  **************************/

//   m_PosY += static_cast<int>(m_VelY);


//  /************************************
//  * If the dot went too far up
//  *************************************/

//   if( m_PosY < 0 )
//   {
//     // Move back
//     m_IsAccelUp   = false;
//     m_IsAccelDown = false;
//     m_PosY = 0;
//     m_VelY = 0;
//   }
//   else { /* Movement was OK */ }


//  /************************************
//  * If the dot went too far down
//  *************************************/

//   if( m_PosY + DOT_H > LEVEL_H )
//   {
//     // Move back
//     m_IsAccelUp   = false;
//     m_IsAccelDown = false;
//     m_PosY = LEVEL_H - DOT_H;
//     m_VelY = 0;
//   }
//   else { /* Movement was OK */ }
// }


// /**
//  * @brief Requests acceleration in a given direction.
//  **/
// void Dot::AccelReq( Dot::ACCEL_DIR Direction )
// {
//   switch ( Direction )
//   {
//     case Dot::ACCEL_DIR::UP:
//       m_IsAccelUp = true;
//       break;

//     case Dot::ACCEL_DIR::DOWN:
//       m_IsAccelDown = true;
//       break;

//     case Dot::ACCEL_DIR::LEFT:
//       m_IsAccelLeft = true;
//       break;

//     case Dot::ACCEL_DIR::RIGHT:
//       m_IsAccelRight = true;
//       break;

//     default:
//       break;
//   }
// }


// /**
//  * @brief Stops acceleration requests in a given direction.
//  **/
// void Dot::AccelStop( Dot::ACCEL_DIR Direction )
// {
//   switch ( Direction )
//   {
//     case Dot::ACCEL_DIR::UP:
//       m_IsAccelUp = false;
//       break;

//     case Dot::ACCEL_DIR::DOWN:
//       m_IsAccelDown = false;
//       break;

//     case Dot::ACCEL_DIR::LEFT:
//       m_IsAccelLeft = false;
//       break;

//     case Dot::ACCEL_DIR::RIGHT:
//       m_IsAccelRight = false;
//       break;

//     default:
//       break;
//   }
// }


// /**
//  * @brief // Show the dot relative to the camera
//  *
//  * @param camX Camera's x position
//  * @param camY Camera's y position
//  **/
// void Dot::render( int camX, int camY )
// {
//   // g_DotTexture.render( m_PosX - camX, m_PosY - camY );
// }


// int Dot::getPosX(void) const
// {
//   return m_PosX;
// }


// int Dot::getPosY(void) const
// {
//   return m_PosY;
// }


// double Dot::getVelX_Debug(void) const
// {
//   return m_VelX;
// }


// double Dot::getVelY_Debug(void) const
// {
//   return m_VelY;
// }


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
    g_Window = SDL_CreateWindow( "Pallina",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  WINDOW_W,
                                  WINDOW_H,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );

    if( g_Window == NULL )
    {
      printf( "\nWindow could not be created! SDL Error: \"%s\"\n", SDL_GetError() );
      success = false;
    }
    else
    {
      printf( "\nWindow created" );

      // Create accelerated and vsynced renderer for window
      g_Renderer = SDL_CreateRenderer( g_Window, INIT_F1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

      if( g_Renderer == NULL )
      {
        printf( "\nRenderer could not be created! SDL Error: %s", SDL_GetError() );
        success = false;
      }
      else
      {
        printf( "\nRenderer created" );

        // Initialize renderer color
        SDL_SetRenderDrawColor( g_Renderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );

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
  g_Font = TTF_OpenFont( FontPath.c_str(), 28 );

  if( g_Font == NULL )
  {
    printf( "\nFailed to load Rachel Brown font! SDL_ttf Error: \"%s\"\n", TTF_GetError() );
    success = false;
  }
  else
  {
    printf( "\nOK: Rachel Brown font loaded" );
  }

  // Load sprite sheet
  if ( !g_SpriteSheet.loadFromFile( SpriteSheetPath.c_str() ) )
  {
    printf( "\nFailed to load sprite sheet!" );
    success = false;
  }
  else
  {
    printf( "\nOK: sprite sheet loaded" );
  }

  // Load background texture
  // if ( !g_BGTexture.loadFromFile( BGPath.c_str() ) )
  // {
  //   printf( "\nFailed to load background texture!\n" );
  //   success = false;
  // }
  // else
  // {
  //   printf( "\nBackground texture loaded" );
  // }

  return success;
}


static void close(void)
{
  // Free loaded images
  // g_DotTexture.free();
  // g_BGTexture.free();
//
  // // Destroy window
  // SDL_DestroyRenderer( g_Renderer );
  // SDL_DestroyWindow( g_Window );
  g_Renderer = NULL;
  g_Window   = NULL;

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
      // Dot ScreenDot;

      // The camera area
      // SDL_Rect camera = { 0, 0, WINDOW_W, WINDOW_H };

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
          // ScreenDot.handleEvent( e );
        }

        // Move the dot
        // ScreenDot.ProcessMovement();

        // Center the camera over the dot
        // camera.x = ( ScreenDot.getPosX() + Dot::DOT_W / 2 ) - WINDOW_W / 2;
        // camera.y = ( ScreenDot.getPosY() + Dot::DOT_H / 2 ) - WINDOW_H / 2;

        // Keep the camera in bounds
        // if( camera.x < 0 )
        // {
        //   camera.x = 0;
        // }
        // else { /* Camera's position is OK */ }

        // if( camera.y < 0 )
        // {
        //   camera.y = 0;
        // }
        // else { /* Camera's position is OK */ }

        // if( camera.x > LEVEL_W - camera.w )
        // {
        //   camera.x = LEVEL_W - camera.w;
        // }
        // else { /* Camera's position is OK */ }

        // if( camera.y > LEVEL_H - camera.h )
        // {
        //   camera.y = LEVEL_H - camera.h;
        // }
        // else { /* Camera's position is OK */ }


        // Clear screen
        SDL_SetRenderDrawColor( g_Renderer, WHITE_R, WHITE_G, WHITE_B, WHITE_A );
        SDL_RenderClear( g_Renderer );

        // Render background
        SDL_SetRenderDrawColor( g_Renderer, GREY_R, GREY_G, GREY_B, GREY_A );
        SDL_RenderFillRect( g_Renderer, NULL );

        // Render background
        SDL_SetRenderDrawColor( g_Renderer, BLACK_R, BLACK_G, BLACK_B, BLACK_A );
        SDL_RenderFillRect( g_Renderer, &SCREEN );

        // Render objects
        g_SpriteSheet.render( 50, 200, &BUTTON_1 );
        // ScreenDot.render( camera.x, camera.y );

        /************************************
        * Stampa info di debugging on-screen
        *************************************/

        // std::stringstream StringStream_xPos;
        // std::stringstream StringStream_yPos;
        // std::stringstream StringStream_xVel;
        // std::stringstream StringStream_yVel;

        // StringStream_xPos << "x pos: " << std::fixed << std::setprecision(Dot::NUM_OF_DIGITS) << ScreenDot.getPosX();
        // StringStream_yPos << "y pos: " << std::fixed << std::setprecision(Dot::NUM_OF_DIGITS) << ScreenDot.getPosY();
        // StringStream_xVel << "x vel: " << std::fixed << std::setprecision(Dot::NUM_OF_DIGITS) << ScreenDot.getVelX_Debug();
        // StringStream_yVel << "y vel: " << std::fixed << std::setprecision(Dot::NUM_OF_DIGITS) << ScreenDot.getVelY_Debug();

        // g_xPosText_Debug.loadFromRenderedText(StringStream_xPos.str(), g_TextColorBlack );
        // g_yPosText_Debug.loadFromRenderedText(StringStream_yPos.str(), g_TextColorBlack );
        // g_xVelText_Debug.loadFromRenderedText(StringStream_xVel.str(), g_TextColorBlack );
        // g_yVelText_Debug.loadFromRenderedText(StringStream_yVel.str(), g_TextColorBlack );

        // // Position of debugging text
        // int xPosText_xPos_Debug = ( WINDOW_W - g_xPosText_Debug.getWidth()  ) / 2;
        // int xPosText_yPos_Debug = ( WINDOW_H - g_xPosText_Debug.getHeight() ) / 2 + 0 * g_yPosText_Debug.getHeight();
        // int yPosText_xPos_Debug = ( WINDOW_W - g_yPosText_Debug.getWidth()  ) / 2;
        // int yPosText_yPos_Debug = ( WINDOW_H - g_yPosText_Debug.getHeight() ) / 2 + 1 * g_yPosText_Debug.getHeight();

        // int xVelText_xPos_Debug = ( WINDOW_W - g_xVelText_Debug.getWidth()  ) / 2;
        // int xVelText_yPos_Debug = ( WINDOW_H - g_xVelText_Debug.getHeight() ) / 2 + 2 * g_xVelText_Debug.getHeight();
        // int yVelText_xPos_Debug = ( WINDOW_W - g_yVelText_Debug.getWidth()  ) / 2;
        // int yVelText_yPos_Debug = ( WINDOW_H - g_yVelText_Debug.getHeight() ) / 2 + 3 * g_yVelText_Debug.getHeight();

        // g_xPosText_Debug.render( xPosText_xPos_Debug, xPosText_yPos_Debug );
        // g_yPosText_Debug.render( yPosText_xPos_Debug, yPosText_yPos_Debug );
        // g_xVelText_Debug.render( xVelText_xPos_Debug, xVelText_yPos_Debug );
        // g_yVelText_Debug.render( yVelText_xPos_Debug, yVelText_yPos_Debug );

        // Update screen
        SDL_RenderPresent( g_Renderer );

      } // Main loop

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