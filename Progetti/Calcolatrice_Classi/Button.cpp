#include "Button.hpp"
#include "Renderer.hpp"


Button::Button(void)
{
  m_Position.x    = 0;
  m_Position.y    = 0;
  m_CurrentSprite = ButtonSprite::BUTTON_SPRITE_NORMAL;
}


void Button::setPosition( int x, int y )
{
  m_Position.x = x;
  m_Position.y = y;
}


void Button::setClip ( SDL_Rect Clip )
{
  m_Clip = Clip;
}


void Button::handleEvent( SDL_Event* e )
{
  // If mouse event happened
  if( /* e->type == SDL_MOUSEMOTION || */ e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
  {
    // Get mouse position
    int x, y;
    SDL_GetMouseState( &x, &y );

    // Check if mouse is inside button
    bool inside = true;

    // Mouse is left of the button
    if( x < m_Position.x )
    {
      inside = false;
    }
    // Mouse is right of the button
    else if( x > m_Position.x + BUTTON_W_px )
    {
      inside = false;
    }
    // Mouse above the button
    else if( y < m_Position.y )
    {
      inside = false;
    }
    // Mouse below the button
    else if( y > m_Position.y + BUTTON_H_px )
    {
      inside = false;
    }
    else
    {;} // Mouse is inside the button

    // Mouse is outside button
    if( !inside )
    {
      m_CurrentSprite = BUTTON_SPRITE_NORMAL;
    }
    // Mouse is inside button
    else
    {
      // Set mouse over sprite
      switch( e->type )
      {
        // case SDL_MOUSEMOTION:
        // mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
        // break;

        case SDL_MOUSEBUTTONDOWN:
          m_CurrentSprite = BUTTON_SPRITE_PRESSED;
          break;

        case SDL_MOUSEBUTTONUP:
          m_CurrentSprite = BUTTON_SPRITE_NORMAL;
          break;

        default:
          m_CurrentSprite = BUTTON_SPRITE_NORMAL;
          break;
      }
    }
  }
  else
  {;} // No mouse event happened
}


void Button::render(void)
{
  // Show current button sprite
  // Renderer::Get().GetKeysTexture().render( m_Position.x, m_Position.y, &gSpriteClips[ mCurrentSprite ] );
  Renderer::Get().GetKeysTexture().render( m_Position.x, m_Position.y, &m_Clip );
}
