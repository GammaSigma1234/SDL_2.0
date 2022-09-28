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


/***************************************************************************************************
* Singletons instances
****************************************************************************************************/

MainWindow MainWindow::s_MainWindow_Sgl;
Renderer     Renderer::s_Renderer_Sgl;


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
