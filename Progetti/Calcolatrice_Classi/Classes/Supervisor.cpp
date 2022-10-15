/***************************************************************************************************
* Includes
****************************************************************************************************/

#include <iostream>
#include "Supervisor.hpp"


/***************************************************************************************************
* Methods
****************************************************************************************************/

/**
 * @brief Constructor
 **/
Supervisor::Supervisor(void)
  : m_isThereAnyFault(false)
{
  std::cout << "\nInitialising Supervisor...\n";
  std::cout << "\tOK: Supervisor initialised.\n";
}


/**
 * @brief Destuctor
 **/
Supervisor::~Supervisor(void)
{;}


/**
 * @brief Creates instance of singleton in static memory
 *
 * @return Supervisor&
 **/
Supervisor& Supervisor::Get(void)
{
  static Supervisor Instance;
  return Instance;
}


void Supervisor::PressEnter(void)
{
  int UserChoice = '\0';

  printf( "\nPress ENTER to continue..." );

  while ( ( UserChoice = getchar() ) != '\n' )
  {;}
}


void Supervisor::StartDebuggingConsole( int argc, char* argv[] )
{
  printf( "\n*** Debugging console ***\n" );
  printf( "\tProgram \"%s\" started with %d additional arguments.\n", argv[0], argc - 1 ); // Il primo argomento è il nome dell'eseguibile

  for ( int i = 1; i != argc; ++i )
  {
    printf( "\tArgument #%d: %s\n", i, argv[i] );
  }
}


void Supervisor::PerformIntegrityCheck( void )
{
  if ( m_isThereAnyFault  )
  {
    printf( "\nThere was a problem during the execution of the program!\n" );
  }
  else
  {
    printf( "\nProgram ended successfully!\n" );
  }
}


/**
 * @brief Call this method to signal to the Supervisor that there has been a problem somewhere.
 **/
void Supervisor::RaiseFault( void )
{
  m_isThereAnyFault = true;
}


void Supervisor::PrintMessage ( const std::string& Msg, FaultLevel Level)
{
  switch ( Level )
  {
  case FaultLevel::NO_FAULT:
    std::cout << "\nSupervisor OK: " << Msg;
    break;

  case FaultLevel::WARNING:
    std::cerr << "\nSupervisor WARNING: " << Msg;
    break;

  case FaultLevel::BLOCKING:
    std::cerr << "\nSupervisor BLOCKING FAULT: "  << Msg;
    std::exit(EXIT_FAILURE); // TODO: GS solo per test. Non va bene uscire così in caso di errore!
    break;

  default:
    std::cerr << "\nUnexpected error.";
    break;
  }
}


bool Supervisor::IsThereAnyFault( void )
{
  return m_isThereAnyFault;
}