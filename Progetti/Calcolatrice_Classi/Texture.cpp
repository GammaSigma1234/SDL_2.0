#include "Texture.hpp"
#include "Renderer.hpp"
#include "../../Colours_Lib/colours.hpp"
#include <SDL_Image.h>


Texture::Texture(void)
  : m_Texture( NULL ), mWidth( 0 ), mHeight( 0 )
{
  printf("\nTexture initialised");
}


Texture::~Texture(void)
{
  // Deallocate
  free();
}


bool Texture::loadFromFile( const std::string& path, SDL_Renderer* Renderer_Ptr )
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
    printf( "\nImage \"%s\" loaded", path.c_str() );

    // Color key image
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, CYAN_R, CYAN_G, CYAN_B ) );

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( Renderer_Ptr, loadedSurface );

    if( newTexture == NULL )
    {
      printf( "\nUnable to create texture from \"%s\"! SDL Error: %s", path.c_str(), SDL_GetError() );
    }
    else
    {
      printf( "\nTexture created from \"%s\"", path.c_str() );

      // Get image dimensions
      mWidth  = loadedSurface->w;
      mHeight = loadedSurface->h;
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
      mWidth  = textSurface->w;
      mHeight = textSurface->h;
    }

    // Get rid of old surface
    SDL_FreeSurface( textSurface );
  }

  // Return success
  return m_Texture != NULL;
}
#endif


void Texture::free(void)
{
  // Free texture if it exists
  if( m_Texture != NULL )
  {
    SDL_DestroyTexture( m_Texture );
    m_Texture = NULL;
    mWidth = 0;
    mHeight = 0;
  }
  else
  {;}
}


void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
  // Modulate texture's RGB
  SDL_SetTextureColorMod( m_Texture, red, green, blue );
}


void Texture::setBlendMode( SDL_BlendMode blending )
{
  // Set blending function
  SDL_SetTextureBlendMode( m_Texture, blending );
}


void Texture::setAlpha( Uint8 alpha )
{
  // Modulate texture alpha
  SDL_SetTextureAlphaMod( m_Texture, alpha );
}


/**
 * @brief Renders texture at given (x, y) point of the target window. Accepts a rectangle defining
 * which portion of the texture we want to render. Default argument of NULL to render the whole
 * texture.
 *
 * @param x x coordinate of the destination rendering point.
 * @param y y coordinate of the destination rendering point.
 * @param SourceClip Rectangle defining which portion of the texture we want to render. Defaults to
 * NULL (to render the whole texture).
 **/
// void Texture::render( int x, int y, SDL_Rect* SourceClip, double angle, SDL_Point* center, SDL_RendererFlip flip )
void Texture::render( SDL_Rect* SourceClip, SDL_Rect* DestClip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
  // // Set rendering space and render texture to screen
  // SDL_Rect Destination = { x, y, mWidth, mHeight };

  // // Set source clip's rendering dimensions
  // if( SourceClip != NULL )
  // {
  //   Destination.w = SourceClip->w;
  //   Destination.h = SourceClip->h;
  // }
  // else
  // {;}

  // Render to screen
  // SDL_RenderCopyEx( Renderer::Get().GetSDLRendererPtr(), m_Texture, SourceClip, &Destination, angle, center, flip );
  SDL_RenderCopyEx( Renderer::Get().GetSDLRendererPtr(), m_Texture, SourceClip, DestClip, angle, center, flip );
}


int Texture::getWidth(void) const
{
  return mWidth;
}


int Texture::getHeight(void) const
{
  return mHeight;
}
