#ifndef I_CLICKABLE_HPP
#define I_CLICKABLE_HPP

#include <SDL.h>

/**
 * @brief This interface is used to specify that a visual element is clickable.
 **/
class I_Clickable
{
public:

  I_Clickable(void) {;}

  virtual void handleMouseEvent( SDL_Event* ) = 0;

private:
};

#endif // I_CLICKABLE_HPP