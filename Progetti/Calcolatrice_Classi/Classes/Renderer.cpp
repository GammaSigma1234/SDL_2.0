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

static constexpr int ScaleFactor = 1; // Used if the sprite sheet is upscaled

/* Various sizes */

static constexpr int NormalButtonW_px(100 / ScaleFactor);
static constexpr int NormalButtonH_px( 80 / ScaleFactor);

static constexpr int KeyPlusW_px(100 / ScaleFactor);
static constexpr int KeyPlusH_px(180 / ScaleFactor);

static constexpr int SolarCellW_px(100 / ScaleFactor);
static constexpr int SolarCellH_px( 50 / ScaleFactor);

static constexpr int DisplayW_px(425 / ScaleFactor);
static constexpr int DisplayH_px(125 / ScaleFactor);

/* Positions of rows and columns in pixels in the sprite sheet */

static constexpr int RowHeight_SS_px(100);
static constexpr int Row0_SS_px     (150);
static constexpr int Row1_SS_px     (Row0_SS_px + RowHeight_SS_px);
static constexpr int Row2_SS_px     (Row1_SS_px + RowHeight_SS_px);
static constexpr int Row3_SS_px     (Row2_SS_px + RowHeight_SS_px);
static constexpr int Row4_SS_px     (Row3_SS_px + RowHeight_SS_px);
static constexpr int Row5_SS_px     (Row4_SS_px + RowHeight_SS_px);

static constexpr int ColWidth_SS_px (100);
static constexpr int OffsetCol_SS_px(400); // Offset to get to the pressed buttons in the sprite sheet
static constexpr int Col0_SS_px     (0);
static constexpr int Col1_SS_px     (Col0_SS_px + ColWidth_SS_px);
static constexpr int Col2_SS_px     (Col1_SS_px + ColWidth_SS_px);
static constexpr int Col3_SS_px     (Col2_SS_px + ColWidth_SS_px);

static constexpr int Key0_Normal_xSS_px(Col0_SS_px);
static constexpr int Key0_Normal_ySS_px(Row5_SS_px);
static constexpr int Key0_Pressed_xSS_px(Col0_SS_px + OffsetCol_SS_px);
static constexpr int Key0_Pressed_ySS_px(Row5_SS_px);

static constexpr int KeyPoint_Normal_xSS_px(Col1_SS_px);
static constexpr int KeyPoint_Normal_ySS_px(Row5_SS_px);
static constexpr int KeyPoint_Pressed_xSS_px(Col1_SS_px + OffsetCol_SS_px);
static constexpr int KeyPoint_Pressed_ySS_px(Row5_SS_px);

static constexpr int KeyEquals_Normal_xSS_px(Col2_SS_px);
static constexpr int KeyEquals_Normal_ySS_px(Row5_SS_px);
static constexpr int KeyEquals_Pressed_xSS_px(Col2_SS_px + OffsetCol_SS_px);
static constexpr int KeyEquals_Pressed_ySS_px(Row5_SS_px);

static constexpr int Key1_Normal_xSS_px(Col0_SS_px);
static constexpr int Key1_Normal_ySS_px(Row4_SS_px);
static constexpr int Key1_Pressed_xSS_px(Col0_SS_px + OffsetCol_SS_px);
static constexpr int Key1_Pressed_ySS_px(Row4_SS_px);

static constexpr int Key2_Normal_xSS_px(Col1_SS_px);
static constexpr int Key2_Normal_ySS_px(Row4_SS_px);
static constexpr int Key2_Pressed_xSS_px(Col1_SS_px + OffsetCol_SS_px);
static constexpr int Key2_Pressed_ySS_px(Row4_SS_px);

static constexpr int Key3_Normal_xSS_px(Col2_SS_px);
static constexpr int Key3_Normal_ySS_px(Row4_SS_px);
static constexpr int Key3_Pressed_xSS_px(Col2_SS_px + OffsetCol_SS_px);
static constexpr int Key3_Pressed_ySS_px(Row4_SS_px);

static constexpr int Key4_Normal_xSS_px(Col0_SS_px);
static constexpr int Key4_Normal_ySS_px(Row3_SS_px);
static constexpr int Key4_Pressed_xSS_px(Col0_SS_px + OffsetCol_SS_px);
static constexpr int Key4_Pressed_ySS_px(Row3_SS_px);

