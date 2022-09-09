#include "Dot.hpp"
#include "GlobConst.hpp"


Dot::Dot(void)
  : m_PosX(0), m_PosY(0), m_VelX(0.0), m_VelY(0.0),
    m_IsAccelUp(false), m_IsAccelDown(false), m_IsAccelLeft(false), m_IsAccelRight(false), m_isBrakingRequested(false)
{ /* Initialise all non-static private members */ }


Dot::~Dot(void)
{
  m_DotTexture.free();
}


void Dot::handleEvent( SDL_Event& e )
{
  // If a key was pressed (e.key.repeat == 0 serve per ignorare la ripetizione automatica in caso di
  // tasto tenuto premuto)
  if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
  {
    // Adjust the velocity
    switch( e.key.keysym.sym )
    {
      case SDLK_UP    : AccelReq  (Dot::ACCEL_DIR::UP);    break;
      case SDLK_DOWN  : AccelReq  (Dot::ACCEL_DIR::DOWN);  break;
      case SDLK_LEFT  : AccelReq  (Dot::ACCEL_DIR::LEFT);  break;
      case SDLK_RIGHT : AccelReq  (Dot::ACCEL_DIR::RIGHT); break;
      case SDLK_RETURN: m_isBrakingRequested = true;       break;
      default         :                                    break;
    }
  }
  // If a key was released
  else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
  {
    // Adjust the velocity
    switch( e.key.keysym.sym )
    {
      case SDLK_UP    : AccelStop(Dot::ACCEL_DIR::UP);    break;
      case SDLK_DOWN  : AccelStop(Dot::ACCEL_DIR::DOWN);  break;
      case SDLK_LEFT  : AccelStop(Dot::ACCEL_DIR::LEFT);  break;
      case SDLK_RIGHT : AccelStop(Dot::ACCEL_DIR::RIGHT); break;
      case SDLK_RETURN: m_isBrakingRequested = false;     break;
      default         :                                   break;
    }
  }
  else { /* Ignore this event */ }
}


/**
 * @brief Processes the movement of the dot.
 **/
void Dot::ProcessMovement(void)
{
 /******************************************************************
 * Process the acceleration of the dot, based on the user's request
 *******************************************************************/

  if ( m_IsAccelUp )
  {
    m_VelY = m_VelY - 1.0;
  }
  else { /* No upwards acceleration requested */ }

  m_VelY = m_VelY + GRAVITY;

  if ( m_IsAccelLeft )
  {
    m_VelX = m_VelX - m_DOT_ACC;
  }
  else { /* No upwards acceleration requested */ }

  if ( m_IsAccelRight )
  {
    m_VelX = m_VelX + m_DOT_ACC;
  }
  else { /* No downwards acceleration requested */ }


  /****************************
  * Speed check and saturation
  *****************************/

  if ( m_VelY < -m_DOT_MAX_VEL )
  {
    m_VelY = -m_DOT_MAX_VEL;
  }
  else { /* Max speed on negative y axis not yet reached */ }

  if ( m_VelY > m_DOT_MAX_VEL )
  {
    m_VelY = m_DOT_MAX_VEL;
  }
  else { /* Max speed on positive y axis not yet reached */ }

  if ( m_VelX > m_DOT_MAX_VEL )
  {
    m_VelX = m_DOT_MAX_VEL;
  }
  else { /* Max speed on positive x axis not yet reached */ }

  if ( m_VelX < -m_DOT_MAX_VEL )
  {
    m_VelX = -m_DOT_MAX_VEL;
  }
  else { /* Max speed on negative x axis not yet reached */ }


  /* Process braking */

  if ( m_isBrakingRequested )
  {
    if ( m_VelX > 0.0 )
    {
      m_VelX -= BRAKING_FACTOR;

      if ( m_VelX < 0.0 )
      {
        m_VelX = 0.0;
      }
      else {;}
    }
    else if ( m_VelX < 0.0 )
    {
      m_VelX += BRAKING_FACTOR;

      if ( m_VelX > 0.0 )
      {
        m_VelX = 0.0;
      }
      else {;}
    }
    else {;}
  }
  else {;}

 /****************************
 * Move the dot left or right
 *****************************/

  m_PosX += static_cast<int>(m_VelX);


 /**********************************************
 * If the dot went too far to the left
 ***********************************************/

  if( m_PosX < 0 )
  {
    // Move back
    m_IsAccelLeft  = false;
    m_PosX = 0;
    m_VelX = -(m_VelX / 2);
  }
  else { /* Movement was OK */ }


 /**********************************************
 * If the dot went too far to the right
 ***********************************************/

  if( m_PosX + DOT_W_px > LEVEL_W_px )
  {
    // Move back
    m_IsAccelRight = false;
    m_PosX = LEVEL_W_px - DOT_W_px;
    m_VelX = -(m_VelX / 2);
  }
  else { /* Movement was OK */ }


 /*************************
 * Move the dot up or down
 **************************/

  m_PosY += static_cast<int>(m_VelY);


 /************************************
 * If the dot went too far up
 *************************************/

  if( m_PosY < 0 )
  {
    // Move back
    m_IsAccelUp   = false;
    m_PosY = 0;
    m_VelY = -(m_VelY * BOUNCE_FACTOR);
  }
  else { /* Movement was OK */ }


 /************************************
 * If the dot went too far down
 *************************************/

  if( m_PosY + DOT_H_px > LEVEL_H_px - WALL_H_px )
  {
    // Move back
    m_IsAccelDown = false;
    m_PosY = LEVEL_H_px - DOT_H_px - WALL_H_px;
    m_VelY = -(m_VelY * BOUNCE_FACTOR);
  }
  else { /* Movement was OK */ }
}


