#include "Texture.hpp"
#include "Renderer.hpp"
#include "colours.hpp"
#include <SDL_Image.h>


Texture::Texture(void)
  : m_Texture( NULL ), m_Width( 0 ), m_Height( 0 )
{
  printf("\nTexture initialised");
}


/**
 * @brief Deallocates dynamic memory.
 **/
Texture::~Texture(void)
{
  // Deallocate
  free();
}


/**
 * @brief Loads an image to be used as a texture.
 * 
 * @param Path The path of the source image.
 * @param Renderer_Ptr The SDL renderer that will render this texture.
 * @return true if successful; false otherwise.
 **/
bool Texture::loadFromFile( const std::string& Path, SDL_Renderer* Renderer_Ptr )
{
  // Get rid of preexisting texture
  free();

  // The final texture
  SDL_Texture* newTexture = NULL;

  // Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load( Path.c_str() );

  if( loadedSurface == NULL )
  {
    printf( "\nUnable to load image \"%s\"! SDL_image Error: %s", Path.c_str(), IMG_GetError() );
  }
  else
  {
    printf( "\nImage \"%s\" loaded", Path.c_str() );

    // Color key image
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, CYAN_R, CYAN_G, CYAN_B ) );

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( Renderer_Ptr, loadedSurface );

    if( newTexture == NULL )
    {
      printf( "\nUnable to create texture from \"%s\"! SDL Error: %s", Path.c_str(), SDL_GetError() );
    }
    else
    {
      printf( "\nTexture created from \"%s\"", Path.c_str() );

      // Get image dimensions
      m_Width  = loadedSurface->w;
      m_Height = loadedSurface->h;
    }

    // Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
  }

  // Return success
  m_Texture = newTexture;

  return m_Texture != NULL;
}


#if defined(SDL_TTF_MAJOR_VERSION)
bool Texture::loadFromRenderedText( const std::string& textureText, SDL_Color textColor )
{
  // Get rid of preexisting texture
  free();

  // Render text surface
  SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );

  if( textSurface == NULL )
  {
    printf( "\nUnable to render text surface! SDL_ttf Error: %s", TTF_GetError() );
  }
  else
  {
    printf( "\nText surface \"%s\" loaded", textureText.c_str() );

    // Create texture from surface pixels
        m_Texture = SDL_CreateTextureFromSurface( Renderer_Ptr, textSurface );

    if( m_Texture == NULL )
    {
      printf( "\nUnable to create texture from rendered text! SDL Error: %s", SDL_GetError() );
    }
    else
    {
      printf( "\nTexture created from \"%s\"", textureText.c_str() );

      // Get image dimensions
      m_Width  = textSurface->w;
      m_Height = textSurface->h;
    }

    // Get rid of old surface
    SDL_FreeSurface( textSurface );
  }

  // Return success
  return m_Texture != NULL;
}
#endif


/**
 * @brief Deallocates a texture.
 **/
void Texture::free(void)
{
  // Free texture if it exists
  if( m_Texture != NULL )
  {
    SDL_DestroyTexture( m_Texture );
    m_Texture = NULL;
    m_Width   = 0;
    m_Height  = 0;
  }
  else
  {;}
}


/**
 * @brief Set texture's RGB colour modulation.
 **/
void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
  SDL_SetTextureColorMod( m_Texture, red, green, blue );
}


/**
 * @brief Set texture's blending.
 * 
 * @param blending 
 **/
void Texture::setBlendMode( SDL_BlendMode blending )
{
  SDL_SetTextureBlendMode( m_Texture, blending );
}


/**
 * @brief Set texture's alpha modulation.
 * 
 * @param alpha 
 **/
void Texture::setAlpha( Uint8 alpha )
{
  SDL_SetTextureAlphaMod( m_Texture, alpha );
}


/**
 * @brief Renders texture at given (x, y) point of the target window.
 *
 * @param x x coordinate of the destination rendering point.
 * @param y y coordinate of the destination rendering point.
 * @param SourceClip Rectangle defining which portion of the texture we wish to render. Defaults to
 * NULL (to render the whole texture).
 **/
void Texture::render( int x, int y, SDL_Rect* SourceClip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
  // Set rendering space and render texture to screen
  SDL_Rect Destination = { x, y, m_Width, m_Height };

  // Set source clip's rendering dimensions
  if( SourceClip != NULL )
  {
    Destination.w = SourceClip->w;
    Destination.h = SourceClip->h;
  }
  else
  {;}

  // Render to screen
  SDL_RenderCopyEx( Renderer::Get().GetSDLRendererPtr(), m_Texture, SourceClip, &Destination, angle, center, flip );
}


int Texture::getWidth(void) const
{
  return m_Width;
}


int Texture::getHeight(void) const
{
  return m_Height;
}


/**
 * @brief Checks whether the texture has been successfully created.
 * 
 * @return true, false 
 **/
bool Texture::isValid(void) const
{
  return !(m_Texture == nullptr);
}
