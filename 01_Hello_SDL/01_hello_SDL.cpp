/**
 * @file 01_hello_SDL.cpp
 * @brief https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/mingw/
 * 
 * - Scaricare SDL Development Libraries (versione MinGW)
 * - Estrarre l'archivio in una cartella a propria scelta
 * - Per MinGW-w64, utilizzeremo la versione x86_64-w64-mingw32 di SDL (non la i686)
 * - Inserire nella PATH di Windows il percorso al file /x86_64-w64-mingw32/bin/SDL2.dll
 * - In alternativa, è possibile copiare il file SDL2.dll nella cartella di progetto
 * - Il batch script "Build.bat" assume che la cartella d'installazione di SDL sia
 * 	 "D:/Dati/". La posizione può essere liberamente modificata nello script.
 * - Lanciare "Build.bat": verrà compilato l'eseguibile.
 * - Lanciare l'eseguibile.
 **/

/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

int main( int argc, char* args[] )
{
	//The window we'll be rendering to
	SDL_Window* window = NULL;
	
	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
		//Create window
		window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( window == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get window surface
			screenSurface = SDL_GetWindowSurface( window );

			//Fill the surface white
			SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
			
			//Update the surface
			SDL_UpdateWindowSurface( window );

			//Wait two seconds
			SDL_Delay( 2000 );
		}
	}

	//Destroy window
	SDL_DestroyWindow( window );

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}