static constexpr int Key5_Normal_xSS_px(Col1_SS_px);
static constexpr int Key5_Normal_ySS_px(Row3_SS_px);
static constexpr int Key5_Pressed_xSS_px(Col1_SS_px + OffsetCol_SS_px);
static constexpr int Key5_Pressed_ySS_px(Row3_SS_px);

static constexpr int Key6_Normal_xSS_px(Col2_SS_px);
static constexpr int Key6_Normal_ySS_px(Row3_SS_px);
static constexpr int Key6_Pressed_xSS_px(Col2_SS_px + OffsetCol_SS_px);
static constexpr int Key6_Pressed_ySS_px(Row3_SS_px);

static constexpr int Key7_Normal_xSS_px(Col0_SS_px);
static constexpr int Key7_Normal_ySS_px(Row2_SS_px);
static constexpr int Key7_Pressed_xSS_px(Col0_SS_px + OffsetCol_SS_px);
static constexpr int Key7_Pressed_ySS_px(Row2_SS_px);

static constexpr int Key8_Normal_xSS_px(Col1_SS_px);
static constexpr int Key8_Normal_ySS_px(Row2_SS_px);
static constexpr int Key8_Pressed_xSS_px(Col1_SS_px + OffsetCol_SS_px);
static constexpr int Key8_Pressed_ySS_px(Row2_SS_px);

static constexpr int Key9_Normal_xSS_px(Col2_SS_px);
static constexpr int Key9_Normal_ySS_px(Row2_SS_px);
static constexpr int Key9_Pressed_xSS_px(Col2_SS_px + OffsetCol_SS_px);
static constexpr int Key9_Pressed_ySS_px(Row2_SS_px);

static constexpr int KeyMRC_Normal_xSS_px(Col0_SS_px);
static constexpr int KeyMRC_Normal_ySS_px(Row1_SS_px);
static constexpr int KeyMRC_Pressed_xSS_px(Col0_SS_px + OffsetCol_SS_px);
static constexpr int KeyMRC_Pressed_ySS_px(Row1_SS_px);

static constexpr int KeyMemMinus_Normal_xSS_px(Col1_SS_px);
static constexpr int KeyMemMinus_Normal_ySS_px(Row1_SS_px);
static constexpr int KeyMemMinus_Pressed_xSS_px(Col1_SS_px + OffsetCol_SS_px);
static constexpr int KeyMemMinus_Pressed_ySS_px(Row1_SS_px);

static constexpr int KeyMemPlus_Normal_xSS_px(Col2_SS_px);
static constexpr int KeyMemPlus_Normal_ySS_px(Row1_SS_px);
static constexpr int KeyMemPlus_Pressed_xSS_px(Col2_SS_px + OffsetCol_SS_px);
static constexpr int KeyMemPlus_Pressed_ySS_px(Row1_SS_px);

static constexpr int KeyPlus_Normal_xSS_px(Col3_SS_px);
static constexpr int KeyPlus_Normal_ySS_px(Row4_SS_px);
static constexpr int KeyPlus_Pressed_xSS_px(Col3_SS_px + OffsetCol_SS_px);
static constexpr int KeyPlus_Pressed_ySS_px(Row4_SS_px);

static constexpr int KeyMinus_Normal_xSS_px(Col3_SS_px);
static constexpr int KeyMinus_Normal_ySS_px(Row3_SS_px);
static constexpr int KeyMinus_Pressed_xSS_px(Col3_SS_px + OffsetCol_SS_px);
static constexpr int KeyMinus_Pressed_ySS_px(Row3_SS_px);

static constexpr int KeyMult_Normal_xSS_px(Col3_SS_px);
static constexpr int KeyMult_Normal_ySS_px(Row2_SS_px);
static constexpr int KeyMult_Pressed_xSS_px(Col3_SS_px + OffsetCol_SS_px);
static constexpr int KeyMult_Pressed_ySS_px(Row2_SS_px);

static constexpr int KeyDiv_Normal_xSS_px(Col3_SS_px);
static constexpr int KeyDiv_Normal_ySS_px(Row1_SS_px);
static constexpr int KeyDiv_Pressed_xSS_px(Col3_SS_px + OffsetCol_SS_px);
static constexpr int KeyDiv_Pressed_ySS_px(Row1_SS_px);

