/**
 * @file Supervisor.hpp
 *
 * @brief Singleton class. Acts as a supervisor / checker of the entire program.
 **/

#ifndef SUPERVISOR_HPP
#define SUPERVISOR_HPP

/**
 * @brief
 **/
class Supervisor
{
public:

  Supervisor( const Supervisor& )  = delete;
  Supervisor(       Supervisor&& ) = delete;

  Supervisor& operator=( const Supervisor& ) = delete;

  static Supervisor& Get(void);

  void PressEnter           ( void );
  void StartDebuggingConsole( int, char* [] );
  void PerformIntegrityCheck( void );
  void RaiseFault           ( void );
  bool IsThereAnyFault      ( void );

private:

  bool m_isThereAnyFault;

   Supervisor( void );
  ~Supervisor( void );

};

#endif // SUPERVISOR_HPP