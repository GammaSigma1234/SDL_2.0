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

// static const char* NormalKeys_Path  ("./Sprites/Keys_Normal_400x600.png");
// static const char* PressedKeys_Path ("./Sprites/Keys_Pressed_400x600.png");
// static const char* Components_Path  ("./Sprites/Components_400x200_px.png");
static const char* AllComponents_Path  ("./Sprites/AllComponents_800x750.png");

static constexpr int FIRST_ONE_AVAILABLE = -1;

/* The following variables are used for the geometry of the buttons */

static constexpr int NormalButtonW_px(100);
static constexpr int NormalButtonH_px(100);

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

/* Positions of display and photovoltaic cell of the sprite sheets in pixels */

static constexpr int PhotovoltaicCell_x_px(0);
static constexpr int PhotovoltaicCell_y_px(0);
static constexpr int Display_x_px(0);
static constexpr int Display_y_px(100);

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
  /* Manually add here the sprite sheets to be loaded */

  // size_t SpriteSheetsNum(static_cast<size_t>(SpriteSheets_Enum::HOW_MANY));

  // m_SpriteSheets_Paths.push_back(NormalKeys_Path);
  // m_SpriteSheets_Paths.push_back(PressedKeys_Path);
  // m_SpriteSheets_Paths.push_back(Components_Path);

  /* Automatic management of the sprite sheets vector */

  // if ( m_SpriteSheets_Paths.size() == SpriteSheetsNum )
  // {
  //   Msg.str(std::string());
  //   Msg << "Sprite sheets enumeration successful.";
  //   Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::NO_FAULT);
  // }
  // else
  // {
  //   Msg.str(std::string());
  //   Msg << "*** WARNING ***\n\tNumber of specified sprite sheets: " << m_SpriteSheets_Paths.size() << ". Expected: " << SpriteSheetsNum;
  //   Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::BLOCKING);
  // }

  // m_SpriteSheets_Vec.resize(SpriteSheetsNum);

  // for (size_t i = 0; i != SpriteSheetsNum; ++i)
  // {
  //   m_SpriteSheets_Vec[i].loadFromFile( m_SpriteSheets_Paths[i], m_Renderer );

  //   if( m_SpriteSheets_Vec[i].isValid() )
  //   {
  //     Msg.str(std::string());
  //     Msg << "Texture creation successful.";
  //     Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::NO_FAULT);
  //   }
  //   else
  //   {
  //     Msg.str(std::string());
  //     Msg << "Failed to load \"" << m_SpriteSheets_Paths[i].c_str() << "\"!",
  //     Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::BLOCKING);

  //     m_WasInitSuccessful = false;
  //   }
  // }
}


