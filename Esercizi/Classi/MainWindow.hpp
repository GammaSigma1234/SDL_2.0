#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <SDL.h>

class MainWindow
{
  public:

  MainWindow(const MainWindow&) = delete;

  static SDL_Window* Get(void);
  static void Debug(void);

  private:

  MainWindow(void);

  static MainWindow MainWindow_Singleton;

  static constexpr int s_WINDOW_W = 640;
  static constexpr int s_WINDOW_H = 480;

  bool        m_SuccessfulInit = true;
  
  static SDL_Window* m_Window;
};

#endif // MAINWINDOW_HPP