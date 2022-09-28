/**
 * @file main.cpp
 *
 * @brief Tentativo di creare un programma ad oggetti trattando la main window e il renderer come
 * dei singleton, ma questo porta a dei problemi a causa dell'interdipendenza. Il renderer necessita
 * di una main window, ma se creiamo entrambi come delle classi singleton, non abbiamo la garanzia
 * che la main window venga istanziata prima del renderer. Pertanto, probabilmente è inevitabile
 * dover ricorrere a una funzione di inizializzazione.
 **/


#include <iostream>
#include "MainWindow.hpp"
#include "Renderer.hpp"


static void PressEnter(void);


int main( int argc, char* args[] )
{
  std::cout << "\n*********************************** INIZIO ***********************************\n";

  bool HasProgramSucceeded = true;

  printf("\n*** Debugging console ***\n");
  printf("\nProgram started with %d additional arguments.", argc - 1); // Il primo argomento è il nome dell'eseguibile

  for (int i = 1; i != argc; ++i)
  {
    printf("\nArgument #%d: %s\n", i, args[i]);
  }

  std::cout << "\nIndirizzo istanza MainWindow singleton: " << static_cast<void*>( MainWindow::Get() );
  std::cout << "\nIndirizzo istanza Renderer   singleton: " << static_cast<void*>( Renderer::Get() );
  // MainWindow::Debug();

  // Integrity check
  if ( HasProgramSucceeded == true )
  {
    printf("\nProgram ended successfully!\n");
  }
  else
  {
    printf("\nThere was a problem during the execution of the program!\n");
  }

  PressEnter();

  std::cout << "\n***********************************  FINE  ***********************************\n";
  return 0;
}


static void PressEnter(void)
{
  int UserChoice = '\0';

  printf("\nPress ENTER to continue...");

  while ( ( UserChoice = getchar() ) != '\n' )
  {;}
}
