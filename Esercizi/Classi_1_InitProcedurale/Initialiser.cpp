/**
 * @file Initialiser.cpp
 * 
 * @brief Initialises SDL.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

#include "Initialiser.hpp"

#include "MainWindow.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"


/***************************************************************************************************
* Private constants
****************************************************************************************************/

static MainWindow&     MainWindow_Ref(   MainWindow::Get() );
static Renderer&         Renderer_Ref(     Renderer::Get() );
static InputManager& InputManager_Ref( InputManager::Get() );


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/


/***************************************************************************************************
* Public functions definitions
****************************************************************************************************/

/**
 * @brief Initialises SDL and all the required submodules.
 *
 * @return true if successful; false otherwise.
 **/
bool Initialiser_InitAll(void)
{
  bool WasInitSuccessful = true;

  return WasInitSuccessful;
}


void Initialiser_DeInitAll(void)
{
  MainWindow_Ref.DestroyWindow();

  SDL_Quit();
}


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/
