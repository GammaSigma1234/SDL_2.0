/***************************************************************************************************
* Includes
****************************************************************************************************/

#include <sstream>
#include <cstdio>
#include "MainWindow.hpp"
#include "Supervisor.hpp"
#include "Renderer.hpp"
#include "colours.hpp"


/***************************************************************************************************
* Private constants
****************************************************************************************************/

static std::stringstream Msg;

static const char* AllComponents_Path  ("./Sprites/AllComponents_800x750.png");

static constexpr int FIRST_ONE_AVAILABLE = -1;

/* Various sizes */

static constexpr int NormalButtonW_px(100);
static constexpr int NormalButtonH_px(100);

static constexpr int SolarCellW_px(100);
static constexpr int SolarCellH_px(50);

static constexpr int DisplayW_px(380);
static constexpr int DisplayH_px(130);

/* Positions of rows and columns in pixels in the sprite sheet */

// static constexpr int Row_0_px   (0);
// static constexpr int Row_1_px   (100);
// static constexpr int Row_2_px   (200);
// static constexpr int Row_3_px   (300);
// static constexpr int Row_4_px   (400);
// static constexpr int Row_5_px   (500);
// static constexpr int Column_0_px(0);
// static constexpr int Column_1_px(100);
// static constexpr int Column_2_px(200);
// static constexpr int Column_3_px(300);

static constexpr int Key0_xSS_px(0);
static constexpr int Key0_ySS_px(650);

static constexpr int KeyPoint_xSS_px(100);
static constexpr int KeyPoint_ySS_px(650);

static constexpr int KeyEquals_xSS_px(200);
static constexpr int KeyEquals_ySS_px(650);

static constexpr int Key1_xSS_px(0);
static constexpr int Key1_ySS_px(550);

static constexpr int Key2_xSS_px(100);
static constexpr int Key2_ySS_px(550);

static constexpr int Key3_xSS_px(200);
static constexpr int Key3_ySS_px(550);

/* Positions of display and photovoltaic cell in pixels in the sprite sheets */

static constexpr int SolarCell_xSS_px(0);
static constexpr int SolarCell_ySS_px(10);
static constexpr int Display_xSS_px(120);
static constexpr int Display_ySS_px(10);

/* Positions of elements in the Main Window (MW) */

static const SDL_Point Key0_MWPos     {0  , 600};
static const SDL_Point KeyPoint_MWPos {100, 600};
static const SDL_Point KeyEquals_MWPos{200, 600};
static const SDL_Point Key1_MWPos     {0  , 500};
static const SDL_Point Key2_MWPos     {100, 500};
static const SDL_Point Key3_MWPos     {200, 500};
static const SDL_Point Key4_MWPos     {0  , 400};
static const SDL_Point Key5_MWPos     {100, 400};
static const SDL_Point Key6_MWPos     {200, 400};
static const SDL_Point Key7_MWPos     {0  , 300};
static const SDL_Point Key8_MWPos     {100, 300};
static const SDL_Point Key9_MWPos     {200, 300};

static constexpr int SolarCell_xMW_px(10);
static constexpr int SolarCell_yMW_px(10);

static constexpr int Display_xMW_px(10);
static constexpr int Display_yMW_px(100);


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

  CreateGraphicElements_Pvt();
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
  m_Renderer = SDL_CreateRenderer( MainWindow::Get().GetSDLWindowPtr(), FIRST_ONE_AVAILABLE, SDL_RENDERER_ACCELERATED );

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
  m_SpriteSheet.loadFromFile(AllComponents_Path, m_Renderer);
}


