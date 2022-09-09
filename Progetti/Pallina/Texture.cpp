#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Texture.hpp"
#include "colours.hpp"


LTexture::LTexture(void)
  : m_Texture(NULL), m_Width(0), m_Height(0)
{ /* Initialize members */ }


LTexture::~LTexture(void)
{
  // Deallocate
  free();
}


bool LTexture::loadFromFile( SDL_Renderer* Renderer, const std::string& path )
{
  // Get rid of preexisting texture
  free();

  // The final texture
  SDL_Texture* newTexture = NULL;

  // Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

  if( loadedSurface == NULL )
  {
    printf( "\nUnable to load image \"%s\"! SDL_image Error: %s", path.c_str(), IMG_GetError() );
  }
  else
  {
    printf( "\nOK: image \"%s\" loaded", path.c_str() );

    // Color key image
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, CYAN_R, CYAN_G, CYAN_B ) );

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( Renderer, loadedSurface );

    if( newTexture == NULL )
    {
      printf( "\nUnable to create texture from \"%s\"! SDL Error: %s", path.c_str(), SDL_GetError() );
    }
    else
    {
      printf( "\nOK: texture created from \"%s\"", path.c_str() );

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
bool LTexture::loadFromRenderedText( SDL_Renderer* Renderer, TTF_Font* Font, const std::string& textureText, SDL_Color textColor )
{
  // Get rid of preexisting texture
  free();

  // Render text surface
  SDL_Surface* textSurface = TTF_RenderText_Blended( Font, textureText.c_str(), textColor );

  if( textSurface != NULL )
  {
    // Create texture from surface pixels
    m_Texture = SDL_CreateTextureFromSurface( Renderer, textSurface );

    if( m_Texture == NULL )
    {
      printf( "\nUnable to create texture from rendered text! SDL Error: %s", SDL_GetError() );
    }
    else
    {
      // Get image dimensions
      m_Width  = textSurface->w;
      m_Height = textSurface->h;
    }

    // Get rid of old surface
    SDL_FreeSurface( textSurface );
  }
  else
  {
    printf( "\nUnable to render text surface! SDL_ttf Error: %s", TTF_GetError() );
  }

  // Return success
  return m_Texture != NULL;
}
#endif


void LTexture::free(void)
{
  // Free texture if it exists
  if( m_Texture != NULL )
  {
    SDL_DestroyTexture( m_Texture );
    m_Texture = NULL;
    m_Width = 0;
    m_Height = 0;
  }
  else { /* Nothing to destroy */ }
}


/**
 * @brief Modulate texture rgb
 *
 * @param red
 * @param green
 * @param blue
 **/
void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
  SDL_SetTextureColorMod( m_Texture, red, green, blue );
}


/**
 * @brief Set blending function
 *
 * @param blending
 **/
void LTexture::setBlendMode( SDL_BlendMode blending )
{
  SDL_SetTextureBlendMode( m_Texture, blending );
}


/**
 * @brief Modulate texture alpha
 *
 * @param alpha
 **/
void LTexture::setAlpha( Uint8 alpha )
{
  SDL_SetTextureAlphaMod( m_Texture, alpha );
}


/**
 * @brief Renders the texture.
 *
 * @param x x position on the window where the texture will be rendered.
 * @param y y position on the window where the texture will be rendered.
 * @param clip used to render only a portion of the texture. Defaults to NULL (renders the whole texture).
 * @param angle the rotation angle of the texture around the centre. Defaults to 0.0 (no rotation).
 * @param centre centre of rotation. Defaults to NULL (renders around 0, 0).
 * @param flip flips the image around the vertical or horizontal axis. Defaults to SDL_flip_NONE (no flipping).
 **/
void LTexture::render( SDL_Renderer* Renderer, int x, int y, SDL_Rect* clip, double angle, SDL_Point* centre, SDL_RendererFlip flip )
{
  // Set rendering space and render to screen
  SDL_Rect renderQuad = { x, y, m_Width, m_Height };

  // Set clip rendering dimensions
  if( clip != NULL )
  {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }
  else { /* Render the whole texture */ }

  // Render to screen
  SDL_RenderCopyEx( Renderer, m_Texture, clip, &renderQuad, angle, centre, flip );
}


int LTexture::getWidth(void) const
{
  return m_Width;
}


int LTexture::getHeight(void) const
{
  return m_Height;
}
