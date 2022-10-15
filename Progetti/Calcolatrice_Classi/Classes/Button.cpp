#include "Button.hpp"
#include "Renderer.hpp"


Button::Button(void)
{
  m_CurrentSprite = ButtonSprite::BUTTON_SPRITE_NORMAL;
  m_Clips_RectVec.reserve(ButtonSprite::BUTTON_SPRITE_HOWMANY);
}


/**
 * @brief Sets the current clip of the sprite sheet that has do be drawn for this graphic element.
 *
 * @param Clip which clip of the sprite sheet has do be drawn.
 **/
void Button::setClip(const SDL_Rect& Clip, ButtonSprite Button_Sprite)
{
  m_Clips_RectVec[Button_Sprite] = Clip;
}


/**
 * @brief Manages mouse clicks. Overrides pure virtual function.
 *
 * @param Event The mouse event to be managed.
 **/
void Button::handleMouseEvent( SDL_Event* Event )
{
  // If mouse event happened
  if( /* Event->type == SDL_MOUSEMOTION || */ Event->type == SDL_MOUSEBUTTONDOWN || Event->type == SDL_MOUSEBUTTONUP )
  {
    // Get mouse position
    int x, y;
    SDL_GetMouseState( &x, &y );

    // Check if mouse is inside button
    bool inside = true;

    // Mouse is left of the button
    if( x < m_CurrentPosition_pt.x )
    {
      inside = false;
    }
    // Mouse is right of the button
    else if( x > m_CurrentPosition_pt.x + m_Width_px )
    {
      inside = false;
    }
    // Mouse above the button
    else if( y < m_CurrentPosition_pt.y )
    {
      inside = false;
    }
    // Mouse below the button
    else if( y > m_CurrentPosition_pt.y + m_Height_px )
    {
      inside = false;
    }
    else
    {;} // Mouse is inside the button

    if( !inside )
    {
      // Mouse is outside button
      m_CurrentSprite = BUTTON_SPRITE_NORMAL;
    }
    else
    {
      // Mouse is inside button
      // printf("\nInside!"); // TODO: Debug

      // Set mouse over sprite
      switch( Event->type )
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


/**
 * @brief Renders current button sprite.
 **/
void Button::render(void)
{
  Renderer::Get().GetSpriteSheet().render( m_CurrentPosition_pt.x, m_CurrentPosition_pt.y, &m_Clips_RectVec[m_CurrentSprite] );
}