void Renderer::CreateShapes_Pvt(void)
{
  /* Buttons */

  size_t NumOfCreatedButtons(0);
  size_t NumOfExpectedButtons(static_cast<size_t>(ButtonsClips_Enum::HOW_MANY));

  m_Button0.setPosition(SDL_Point{0, 650});
  m_Button0.setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button0.setClip(SDL_Rect{0  , 650, m_Button0.GetWidth(), m_Button0.GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button0.setClip(SDL_Rect{400, 650, m_Button0.GetWidth(), m_Button0.GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  m_Button1.setPosition(SDL_Point{100, 650});
  m_Button1.setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button1.setClip(SDL_Rect{100, 650, m_Button1.GetWidth(), m_Button1.GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button1.setClip(SDL_Rect{500, 650, m_Button1.GetWidth(), m_Button1.GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
  ++NumOfCreatedButtons;

  m_Button2.setPosition(SDL_Point{200, 650});
  m_Button2.setSize(NormalButtonW_px, NormalButtonH_px);
  m_Button2.setClip(SDL_Rect{200, 650, m_Button2.GetWidth(), m_Button2.GetHeight()}, Button::BUTTON_SPRITE_NORMAL);
  m_Button2.setClip(SDL_Rect{600, 650, m_Button2.GetWidth(), m_Button2.GetHeight()}, Button::BUTTON_SPRITE_PRESSED);
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

  // size_t NumOfCreatedComponents(0);
  // size_t NumOfExpectedComponents(static_cast<size_t>(ComponentsClips_Enum::HOW_MANY));

  // m_Components_Vec.resize(NumOfExpectedComponents);

  // size_t PhotoVoltCell(static_cast<size_t>(ComponentsClips_Enum::PHOTOVOLTAIC_CELL));
  // m_Components_Vec[PhotoVoltCell].setPosition(Key0_Pos);
  // m_Components_Vec[PhotoVoltCell].setClip(SDL_Rect{Column_0_px, Row_5_px, ButtonWidth_px, ButtonHeight_px});
  // ++NumOfCreatedComponents;

  // if ( NumOfCreatedButtons == NumOfExpectedComponents)
  // {
  //   Msg.str(std::string());
  //   Msg << "Number of created buttons OK.";
  //   Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::NO_FAULT);
  // }
  // else
  // {
  //   Msg.str(std::string());
  //   Msg << "Number of created buttons: " << NumOfCreatedButtons << ". Expected: " << NumOfExpectedButtons;
  //   Supervisor::Get().PrintMessage(Msg.str(), Supervisor::FaultLevel::WARNING);
  // }
}


// Texture& Renderer::GetSpriteSheet( SpriteSheets_Enum ss )
// {
//   size_t SpriteSheets_Index;

//   switch ( ss )
//   {
//     default:
//     case SpriteSheets_Enum::NORMAL_BUTTONS:
//       SpriteSheets_Index = static_cast<size_t>(SpriteSheets_Enum::NORMAL_BUTTONS);
//       return m_SpriteSheets_Vec[SpriteSheets_Index];
//       break;

//     case SpriteSheets_Enum::PRESSED_BUTTONS:
//       SpriteSheets_Index = static_cast<size_t>(SpriteSheets_Enum::PRESSED_BUTTONS);
//       return m_SpriteSheets_Vec[SpriteSheets_Index];

//     case SpriteSheets_Enum::COMPONENTS:
//       SpriteSheets_Index = static_cast<size_t>(SpriteSheets_Enum::COMPONENTS);
//       return m_SpriteSheets_Vec[SpriteSheets_Index];
//       break;
//   }
// }


Texture& Renderer::GetSpriteSheet( void )
{
  return m_SpriteSheet;
}


// std::vector<GraphicElement>& Renderer::GetButtonVector( void )
// {
//   return m_GraphicElem_Vec;
// }


/**
 * @brief Performs all the rendering.
 **/
void Renderer::Render(void)
{
  SDL_SetRenderDrawColor( m_Renderer, WHITE_R, WHITE_G, WHITE_B, ALPHA_MAX );

  SDL_RenderClear( m_Renderer );

  // for (auto& Button : m_GraphicElem_Vec)
  // {
  //   Button.render();
  // }

  m_Button0.render();
  m_Button1.render();
  m_Button2.render();

  // SDL_Rect Rect{0, 0, 50, 50};

  // m_SpriteSheet.render(100, 100, &Rect);

  // SDL_Rect Test1{0, 0, 50, 50};
  // SDL_Rect Test2{100, 100, 50, 50};
  // // Renderer::Get().GetSpriteSheet(SpriteSheets_Enum::COMPONENTS).render(0, 0, NULL);
  // Renderer::Get().GetSpriteSheet(SpriteSheets_Enum::COMPONENTS).render(100, 300, NULL);
  // // Renderer::Get().GetSpriteSheet(SpriteSheets_Enum::COMPONENTS).render(0, 0, &Test1);
  // // Renderer::Get().GetSpriteSheet(SpriteSheets_Enum::COMPONENTS).render(100, 100, &Test2);

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