static constexpr int KeySign_Normal_xSS_px(Col3_SS_px);
static constexpr int KeySign_Normal_ySS_px(Row0_SS_px);
static constexpr int KeySign_Pressed_xSS_px(Col3_SS_px + OffsetCol_SS_px);
static constexpr int KeySign_Pressed_ySS_px(Row0_SS_px);

static constexpr int KeyPower_Normal_xSS_px(Col0_SS_px);
static constexpr int KeyPower_Normal_ySS_px(Row0_SS_px);
static constexpr int KeyPower_Pressed_xSS_px(Col0_SS_px + OffsetCol_SS_px);
static constexpr int KeyPower_Pressed_ySS_px(Row0_SS_px);

static constexpr int KeyDel_Normal_xSS_px(Col1_SS_px);
static constexpr int KeyDel_Normal_ySS_px(Row0_SS_px);
static constexpr int KeyDel_Pressed_xSS_px(Col1_SS_px + OffsetCol_SS_px);
static constexpr int KeyDel_Pressed_ySS_px(Row0_SS_px);

static constexpr int KeyPct_Normal_xSS_px(Col2_SS_px);
static constexpr int KeyPct_Normal_ySS_px(Row0_SS_px);
static constexpr int KeyPct_Pressed_xSS_px(Col2_SS_px + OffsetCol_SS_px);
static constexpr int KeyPct_Pressed_ySS_px(Row0_SS_px);

/* Positions of display and photovoltaic cell in pixels in the sprite sheets */

static constexpr int SolarCell_xSS_px(0 / ScaleFactor);
static constexpr int SolarCell_ySS_px(0 / ScaleFactor);

static constexpr int Display_xSS_px(110 / ScaleFactor);
static constexpr int Display_ySS_px(0   / ScaleFactor);

/* Positions of elements in the Main Window (MW) */

static constexpr int RowHeight_MW_px(NormalButtonH_px + 20);
static constexpr int Row0_MW_px     (200);
static constexpr int Row1_MW_px     (Row0_MW_px + RowHeight_MW_px);
static constexpr int Row2_MW_px     (Row1_MW_px + RowHeight_MW_px);
static constexpr int Row3_MW_px     (Row2_MW_px + RowHeight_MW_px);
static constexpr int Row4_MW_px     (Row3_MW_px + RowHeight_MW_px);
static constexpr int Row5_MW_px     (Row4_MW_px + RowHeight_MW_px);

static constexpr int ColWidth_MW_px(NormalButtonW_px + 10);
static constexpr int Col0_MW_px    (0);
static constexpr int Col1_MW_px    (Col0_MW_px + ColWidth_MW_px);
static constexpr int Col2_MW_px    (Col1_MW_px + ColWidth_MW_px);
static constexpr int Col3_MW_px    (Col2_MW_px + ColWidth_MW_px);

static const SDL_Point Key0_MWPos       {Col0_MW_px, Row5_MW_px};
static const SDL_Point KeyPoint_MWPos   {Col1_MW_px, Row5_MW_px};
static const SDL_Point KeyEquals_MWPos  {Col2_MW_px, Row5_MW_px};
static const SDL_Point Key1_MWPos       {Col0_MW_px, Row4_MW_px};
static const SDL_Point Key2_MWPos       {Col1_MW_px, Row4_MW_px};
static const SDL_Point Key3_MWPos       {Col2_MW_px, Row4_MW_px};
static const SDL_Point Key4_MWPos       {Col0_MW_px, Row3_MW_px};
static const SDL_Point Key5_MWPos       {Col1_MW_px, Row3_MW_px};
static const SDL_Point Key6_MWPos       {Col2_MW_px, Row3_MW_px};
static const SDL_Point Key7_MWPos       {Col0_MW_px, Row2_MW_px};
static const SDL_Point Key8_MWPos       {Col1_MW_px, Row2_MW_px};
static const SDL_Point Key9_MWPos       {Col2_MW_px, Row2_MW_px};
static const SDL_Point KeyMRC_MWPos     {Col0_MW_px, Row1_MW_px};
static const SDL_Point KeyMemMinus_MWPos{Col1_MW_px, Row1_MW_px};
static const SDL_Point KeyMemPlus_MWPos {Col2_MW_px, Row1_MW_px};
static const SDL_Point KeyPlus_MWPos    {Col3_MW_px, Row4_MW_px};
static const SDL_Point KeyMinus_MWPos   {Col3_MW_px, Row3_MW_px};
static const SDL_Point KeyMult_MWPos    {Col3_MW_px, Row2_MW_px};
static const SDL_Point KeyDiv_MWPos     {Col3_MW_px, Row1_MW_px};
static const SDL_Point KeySign_MWPos    {Col3_MW_px, Row0_MW_px};
static const SDL_Point KeyPower_MWPos   {Col0_MW_px, Row0_MW_px};
static const SDL_Point KeyDel_MWPos     {Col1_MW_px, Row0_MW_px};
static const SDL_Point KeyPct_MWPos     {Col2_MW_px, Row0_MW_px};

