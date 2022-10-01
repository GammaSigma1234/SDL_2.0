/**
 * @file main.cpp
 *
 * @brief Creazione di un programma con paradigma di programmazione ad oggetti, trattando la Main
 * Window e il Renderer come dei singleton. Per evitare problemi di interdipendenza (ad esempio, il
 * renderer necessita di una Main Window già inizializzata), si è dovuto ricorrere a una funzione di
 * inizializzazione.
 **/


/***************************************************************************************************
* Includes
****************************************************************************************************/

#include <iostream>
#include "Initialiser.hpp"
#include "InputManager.hpp"
#include "Renderer.hpp"


/***************************************************************************************************
* Private prototypes
****************************************************************************************************/

static void PressEnter           ( void );
static void StartDebuggingConsole( int, char* [] );
static void PerformIntegrityCheck( bool );


/***************************************************************************************************
* Private constants
****************************************************************************************************/

static auto& Renderer_Ref     (     Renderer::Get() );
static auto& InputManager_Ref ( InputManager::Get() );


/***************************************************************************************************
* Main function
****************************************************************************************************/

int main( int argc, char* argv[] )
{
  std::cout << "\n\n*********************************** INIZIO ***********************************\n";

  bool  HasProgramSucceeded;

  StartDebuggingConsole( argc, argv );

  HasProgramSucceeded = Initialiser_InitAll();

  while( !InputManager_Ref.WasQuitRequested() )
  {
    InputManager_Ref.ManageInput();

    Renderer_Ref.Render();
  }

  Initialiser_DeInitAll();

  PerformIntegrityCheck( HasProgramSucceeded );

  PressEnter();

  std::cout << "\n***********************************  FINE  ***********************************\n\n";
  return 0;
}


/***************************************************************************************************
* Private functions definitions
****************************************************************************************************/

static void PressEnter(void)
{
  int UserChoice = '\0';

  printf( "\nPress ENTER to continue..." );

  while ( ( UserChoice = getchar() ) != '\n' )
  {;}
}


static void StartDebuggingConsole( int argc, char* argv[] )
{
  printf( "\n*** Debugging console ***\n" );
  printf( "\nProgram \"%s\" started with %d additional arguments.", argv[0], argc - 1 ); // Il primo argomento è il nome dell'eseguibile

  for ( int i = 1; i != argc; ++i )
  {
    printf( "\nArgument #%d: %s\n", i, argv[i] );
  }
}


static void PerformIntegrityCheck( bool HasProgramSucceeded )
{
  if ( HasProgramSucceeded == true )
  {
    printf( "\nProgram ended successfully!\n" );
  }
  else
  {
    printf( "\nThere was a problem during the execution of the program!\n" );
  }
}