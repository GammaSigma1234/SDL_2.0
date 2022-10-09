#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SDL.h>

/**
 * @brief This class represents a button. It has a constructor to initialize, a position setter, an
 * event handler for the event loop, and a rendering function. It also has a position and a sprite
 * enumeration, so we know which sprite to render for the button.
 **/
class Button
{
public:

  Button(void); // Initializes internal variables

  void setPosition( int, int ); // Sets top left position
  void setClip    ( SDL_Rect ); // Sets top left position
  void handleEvent( SDL_Event* ); // Handles mouse event
  void render     ( void ); // Shows button sprite

  static constexpr int BUTTON_W_px = 100;
  static constexpr int BUTTON_H_px = 100;

private:

  enum ButtonSprite
  {
    BUTTON_SPRITE_NORMAL  = 0,
    BUTTON_SPRITE_PRESSED,

    BUTTON_SPRITE_HOWMANY
  };

  SDL_Point    m_Position; // Top left position
  SDL_Rect     m_Clip;
  ButtonSprite m_CurrentSprite; // Currently used sprite
};

#endif // BUTTON_HPP