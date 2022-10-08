/**
 * @file SDL_Initialiser.hpp
 *
 * @brief
 **/

#ifndef SDL_INITIALISER_HPP
#define SDL_INITIALISER_HPP

/**
 * @brief
 **/
class SDL_Initialiser
{
public:

  SDL_Initialiser(const SDL_Initialiser& ) = delete;
  SDL_Initialiser(      SDL_Initialiser&&) = delete;

  SDL_Initialiser& operator=(const SDL_Initialiser&)  = delete;
  SDL_Initialiser& operator=(      SDL_Initialiser&&) = delete;

  static SDL_Initialiser& Get(void);

  bool isSDLInitialised     (void);
  void QuitSDL              (void);

private:

   SDL_Initialiser(void);
  ~SDL_Initialiser(void);

  bool m_WasInitSuccessful;
};


#endif // SDL_INITIALISER_HPP