/**
 * @file MainWindow.hpp
 * 
 * @brief The main window.
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

  static SDL_Window*  Get               (void);
  static bool         WasInitSuccessful (void);
  static int          GetWindowWidth    (void);
  static int          GetWindowHeight   (void);
  static void         DestroyWindow     (void);

  private:

   MainWindow( void );
  ~MainWindow( void );

  static constexpr int s_WINDOW_W = 640;
  static constexpr int s_WINDOW_H = 480;

  static MainWindow   s_MainWindow_Sgl;
  static SDL_Window*  s_Window;
  static bool         s_WasInitSuccessful;
};

#endif // MAINWINDOW_HPP