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

   Texture(void);
  ~Texture(void);

  bool loadFromFile( const std::string&, SDL_Renderer* );

#if defined(SDL_TTF_MAJOR_VERSION)
  // Creates image from font string
  bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
#endif

  void  free        ( void );
  void  setColor    ( Uint8, Uint8, Uint8 );
  void  setBlendMode( SDL_BlendMode );
  void  setAlpha    ( Uint8 );
  void  render      ( int, int, SDL_Rect* = NULL, double = 0.0, SDL_Point* = NULL, SDL_RendererFlip = SDL_FLIP_NONE );
  int   getWidth    ( void ) const;
  int   getHeight   ( void ) const;
  bool  isValid     ( void ) const;

private:

  SDL_Texture*  m_Texture; // The actual hardware texture
  int           m_Width;
  int           m_Height;
};

#endif // TEXTURE_HPP