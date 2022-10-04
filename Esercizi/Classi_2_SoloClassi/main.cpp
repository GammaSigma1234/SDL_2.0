/**
 * @file main.cpp
 *
 * @brief Creazione di un programma con paradigma di programmazione ad oggetti, trattando la Main
 * Window, il Renderer e l'Input Manager come dei singleton.
 *
 * Questo esempio è simile alla versione 1, con la differenza che qui il modulo d'inizializzazione è
 * stato rimosso. L'aspetto curioso è che non sembra esserci alcun bisogno di gestire a mano
 * l'ordine d'istanziamento dei singleton: pare che il codice gestica automaticamente l'ordine
 * giusto affinché i singleton interdipendenti vengano istanziati nella sequenza prevista (Main
 * Window ==> Renderer ==> Input Manager).
 *
 * È stata inoltre aggiunta una classe di supervisione del funzionamento del programma.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

#include <iostream>
#include "Supervisor.hpp"
#include "MainWindow.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/


/***************************************************************************************************
* Private constants
****************************************************************************************************/

static auto&   Supervisor_Ref(   Supervisor::Get() );
static auto&   MainWindow_Ref(   MainWindow::Get() );
static auto&     Renderer_Ref(     Renderer::Get() );
static auto& InputManager_Ref( InputManager::Get() );


/***************************************************************************************************
* Main function
****************************************************************************************************/

int main( int argc, char* argv[] )
{
  std::cout << "\n\n*********************************** INIZIO ***********************************\n";

  Supervisor_Ref.StartDebuggingConsole( argc, argv );

  while( !InputManager_Ref.WasQuitRequested() && !Supervisor_Ref.IsThereAnyFault() )
  {
    InputManager_Ref.ManageInput();

    Renderer_Ref.Render();
  }

  MainWindow_Ref.DestroyWindow();

  Supervisor_Ref.PerformIntegrityCheck();
  Supervisor_Ref.PressEnter();

  std::cout << "\n***********************************  FINE  ***********************************\n\n";
  return 0;
}