static constexpr int SolarCell_xMW_px(5);
static constexpr int SolarCell_yMW_px(5);

static constexpr int Display_xMW_px(5);
static constexpr int Display_yMW_px(SolarCell_yMW_px + SolarCellH_px + 5);


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
    Supervisor::Get().PrintMessage(Msg.str());
  }
}


/**
 * @brief Loads all media needed for this project.
 **/
void Renderer::LoadMedia_Pvt( void )
{
  if ( m_SpriteSheet.loadFromFile(AllComponents_Path, m_Renderer) )
  {
    Msg.str(std::string());
    Msg << "Sprite sheet properly created.";
    Supervisor::Get().PrintMessage(Msg.str());
  }
  else
  {
    Msg.str(std::string());
    Msg << "Sprite sheet could not be loaded!";
    Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::BLOCKING);
  }
}


void Renderer::CreateGraphicElements_Pvt(void)
{
  /* Create Buttons */

  size_t NumOfCreatedButtons(0);
  size_t NumOfExpectedButtons(static_cast<size_t>(ButtonsClips_Enum::HOW_MANY));

  m_Button_Vec.resize(static_cast<size_t>(ButtonsClips_Enum::HOW_MANY));

  size_t Key0(static_cast<size_t>(ButtonsClips_Enum::KEY_0));
  m_Button_Vec[Key0].setPosition(Key0_MWPos);
  m_Button_Vec[Key0].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key0].setClip(SDL_Rect{Key0_Normal_xSS_px , Key0_Normal_ySS_px , m_Button_Vec[Key0].GetWidth(), m_Button_Vec[Key0].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key0].setClip(SDL_Rect{Key0_Pressed_xSS_px, Key0_Pressed_ySS_px, m_Button_Vec[Key0].GetWidth(), m_Button_Vec[Key0].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyPoint(static_cast<size_t>(ButtonsClips_Enum::KEY_POINT));
  m_Button_Vec[KeyPoint].setPosition(KeyPoint_MWPos);
  m_Button_Vec[KeyPoint].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyPoint].setClip(SDL_Rect{KeyPoint_Normal_xSS_px , KeyPoint_Normal_ySS_px , m_Button_Vec[KeyPoint].GetWidth(), m_Button_Vec[KeyPoint].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyPoint].setClip(SDL_Rect{KeyPoint_Pressed_xSS_px, KeyPoint_Pressed_ySS_px, m_Button_Vec[KeyPoint].GetWidth(), m_Button_Vec[KeyPoint].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyEquals(static_cast<size_t>(ButtonsClips_Enum::KEY_EQUALS));
  m_Button_Vec[KeyEquals].setPosition(KeyEquals_MWPos);
  m_Button_Vec[KeyEquals].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyEquals].setClip(SDL_Rect{KeyEquals_Normal_xSS_px , KeyEquals_Normal_ySS_px , m_Button_Vec[KeyEquals].GetWidth(), m_Button_Vec[KeyEquals].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyEquals].setClip(SDL_Rect{KeyEquals_Pressed_xSS_px, KeyEquals_Pressed_ySS_px, m_Button_Vec[KeyEquals].GetWidth(), m_Button_Vec[KeyEquals].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key1(static_cast<size_t>(ButtonsClips_Enum::KEY_1));
  m_Button_Vec[Key1].setPosition(Key1_MWPos);
  m_Button_Vec[Key1].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key1].setClip(SDL_Rect{Key1_Normal_xSS_px , Key1_Normal_ySS_px , m_Button_Vec[Key1].GetWidth(), m_Button_Vec[Key1].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key1].setClip(SDL_Rect{Key1_Pressed_xSS_px, Key1_Pressed_ySS_px, m_Button_Vec[Key1].GetWidth(), m_Button_Vec[Key1].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key2(static_cast<size_t>(ButtonsClips_Enum::KEY_2));
  m_Button_Vec[Key2].setPosition(Key2_MWPos);
  m_Button_Vec[Key2].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key2].setClip(SDL_Rect{Key2_Normal_xSS_px , Key2_Normal_ySS_px , m_Button_Vec[Key2].GetWidth(), m_Button_Vec[Key2].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key2].setClip(SDL_Rect{Key2_Pressed_xSS_px, Key2_Pressed_ySS_px, m_Button_Vec[Key2].GetWidth(), m_Button_Vec[Key2].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key3(static_cast<size_t>(ButtonsClips_Enum::KEY_3));
  m_Button_Vec[Key3].setPosition(Key3_MWPos);
  m_Button_Vec[Key3].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key3].setClip(SDL_Rect{Key3_Normal_xSS_px , Key3_Normal_ySS_px , m_Button_Vec[Key3].GetWidth(), m_Button_Vec[Key3].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key3].setClip(SDL_Rect{Key3_Pressed_xSS_px, Key3_Pressed_ySS_px, m_Button_Vec[Key3].GetWidth(), m_Button_Vec[Key3].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key4(static_cast<size_t>(ButtonsClips_Enum::KEY_4));
  m_Button_Vec[Key4].setPosition(Key4_MWPos);
  m_Button_Vec[Key4].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key4].setClip(SDL_Rect{Key4_Normal_xSS_px , Key4_Normal_ySS_px , m_Button_Vec[Key4].GetWidth(), m_Button_Vec[Key4].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key4].setClip(SDL_Rect{Key4_Pressed_xSS_px, Key4_Pressed_ySS_px, m_Button_Vec[Key4].GetWidth(), m_Button_Vec[Key4].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key5(static_cast<size_t>(ButtonsClips_Enum::KEY_5));
  m_Button_Vec[Key5].setPosition(Key5_MWPos);
  m_Button_Vec[Key5].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key5].setClip(SDL_Rect{Key5_Normal_xSS_px , Key5_Normal_ySS_px , m_Button_Vec[Key5].GetWidth(), m_Button_Vec[Key5].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key5].setClip(SDL_Rect{Key5_Pressed_xSS_px, Key5_Pressed_ySS_px, m_Button_Vec[Key5].GetWidth(), m_Button_Vec[Key5].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key6(static_cast<size_t>(ButtonsClips_Enum::KEY_6));
  m_Button_Vec[Key6].setPosition(Key6_MWPos);
  m_Button_Vec[Key6].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key6].setClip(SDL_Rect{Key6_Normal_xSS_px , Key6_Normal_ySS_px , m_Button_Vec[Key6].GetWidth(), m_Button_Vec[Key6].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key6].setClip(SDL_Rect{Key6_Pressed_xSS_px, Key6_Pressed_ySS_px, m_Button_Vec[Key6].GetWidth(), m_Button_Vec[Key6].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key7(static_cast<size_t>(ButtonsClips_Enum::KEY_7));
  m_Button_Vec[Key7].setPosition(Key7_MWPos);
  m_Button_Vec[Key7].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key7].setClip(SDL_Rect{Key7_Normal_xSS_px , Key7_Normal_ySS_px , m_Button_Vec[Key7].GetWidth(), m_Button_Vec[Key7].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key7].setClip(SDL_Rect{Key7_Pressed_xSS_px, Key7_Pressed_ySS_px, m_Button_Vec[Key7].GetWidth(), m_Button_Vec[Key7].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key8(static_cast<size_t>(ButtonsClips_Enum::KEY_8));
  m_Button_Vec[Key8].setPosition(Key8_MWPos);
  m_Button_Vec[Key8].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key8].setClip(SDL_Rect{Key8_Normal_xSS_px , Key8_Normal_ySS_px , m_Button_Vec[Key8].GetWidth(), m_Button_Vec[Key8].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key8].setClip(SDL_Rect{Key8_Pressed_xSS_px, Key8_Pressed_ySS_px, m_Button_Vec[Key8].GetWidth(), m_Button_Vec[Key8].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t Key9(static_cast<size_t>(ButtonsClips_Enum::KEY_9));
  m_Button_Vec[Key9].setPosition(Key9_MWPos);
  m_Button_Vec[Key9].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[Key9].setClip(SDL_Rect{Key9_Normal_xSS_px , Key9_Normal_ySS_px , m_Button_Vec[Key9].GetWidth(), m_Button_Vec[Key9].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[Key9].setClip(SDL_Rect{Key9_Pressed_xSS_px, Key9_Pressed_ySS_px, m_Button_Vec[Key9].GetWidth(), m_Button_Vec[Key9].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyMRC(static_cast<size_t>(ButtonsClips_Enum::KEY_MRC));
  m_Button_Vec[KeyMRC].setPosition(KeyMRC_MWPos);
  m_Button_Vec[KeyMRC].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyMRC].setClip(SDL_Rect{KeyMRC_Normal_xSS_px , KeyMRC_Normal_ySS_px , m_Button_Vec[KeyMRC].GetWidth(), m_Button_Vec[KeyMRC].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyMRC].setClip(SDL_Rect{KeyMRC_Pressed_xSS_px, KeyMRC_Pressed_ySS_px, m_Button_Vec[KeyMRC].GetWidth(), m_Button_Vec[KeyMRC].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyMemMinus(static_cast<size_t>(ButtonsClips_Enum::KEY_MEMMINUS));
  m_Button_Vec[KeyMemMinus].setPosition(KeyMemMinus_MWPos);
  m_Button_Vec[KeyMemMinus].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyMemMinus].setClip(SDL_Rect{KeyMemMinus_Normal_xSS_px , KeyMemMinus_Normal_ySS_px , m_Button_Vec[KeyMemMinus].GetWidth(), m_Button_Vec[KeyMemMinus].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyMemMinus].setClip(SDL_Rect{KeyMemMinus_Pressed_xSS_px, KeyMemMinus_Pressed_ySS_px, m_Button_Vec[KeyMemMinus].GetWidth(), m_Button_Vec[KeyMemMinus].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyMemPlus(static_cast<size_t>(ButtonsClips_Enum::KEY_MEMPLUS));
  m_Button_Vec[KeyMemPlus].setPosition(KeyMemPlus_MWPos);
  m_Button_Vec[KeyMemPlus].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyMemPlus].setClip(SDL_Rect{KeyMemPlus_Normal_xSS_px , KeyMemPlus_Normal_ySS_px , m_Button_Vec[KeyMemPlus].GetWidth(), m_Button_Vec[KeyMemPlus].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyMemPlus].setClip(SDL_Rect{KeyMemPlus_Pressed_xSS_px, KeyMemPlus_Pressed_ySS_px, m_Button_Vec[KeyMemPlus].GetWidth(), m_Button_Vec[KeyMemPlus].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyPlus(static_cast<size_t>(ButtonsClips_Enum::KEY_PLUS));
  m_Button_Vec[KeyPlus].setPosition(KeyPlus_MWPos);
  m_Button_Vec[KeyPlus].setSize(KeyPlusW_px, KeyPlusH_px);
  m_Button_Vec[KeyPlus].setClip(SDL_Rect{KeyPlus_Normal_xSS_px , KeyPlus_Normal_ySS_px , m_Button_Vec[KeyPlus].GetWidth(), m_Button_Vec[KeyPlus].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyPlus].setClip(SDL_Rect{KeyPlus_Pressed_xSS_px, KeyPlus_Pressed_ySS_px, m_Button_Vec[KeyPlus].GetWidth(), m_Button_Vec[KeyPlus].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyMinus(static_cast<size_t>(ButtonsClips_Enum::KEY_MINUS));
  m_Button_Vec[KeyMinus].setPosition(KeyMinus_MWPos);
  m_Button_Vec[KeyMinus].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyMinus].setClip(SDL_Rect{KeyMinus_Normal_xSS_px , KeyMinus_Normal_ySS_px , m_Button_Vec[KeyMinus].GetWidth(), m_Button_Vec[KeyMinus].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyMinus].setClip(SDL_Rect{KeyMinus_Pressed_xSS_px, KeyMinus_Pressed_ySS_px, m_Button_Vec[KeyMinus].GetWidth(), m_Button_Vec[KeyMinus].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyMult(static_cast<size_t>(ButtonsClips_Enum::KEY_MULTIPLY));
  m_Button_Vec[KeyMult].setPosition(KeyMult_MWPos);
  m_Button_Vec[KeyMult].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyMult].setClip(SDL_Rect{KeyMult_Normal_xSS_px , KeyMult_Normal_ySS_px , m_Button_Vec[KeyMult].GetWidth(), m_Button_Vec[KeyMult].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyMult].setClip(SDL_Rect{KeyMult_Pressed_xSS_px, KeyMult_Pressed_ySS_px, m_Button_Vec[KeyMult].GetWidth(), m_Button_Vec[KeyMult].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyDiv(static_cast<size_t>(ButtonsClips_Enum::KEY_DIVIDE));
  m_Button_Vec[KeyDiv].setPosition(KeyDiv_MWPos);
  m_Button_Vec[KeyDiv].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyDiv].setClip(SDL_Rect{KeyDiv_Normal_xSS_px , KeyDiv_Normal_ySS_px , m_Button_Vec[KeyDiv].GetWidth(), m_Button_Vec[KeyDiv].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyDiv].setClip(SDL_Rect{KeyDiv_Pressed_xSS_px, KeyDiv_Pressed_ySS_px, m_Button_Vec[KeyDiv].GetWidth(), m_Button_Vec[KeyDiv].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeySign(static_cast<size_t>(ButtonsClips_Enum::KEY_SIGN));
  m_Button_Vec[KeySign].setPosition(KeySign_MWPos);
  m_Button_Vec[KeySign].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeySign].setClip(SDL_Rect{KeySign_Normal_xSS_px , KeySign_Normal_ySS_px , m_Button_Vec[KeySign].GetWidth(), m_Button_Vec[KeySign].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeySign].setClip(SDL_Rect{KeySign_Pressed_xSS_px, KeySign_Pressed_ySS_px, m_Button_Vec[KeySign].GetWidth(), m_Button_Vec[KeySign].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyPower(static_cast<size_t>(ButtonsClips_Enum::KEY_POWER));
  m_Button_Vec[KeyPower].setPosition(KeyPower_MWPos);
  m_Button_Vec[KeyPower].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyPower].setClip(SDL_Rect{KeyPower_Normal_xSS_px , KeyPower_Normal_ySS_px , m_Button_Vec[KeyPower].GetWidth(), m_Button_Vec[KeyPower].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyPower].setClip(SDL_Rect{KeyPower_Pressed_xSS_px, KeyPower_Pressed_ySS_px, m_Button_Vec[KeyPower].GetWidth(), m_Button_Vec[KeyPower].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyDel(static_cast<size_t>(ButtonsClips_Enum::KEY_DEL));
  m_Button_Vec[KeyDel].setPosition(KeyDel_MWPos);
  m_Button_Vec[KeyDel].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyDel].setClip(SDL_Rect{KeyDel_Normal_xSS_px , KeyDel_Normal_ySS_px , m_Button_Vec[KeyDel].GetWidth(), m_Button_Vec[KeyDel].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyDel].setClip(SDL_Rect{KeyDel_Pressed_xSS_px, KeyDel_Pressed_ySS_px, m_Button_Vec[KeyDel].GetWidth(), m_Button_Vec[KeyDel].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  size_t KeyPct(static_cast<size_t>(ButtonsClips_Enum::KEY_PERCENT));
  m_Button_Vec[KeyPct].setPosition(KeyPct_MWPos);
  m_Button_Vec[KeyPct].setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button_Vec[KeyPct].setClip(SDL_Rect{KeyPct_Normal_xSS_px , KeyPct_Normal_ySS_px , m_Button_Vec[KeyPct].GetWidth(), m_Button_Vec[KeyPct].GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button_Vec[KeyPct].setClip(SDL_Rect{KeyPct_Pressed_xSS_px, KeyPct_Pressed_ySS_px, m_Button_Vec[KeyPct].GetWidth(), m_Button_Vec[KeyPct].GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  /* Check number of buttons */

  if ( NumOfCreatedButtons == NumOfExpectedButtons )
  {
    Msg.str(std::string());
    Msg << "Number of created buttons OK.";
    Supervisor::Get().PrintMessage(Msg.str());
  }
  else
  {
    Msg.str(std::string());
    Msg << "Number of created buttons: " << NumOfCreatedButtons << ". Expected: " << NumOfExpectedButtons;
    Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::WARNING);
  }

  /* Create other components */

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

  /* Check number of other components */

  if ( NumOfCreatedComponents == NumOfExpectedComponents )
  {
    Msg.str(std::string());
    Msg << "Number of created components OK.";
    Supervisor::Get().PrintMessage(Msg.str());
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