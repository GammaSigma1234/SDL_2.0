#include "GenericGraphicElement.hpp"
#include "Renderer.hpp"


GenericGraphicElement::GenericGraphicElement(void)
{;}


/**
 * @brief Sets the current clip of the sprite sheet that has do be drawn for this graphic element.
 *
 * @param Clip which clip of the sprite sheet has do be drawn.
 **/
void GenericGraphicElement::setClip(const SDL_Rect& Clip)
{
  m_Clip_Rect = Clip;
}


/**
 * @brief Renders current button sprite.
 **/
void GenericGraphicElement::render(void)
{
  Renderer::Get().GetSpriteSheet().render( m_CurrentPosition_pt.x, m_CurrentPosition_pt.y, &m_Clip_Rect );
}
