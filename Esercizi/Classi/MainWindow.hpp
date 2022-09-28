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

  MainWindow(const MainWindow&) = delete;

  static SDL_Window*  Get               (void);
  static bool         WasInitSuccessful (void);
  static void         Debug             (void);

  private:

  MainWindow(void);

  static constexpr int s_WINDOW_W = 640;
  static constexpr int s_WINDOW_H = 480;

  static MainWindow   s_MainWindow_Sgl;
  static SDL_Window*  s_Window;
  static bool         s_WasInitSuccessful;
};

#endif // MAINWINDOW_HPP