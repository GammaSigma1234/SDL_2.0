#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SDL.h>
#include "GraphicElement.hpp"
#include "I_Clickable.hpp"

/**
 * @brief This class represents a clickable button. It has a sprite enumeration, to change the
 * rendered sprite when the button is clicked.
 **/
class Button : public GraphicElement, public I_Clickable
{
private:

public:

  enum ButtonSprite
  {
    BUTTON_SPRITE_NORMAL  = 0,
    BUTTON_SPRITE_PRESSED,

    BUTTON_SPRITE_HOWMANY
  };

  Button(void);

  virtual void handleMouseEvent ( SDL_Event* ) override;
  virtual void render           ( void )       override;

          void setClip          ( const SDL_Rect&, ButtonSprite );

private:

  ButtonSprite          m_CurrentSprite; // Currently used sprite
  std::vector<SDL_Rect> m_Clips_RectVec; // All the usable clips for this element
};

#endif // BUTTON_HPP