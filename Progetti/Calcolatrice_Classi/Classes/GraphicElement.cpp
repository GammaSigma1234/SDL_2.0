#include "GraphicElement.hpp"
#include "Renderer.hpp"


GraphicElement::GraphicElement(void)
{
  m_Width_px              = 0;
  m_Height_px             = 0;
  m_CurrentPosition_pt.x  = 0;
  m_CurrentPosition_pt.y  = 0;
}


GraphicElement::GraphicElement( const SDL_Point& Point = SDL_Point{0, 0}, int Width = 0, int Height = 0 )
  : m_Width_px(Width), m_Height_px(Height), m_CurrentPosition_pt(Point)
{;}


/**
 * @brief Sets the position of the graphical element.
 *
 * @param Point The upper left corner of the element.
 **/
void GraphicElement::setPosition( const SDL_Point& Point )
{
  m_CurrentPosition_pt.x = Point.x;
  m_CurrentPosition_pt.y = Point.y;
}


/**
 * @brief Sets the size of the graphical element.
 * 
 * @param Width_px 
 * @param Height_px 
 **/
void GraphicElement::setSize( int const Width_px, int const Height_px)
{
  m_Width_px  = Width_px;
  m_Height_px = Height_px;
}


int GraphicElement::GetWidth(void) const
{
  return m_Width_px;
}


int GraphicElement::GetHeight(void) const
{
  return m_Height_px;
}