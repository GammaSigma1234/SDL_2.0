/**
 * @file Supervisor.hpp
 *
 * @brief Singleton class. Acts as a supervisor / checker of the entire program.
 **/

#ifndef SUPERVISOR_HPP
#define SUPERVISOR_HPP

#include <string>


/**
 * @brief Singleton supervisor class
 **/
class Supervisor
{
public:

  enum class FaultLevel
  {
    NO_FAULT = 0,
    WARNING,
    BLOCKING,

    HOW_MANY
  };

  Supervisor( const Supervisor&  ) = delete;
  Supervisor(       Supervisor&& ) = delete;

  Supervisor& operator=( const Supervisor&  ) = delete;
  Supervisor& operator=(       Supervisor&& ) = delete;

  static Supervisor& Get(void);

  void StartDebuggingConsole( int, char* [] );
  void PressEnter           ( void );
  void PerformIntegrityCheck( void );
  void RaiseFault           ( void );
  void PrintMessage         ( const std::string&, FaultLevel = FaultLevel::NO_FAULT);
  bool IsThereAnyFault      ( void );

private:

  bool m_isThereAnyFault; // Raised when there is a problem during execution

   Supervisor( void );
  ~Supervisor( void );
};

#endif // SUPERVISOR_HPP