/**
 * @file Initialiser.cpp
 *
 * @brief Singletons are defined here in order to control the sequence of initialisation.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

#include "Initialiser.hpp"

#include "MainWindow.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"


/***************************************************************************************************
* Singletons instances
****************************************************************************************************/

MainWindow   MainWindow::s_MainWindow_Sgl;
Renderer     Renderer::s_Renderer_Sgl;
InputManager InputManager::s_InputManager_Sgl;


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static void MainWindow_Init(void);
static void Renderer_Init  (void);


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

  MainWindow_Init();

  Renderer_Init();

  return WasInitSuccessful;
}


void Initialiser_DeInitAll(void)
{
  MainWindow::DestroyWindow();

  // Quit SDL subsystems
  SDL_Quit();
}


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

static void MainWindow_Init(void)
{
  /* Perform init as needed here... */
}


static void Renderer_Init  (void)
{
  /* Perform init as needed here... */
}
