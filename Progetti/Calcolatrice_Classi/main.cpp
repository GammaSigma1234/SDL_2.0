/**
 * @file main.cpp
 *
 * @brief
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

#include <iostream>
#include "Supervisor.hpp"
#include "SDL_Initialiser.hpp"
#include "MainWindow.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/


/***************************************************************************************************
* Private constants
****************************************************************************************************/


/***************************************************************************************************
* Main function
****************************************************************************************************/

int main( int argc, char* argv[] )
{
  std::cout << "\n\n*********************************** INIZIO ***********************************\n";

  if ( SDL_Initialiser::Get().isSDLInitialised() )
    std::cout << "\nSDL initialised.";
  else
    std::cout << "\nSDL not yet initialised.";

  Supervisor::Get().StartDebuggingConsole( argc, argv );

  while( !InputManager::Get().WasQuitRequested() && !Supervisor::Get().IsThereAnyFault() )
  {
    InputManager::Get().ManageInput();

    Renderer::Get().Render();
  }

  MainWindow::Get().DestroyWindow();
  SDL_Initialiser::Get().QuitSDL();

  std::cout << "\n***********************************  FINE  ***********************************\n\n";

  Supervisor::Get().PerformIntegrityCheck();
  Supervisor::Get().PressEnter();

  return 0;
}
