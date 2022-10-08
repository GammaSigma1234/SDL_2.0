/**
 * @brief This class represents a button. It has a constructor to initialize, a position setter, an
 * event handler for the event loop, and a rendering function. It also has a position and a sprite
 * enumeration, so we know which sprite to render for the button.
 **/
class LButton
{
  public:
    // Initializes internal variables
    LButton(void);

    // Sets top left position
    void setPosition( int, int );

    // Handles mouse event
    void handleEvent( SDL_Event* );

    // Shows button sprite
    void render(void);

  private:
    // Top left position
    SDL_Point mPosition;

    // Currently used global sprite
    LButtonSprite mCurrentSprite;
};
