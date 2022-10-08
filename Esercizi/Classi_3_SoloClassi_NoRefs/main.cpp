/**
 * @file main.cpp
 *
 * @brief Come la versione 2, con l'inserimento di un "SDL_Initialiser" dedicato
 * all'inizializzazione di tutti i sottosistemi SDL.
 *
 * Problema: probabilmente a causa di un errore nella sequenza d'inizializzazione, l'aggiunta
 * dell'SDL_Initialiser manda il programma in crash subito dopo averlo avviato. Rimuovere tutte le
 * references statiche (usate come costanti private) e usare le "::Get()" al loro posto ha risolto il
 * problema.
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
