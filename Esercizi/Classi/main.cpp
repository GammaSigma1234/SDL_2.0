/**
 * @file main.cpp
 *
 * @brief
 **/


#include <iostream>
#include "MainWindow.hpp"


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

  std::cout << "\nIndirizzo istanza singleton: " << static_cast<void*>( MainWindow::Get() );
  MainWindow::Debug();

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