/**
 * @brief Requests acceleration in a given direction.
 **/
void Dot::AccelReq( Dot::ACCEL_DIR Direction )
{
  switch ( Direction )
  {
    case Dot::ACCEL_DIR::UP:
      m_IsAccelUp = true;
      break;

    case Dot::ACCEL_DIR::DOWN:
      m_IsAccelDown = true;
      break;

    case Dot::ACCEL_DIR::LEFT:
      m_IsAccelLeft = true;
      break;

    case Dot::ACCEL_DIR::RIGHT:
      m_IsAccelRight = true;
      break;

    default:
      break;
  }
}


/**
 * @brief Stops acceleration requests in a given direction.
 **/
void Dot::AccelStop( Dot::ACCEL_DIR Direction )
{
  switch ( Direction )
  {
    case Dot::ACCEL_DIR::UP:
      m_IsAccelUp = false;
      break;

    case Dot::ACCEL_DIR::DOWN:
      m_IsAccelDown = false;
      break;

    case Dot::ACCEL_DIR::LEFT:
      m_IsAccelLeft = false;
      break;

    case Dot::ACCEL_DIR::RIGHT:
      m_IsAccelRight = false;
      break;

    default:
      break;
  }
}


/**
 * @brief // Show the dot relative to the camera
 *
 * @param camX Camera's x position
 * @param camY Camera's y position
 **/
void Dot::render( SDL_Renderer* Renderer, int camX, int camY )
{
  m_DotTexture.render(  Renderer, m_PosX - camX, m_PosY - camY );
}


int Dot::getPosX(void) const
{
  return m_PosX;
}


int Dot::getPosY(void) const
{
  return m_PosY;
}


double Dot::getVelX_Debug(void) const
{
  return m_VelX;
}


double Dot::getVelY_Debug(void) const
{
  return m_VelY;
}


int Dot::getWidth(void) const
{
  return DOT_W_px;
}


int Dot::getHeigth(void) const
{
  return DOT_H_px;
}


bool Dot::LoadTexture(SDL_Renderer* Renderer, const std::string& Path )
{
  bool Result = true;

  if ( !m_DotTexture.loadFromFile( Renderer, Path ) )
  {
    printf( "\nFailed to load dot texture!" );
    Result = false;
  }
  else
  {
    printf( "\nOK: dot texture loaded" );
  }

  return Result;
}