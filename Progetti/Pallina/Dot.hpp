#ifndef DOT_H_
#define DOT_H_

#include <SDL.h>
#include "Texture.hpp"

/**
 * @brief The dot that will move around on the screen.
 **/
class Dot
{
  public:

  // Initializes the variables
  Dot(void);
  ~Dot(void);

  bool LoadTexture(SDL_Renderer*, const std::string& );

  // Takes key presses and adjusts the dot's velocity
  void handleEvent( SDL_Event& );

  void ProcessMovement(void);

  // Shows the dot on the screen relative to the camera
  void render( SDL_Renderer*, int, int );

  // Position accessors
  int getPosX(void) const;
  int getPosY(void) const;

  // Velocity accessors
  double getVelX_Debug(void) const;
  double getVelY_Debug(void) const;

  // Size accessors
  int getWidth (void) const;
  int getHeigth(void) const;

  // Number of decimal digits used in the acceleration m_DOT_ACC
  static constexpr size_t NUM_OF_DIGITS = 1;


  private:

  /**
   * @brief Allowed directions for the acceleration of the dot
   **/
  enum class ACCEL_DIR
  {
    UP,
    DOWN,
    LEFT,
    RIGHT,

    HOW_MANY
  };

  // The dimensions (width and height) of the dot
  static constexpr int DOT_W_px = 20;
  static constexpr int DOT_H_px = 20;

  void AccelReq   ( ACCEL_DIR );
  void AccelStop  ( ACCEL_DIR );

  // Maximum axis velocity of the dot
  static constexpr int m_DOT_MAX_VEL = 20;

  // Acceleration addition
  static constexpr double m_DOT_ACC = 0.4;

  // The X and Y offsets of the dot
  int m_PosX, m_PosY;

  // The velocity of the dot
  double m_VelX, m_VelY;

  // Used to set the direction of the acceleration
  bool m_IsAccelUp, m_IsAccelDown, m_IsAccelLeft, m_IsAccelRight, m_isBrakingRequested;

  LTexture m_DotTexture;
};


#endif // DOT_H_