#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SDL.h>
#include <string>


/**
 * @brief A texture.
 **/
class Texture
{
  public:
    // Initializes variables
    Texture(void);

    // Deallocates memory
    ~Texture(void);

    // Loads image at specified path
    bool loadFromFile( const std::string&, SDL_Renderer* );

    #if defined(SDL_TTF_MAJOR_VERSION)
    // Creates image from font string
    bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
    #endif

    // Deallocates texture
    void free(void);

    // Set color modulation
    void setColor( Uint8, Uint8, Uint8 );

    // Set blending
    void setBlendMode( SDL_BlendMode );

    // Set alpha modulation
    void setAlpha( Uint8 );

    // Renders texture at given point
    // void render( int, int, SDL_Rect* = NULL, double = 0.0, SDL_Point* = NULL, SDL_RendererFlip = SDL_FLIP_NONE );
    void render( SDL_Rect* = NULL, SDL_Rect* = NULL, double = 0.0, SDL_Point* = NULL, SDL_RendererFlip = SDL_FLIP_NONE );

    // Gets image dimensions
    int getWidth (void) const;
    int getHeight(void) const;

  private:
    // The actual hardware texture
    SDL_Texture* m_Texture;

    // Image dimensions
    int mWidth;
    int mHeight;
};

#endif // TEXTURE_HPP