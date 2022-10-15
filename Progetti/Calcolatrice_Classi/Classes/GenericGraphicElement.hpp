#ifndef GENERICGRAPHICELEMENT_HPP
#define GENERICGRAPHICELEMENT_HPP

#include <SDL.h>
#include "AbstractGraphicElement.hpp"

/**
 * @brief This class represents a generic, non-interactable graphic element.
 **/
class GenericGraphicElement : public AbstractGraphicElement
{
private:

public:

  GenericGraphicElement(void);

  virtual void render ( void ) override;

  void setClip ( const SDL_Rect& );

private:

  SDL_Rect m_Clip_Rect; // The single clip used for this element
};

#endif // GENERICGRAPHICELEMENT_HPP