/***************************************************************************************************
* Includes
****************************************************************************************************/

#include <sstream>
#include <cstdio>
#include "MainWindow.hpp"
#include "Supervisor.hpp"
#include "Renderer.hpp"
#include "..\..\Colours_Lib\colours.hpp"


/***************************************************************************************************
* Private constants
****************************************************************************************************/

static std::stringstream Msg;

static const char* NormalKeys_Path("./Sprites/Keys_Normal_400x600.png");
static const char* PressedKeys_Path("./Sprites/Keys_Pressed_400x600.png");

/* The following variables are used for the geometry of the buttons */

/* Positions of rows and columns of the sprite sheets in pixels */

static constexpr int Row_0_px   (0);
static constexpr int Row_1_px   (100);
static constexpr int Row_2_px   (200);
static constexpr int Row_3_px   (300);
static constexpr int Row_4_px   (400);
static constexpr int Row_5_px   (500);
static constexpr int Column_0_px(0);
static constexpr int Column_1_px(100);
static constexpr int Column_2_px(200);
static constexpr int Column_3_px(300);

/* Positions of the buttons */

static const SDL_Point Key0_Pos     {0  , 600};
static const SDL_Point KeyPoint_Pos {100, 600};
static const SDL_Point KeyEquals_Pos{200, 600};
static const SDL_Point Key1_Pos     {0  , 500};
static const SDL_Point Key2_Pos     {100, 500};
static const SDL_Point Key3_Pos     {200, 500};
static const SDL_Point Key4_Pos     {0  , 400};
static const SDL_Point Key5_Pos     {100, 400};
static const SDL_Point Key6_Pos     {200, 400};
static const SDL_Point Key7_Pos     {0  , 300};
static const SDL_Point Key8_Pos     {100, 300};
static const SDL_Point Key9_Pos     {200, 300};

static constexpr int ButtonWidth_px ( Button::BUTTON_W_px );
static constexpr int ButtonHeight_px( Button::BUTTON_H_px);

/***************************************************************************************************
* Methods
****************************************************************************************************/

/**
 * @brief Constructor
 **/
Renderer::Renderer( void )
{
  printf( "\nInitialising Renderer...\n");

  CreateRenderer_Pvt();

  LoadMedia_Pvt();

  CreateShapes_Pvt();
}


/**
 * @brief Destructor
 **/
Renderer::~Renderer(void)
{
  printf( "Renderer's destructor called\n" );
}


void Renderer::CreateRenderer_Pvt(void)
{
  m_Renderer = SDL_CreateRenderer( MainWindow::Get().GetSDLWindowPtr(), FIRST_ONE, SDL_RENDERER_ACCELERATED );

  if( m_Renderer == NULL )
  {
    Msg.str(std::string());
    Msg << "Renderer could not be created! SDL Error: \"" << SDL_GetError()
        << "\nMainWindow::Get() = " << static_cast<void*>( MainWindow::Get().GetSDLWindowPtr());
    Supervisor::Get().RaiseFault();
    Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::BLOCKING);
  }
  else
  {
    Msg.str(std::string());
    Msg << "Renderer created. Address: " << static_cast<void*>( m_Renderer );
    Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::NO_FAULT);
  }
}


/**
 * @brief Loads all media needed for this project.
 **/
void Renderer::LoadMedia_Pvt( void )
{
  size_t SpriteSheetsNum(static_cast<size_t>(SpriteSheets_Enum::HOW_MANY));

  m_SpriteSheets_Paths.push_back(NormalKeys_Path);
  m_SpriteSheets_Paths.push_back(PressedKeys_Path);

  if ( m_SpriteSheets_Paths.size() == SpriteSheetsNum )
  {
    Msg.str(std::string());
    Msg << "Sprite sheets enumeration successful.";
    Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::NO_FAULT);
  }
  else
  {
    Msg.str(std::string());
    Msg << "*** WARNING ***\n\tNumber of specified sprite sheets: " << m_SpriteSheets_Paths.size() << ". Expected: " << SpriteSheetsNum;
    Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::BLOCKING);
  }

  m_SpriteSheets_Vec.resize(SpriteSheetsNum);

  for (size_t i = 0; i != SpriteSheetsNum; ++i)
  {
    m_SpriteSheets_Vec[i].loadFromFile( m_SpriteSheets_Paths[i], m_Renderer );

    if( m_SpriteSheets_Vec[i].isValid() )
    {
      Msg.str(std::string());
      Msg << "Texture creation successful.";
      Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::NO_FAULT);
    }
    else
    {
      Msg.str(std::string());
      Msg << "Failed to load \"" << m_SpriteSheets_Paths[i].c_str() << "\"!",
      Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::BLOCKING);

      m_WasInitSuccessful = false;
    }
  }
}