void Renderer::CreateGraphicElements_Pvt(void)
{
  /* Buttons */

  size_t NumOfCreatedButtons(0);
  size_t NumOfExpectedButtons(static_cast<size_t>(ButtonsClips_Enum::HOW_MANY));

  m_Button_Vec.resize(static_cast<size_t>(ButtonsClips_Enum::HOW_MANY));

  size_t Key0(static_cast<size_t>(ButtonsClips_Enum::KEY_0));
  m_Button_Vec[Key0].setPosition(Key0_MWPos);
  m_Button_Vec[Key0].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key0].setClip(SDL_Rect{Key0_xSS_px, Key0_ySS_px, m_Button_Vec[Key0].GetWidth(), m_Button_Vec[Key0].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key0].setClip(SDL_Rect{400, 650, m_Button_Vec[Key0].GetWidth(), m_Button_Vec[Key0].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyPoint(static_cast<size_t>(ButtonsClips_Enum::KEY_POINT));
  m_Button_Vec[KeyPoint].setPosition(KeyPoint_MWPos);
  m_Button_Vec[KeyPoint].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyPoint].setClip(SDL_Rect{KeyPoint_xSS_px, KeyPoint_ySS_px, m_Button_Vec[KeyPoint].GetWidth(), m_Button_Vec[KeyPoint].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyPoint].setClip(SDL_Rect{500, 650, m_Button_Vec[KeyPoint].GetWidth(), m_Button_Vec[KeyPoint].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyEquals(static_cast<size_t>(ButtonsClips_Enum::KEY_EQUALS));
  m_Button_Vec[KeyEquals].setPosition(KeyEquals_MWPos);
  m_Button_Vec[KeyEquals].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyEquals].setClip(SDL_Rect{KeyEquals_xSS_px, KeyEquals_ySS_px, m_Button_Vec[KeyEquals].GetWidth(), m_Button_Vec[KeyEquals].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyEquals].setClip(SDL_Rect{600, 650, m_Button_Vec[KeyEquals].GetWidth(), m_Button_Vec[KeyEquals].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key1(static_cast<size_t>(ButtonsClips_Enum::KEY_1));
  m_Button_Vec[Key1].setPosition(Key1_MWPos);
  m_Button_Vec[Key1].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key1].setClip(SDL_Rect{Key1_xSS_px, Key1_ySS_px, m_Button_Vec[Key1].GetWidth(), m_Button_Vec[Key1].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key1].setClip(SDL_Rect{400, 550, m_Button_Vec[Key1].GetWidth(), m_Button_Vec[Key1].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key2(static_cast<size_t>(ButtonsClips_Enum::KEY_2));
  m_Button_Vec[Key2].setPosition(Key2_MWPos);
  m_Button_Vec[Key2].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key2].setClip(SDL_Rect{Key2_xSS_px, Key2_ySS_px, m_Button_Vec[Key2].GetWidth(), m_Button_Vec[Key2].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key2].setClip(SDL_Rect{500, 550, m_Button_Vec[Key2].GetWidth(), m_Button_Vec[Key2].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key3(static_cast<size_t>(ButtonsClips_Enum::KEY_3));
  m_Button_Vec[Key3].setPosition(Key3_MWPos);
  m_Button_Vec[Key3].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key3].setClip(SDL_Rect{Key3_xSS_px, Key3_ySS_px, m_Button_Vec[Key3].GetWidth(), m_Button_Vec[Key3].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key3].setClip(SDL_Rect{600, 550, m_Button_Vec[Key3].GetWidth(), m_Button_Vec[Key3].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
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

  /* Other components */

  size_t NumOfCreatedComponents(0);
  size_t NumOfExpectedComponents(static_cast<size_t>(ComponentsClips_Enum::HOW_MANY));

  m_SolarCell.setPosition(SDL_Point{SolarCell_xMW_px, SolarCell_yMW_px});
  m_SolarCell.setSize(SolarCellW_px, SolarCellH_px);
  m_SolarCell.setClip(SDL_Rect{SolarCell_xSS_px, SolarCell_ySS_px, m_SolarCell.GetWidth(), m_SolarCell.GetHeight()});
  ++NumOfCreatedComponents;

  m_Display.setPosition(SDL_Point{Display_xMW_px, Display_yMW_px});
  m_Display.setSize(DisplayW_px, DisplayH_px);
  m_Display.setClip(SDL_Rect{Display_xSS_px, Display_ySS_px, m_Display.GetWidth(), m_Display.GetHeight()});
  ++NumOfCreatedComponents;

  if ( NumOfCreatedComponents == NumOfExpectedComponents)
  {
    Msg.str(std::string());
    Msg << "Number of created components OK.";
    Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::NO_FAULT);
  }
  else
  {
    Msg.str(std::string());
    Msg << "Number of created components: " << NumOfCreatedComponents << ". Expected: " << NumOfExpectedComponents;
    Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::WARNING);
  }
}


Texture& Renderer::GetSpriteSheet( void )
{
  return m_SpriteSheet;
}


/**
 * @brief Performs all the rendering.
 **/
void Renderer::Render(void)
{
  SDL_SetRenderDrawColor( m_Renderer, WHITE_R, WHITE_G, WHITE_B, ALPHA_MAX );

  SDL_RenderClear( m_Renderer );

  for (auto& Button : m_Button_Vec)
  {
    Button.render();
  }

  m_SolarCell.render();
  m_Display.render();

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


std::vector<Button>& Renderer::GetButtonVector ( void )
{
  return m_Button_Vec;
}