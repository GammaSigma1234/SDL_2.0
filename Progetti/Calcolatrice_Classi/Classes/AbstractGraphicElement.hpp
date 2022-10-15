#ifndef ABSTRACTGRAPHICELEMENT_HPP
#define ABSTRACTGRAPHICELEMENT_HPP

#include <SDL.h>
#include <vector>

/**
 * @brief This class represents a generic abstract graphic element to be drawn by the renderer. 
 **/
class AbstractGraphicElement
{
public:

  AbstractGraphicElement( void );
  AbstractGraphicElement( const SDL_Point&, int, int );

  void setPosition( const SDL_Point& );
  void setSize    ( int const, int const );
  int  GetWidth   ( void ) const;
  int  GetHeight  ( void ) const;

  virtual void render ( void ) = 0;

protected:
  
  int       m_Width_px;
  int       m_Height_px;
  SDL_Point m_CurrentPosition_pt; // Top left position
};

#endif // ABSTRACTGRAPHICELEMENT_HPP