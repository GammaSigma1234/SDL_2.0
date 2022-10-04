/**
 * @file MainWindow.hpp
 *
 * @brief The main window. It is a singleton class.
 **/

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <SDL.h>

/**
 * @brief Singleton main window class.
 **/
class MainWindow
{
  public:

  MainWindow( const MainWindow& )  = delete;
  MainWindow(       MainWindow&& ) = delete;

  MainWindow& operator=( const MainWindow& ) = delete;

  static MainWindow& Get(void);

  SDL_Window* GetSDLWindowPtr  (void);
  bool        WasInitSuccessful(void);
  int         GetWindowWidth   (void);
  int         GetWindowHeight  (void);
  void        DestroyWindow    (void);

  private:

   MainWindow( void );
  ~MainWindow( void );

  static constexpr int s_WINDOW_W = 640;
  static constexpr int s_WINDOW_H = 480;

  SDL_Window* m_Window;
  bool        m_WasInitSuccessful;
};

#endif // MAINWINDOW_HPP