void Renderer::CreateShapes_Pvt(void)
{
  size_t NumOfCreatedButtons(0);
  size_t NumOfExpectedButtons(static_cast<size_t>(ButtonsClips_Enum::HOW_MANY));

  m_Buttons_Vec.resize(NumOfExpectedButtons);

  size_t Key_0(static_cast<size_t>(ButtonsClips_Enum::KEY_0));
  m_Buttons_Vec[Key_0].setPosition(Key0_Pos);
  m_Buttons_Vec[Key_0].setClip(SDL_Rect{Column_0_px, Row_5_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_Point(static_cast<size_t>(ButtonsClips_Enum::KEY_POINT));
  m_Buttons_Vec[Key_Point].setPosition(KeyPoint_Pos);
  m_Buttons_Vec[Key_Point].setClip(SDL_Rect{Column_1_px, Row_5_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_Equals(static_cast<size_t>(ButtonsClips_Enum::KEY_EQUALS));
  m_Buttons_Vec[Key_Equals].setPosition(KeyEquals_Pos);
  m_Buttons_Vec[Key_Equals].setClip(SDL_Rect{Column_2_px, Row_5_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_1(static_cast<size_t>(ButtonsClips_Enum::KEY_1));
  m_Buttons_Vec[Key_1].setPosition(Key1_Pos);
  m_Buttons_Vec[Key_1].setClip(SDL_Rect{Column_0_px, Row_4_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_2(static_cast<size_t>(ButtonsClips_Enum::KEY_2));
  m_Buttons_Vec[Key_2].setPosition(Key2_Pos);
  m_Buttons_Vec[Key_2].setClip(SDL_Rect{Column_1_px, Row_4_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_3(static_cast<size_t>(ButtonsClips_Enum::KEY_3));
  m_Buttons_Vec[Key_3].setPosition(Key3_Pos);
  m_Buttons_Vec[Key_3].setClip(SDL_Rect{Column_2_px, Row_4_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_4(static_cast<size_t>(ButtonsClips_Enum::KEY_4));
  m_Buttons_Vec[Key_4].setPosition(Key4_Pos);
  m_Buttons_Vec[Key_4].setClip(SDL_Rect{Column_0_px, Row_3_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_5(static_cast<size_t>(ButtonsClips_Enum::KEY_5));
  m_Buttons_Vec[Key_5].setPosition(Key5_Pos);
  m_Buttons_Vec[Key_5].setClip(SDL_Rect{Column_1_px, Row_3_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_6(static_cast<size_t>(ButtonsClips_Enum::KEY_6));
  m_Buttons_Vec[Key_6].setPosition(Key6_Pos);
  m_Buttons_Vec[Key_6].setClip(SDL_Rect{Column_2_px, Row_3_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_7(static_cast<size_t>(ButtonsClips_Enum::KEY_7));
  m_Buttons_Vec[Key_7].setPosition(Key7_Pos);
  m_Buttons_Vec[Key_7].setClip(SDL_Rect{Column_0_px, Row_2_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_8(static_cast<size_t>(ButtonsClips_Enum::KEY_8));
  m_Buttons_Vec[Key_8].setPosition(Key8_Pos);
  m_Buttons_Vec[Key_8].setClip(SDL_Rect{Column_1_px, Row_2_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  size_t Key_9(static_cast<size_t>(ButtonsClips_Enum::KEY_9));
  m_Buttons_Vec[Key_9].setPosition(Key9_Pos);
  m_Buttons_Vec[Key_9].setClip(SDL_Rect{Column_2_px, Row_2_px, ButtonWidth_px, ButtonHeight_px});
  ++NumOfCreatedButtons;

  if ( NumOfCreatedButtons == NumOfExpectedButtons)
  {
    Msg.str(std::string());
    Msg << "Number of created buttons OK.";
    Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::NO_FAULT);
  }
  else
  {
    Msg.str(std::string());
    Msg << "Number of created buttons: " << NumOfCreatedButtons << ". Expected: " << NumOfExpectedButtons;
    Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::WARNING);
  }
}

Texture& Renderer::GetSpriteSheet( SpriteSheets_Enum ss )
{
  size_t SpriteSheets_Index;

  switch ( ss )
  {
    default:
    case SpriteSheets_Enum::NORMAL_BUTTONS:
      SpriteSheets_Index = static_cast<size_t>(SpriteSheets_Enum::NORMAL_BUTTONS);
      return m_SpriteSheets_Vec[SpriteSheets_Index];
      break;

    case SpriteSheets_Enum::PRESSED_BUTTONS:
      SpriteSheets_Index = static_cast<size_t>(SpriteSheets_Enum::PRESSED_BUTTONS);
      return m_SpriteSheets_Vec[SpriteSheets_Index];
      break;
  }
}


std::vector<Button>& Renderer::GetButtonVector( void )
{
  return m_Buttons_Vec;
}


/**
 * @brief Performs all the rendering.
 **/
void Renderer::Render(void)
{
  SDL_SetRenderDrawColor( m_Renderer, WHITE_R, WHITE_G, WHITE_B, ALPHA_MAX );

  SDL_RenderClear( m_Renderer );

  for (auto& Button : m_Buttons_Vec)
  {
    Button.render();
  }

  SDL_RenderPresent( m_Renderer ); // Update screen
}


/**
 * @brief Use this function to invoke the Renderer when needed.
 *
 * @return Renderer&
 **/
Renderer& Renderer::Get(void)
{
  static Renderer Instance; // Creates instance of singleton in static memory.
  return Instance;
}


SDL_Renderer* Renderer::GetSDLRendererPtr(void)
{
  return m_Renderer;
}