/**
 * @file 11_clip_rendering_and_sprite_sheets.cpp
 *
 * @brief Come l'esempio 11, ma semplificato per chiarire come funziona il rendering selettivo.
 *
 * Sunto:
 *  - Solita procedura d'inizializzazione di SDL e SLD_image (vedi i file precedenti).
 *  - Carichiamo l'immagine principale (4 cerchi in sfondo ciano) in "loadedSurface". È solo un
 *    punto di appoggio per poi creare la texture vera e propria in "Texture".
 *  - Impostiamo a ciano il colore di trasparenza dell'immagine "loadedSurface".
 *  - Creiamo quindi la vera texture a partire dalla loadedSurface. Eliminiamo la loadedSurface, ora
 *    divenuta inutile.
 *  - Avviamo il main loop.
 *  - Nel main loop, facciamo il clear dello schermo e poi renderizziamo i quattro punti colorati
 *    nelle posizioni indicate dai vari rettangoli di nome "Dest". La funzione "SDL_RenderCopy"
 *    richiede di specificare quattro paramteri: il motore di rendering; la texture da renderizzare;
 *    la sorgente (cioè la porzione di texture da renderizzare); la destinazione (dove fare apparire
 *    la porzione di texture nella finestra da renderizzare). La sorgente può coincidere con
 *    l'intera texture, mentre la destinazione può coincidere con l'intera finestra da renderizzare.
 *    Le dimensioni di sorgente e destinazione vengono controllate mediante degli SDL_Rect.
 *  - L'aggiornamento vero e proprio dello schermo viene fatto con SDL_RenderPresent.
 **/

/***************************************************************************************************
* Includes
****************************************************************************************************/

// Using SDL, SDL_image, standard math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>


/***************************************************************************************************
* Private functions
****************************************************************************************************/

static void PressEnter(void)
{
  int UserChoice = '\0';

  printf("\nPress ENTER to continue...");

  while ( ( UserChoice = getchar() ) != '\n' )
  {;}
}


/***************************************************************************************************
* Main function
****************************************************************************************************/

int main( int argc, char* args[] )
{
  bool HasProgramSucceeded = true;

  printf("\n*** Debugging console ***\n");
  printf("\nProgram \"%s\" started with %d additional arguments.", args[0], argc - 1); // Il primo argomento è il nome dell'eseguibile

  for (int i = 1; i != argc; ++i)
  {
    printf("\nArgument #%d: %s\n", i, args[i]);
  }

  const std::string FilePath("Colori_GS.png");

  SDL_Window*   Window;
  SDL_Renderer* Renderer;
  SDL_Texture*  Texture = NULL; // The final texture
  SDL_Surface*  loadedSurface = IMG_Load( FilePath.c_str() ); // Load image at specified path. Verrà convertita in texture

  constexpr int INITIALISE_FIRST_ONE_AVAILABLE = -1;

  constexpr int WINDOW_W = 1024;
  constexpr int WINDOW_H = 768;

  constexpr int INIT_R = 0xFF;
  constexpr int INIT_G = 0xFF;
  constexpr int INIT_B = 0xFF;
  constexpr int INIT_A = 0xFF;

  // Colore ciano
  constexpr int CYAN_R = 0x00;
  constexpr int CYAN_G = 0xFF;
  constexpr int CYAN_B = 0xFF;
  // constexpr int CYAN_A = 0xFF;

  // Posizioni e dimensioni dei quattro clip sorgente
  SDL_Rect RedPoint{0  , 0  , 200, 200}; // Punto rosso
  SDL_Rect GrnPoint{200, 0  , 200, 200}; // Punto verde
  SDL_Rect YlwPoint{0  , 200, 200, 200}; // Punto giallo
  SDL_Rect BluPoint{200, 200, 200, 200}; // Punto blu

  // Posizioni e dimensioni delle destinazioni dei quattro clip
  SDL_Rect Dest_TopLft{100, 100, 200, 200};
  SDL_Rect Dest_TopRgt{500, 100, 200, 200};
  SDL_Rect Dest_BotLft{100, 500, 200, 200};
  SDL_Rect Dest_BotRgt{500, 500, 200, 200};

  // Extra clips
  SDL_Rect Dest_Top{350, 150, 100, 100};
  SDL_Rect Dest_Bot{350, 550, 100, 100};
  SDL_Rect Dest_Lft{150, 350, 100, 100};
  SDL_Rect Dest_Rgt{550, 350, 100, 100};
  SDL_Rect Dest_Cnt{350, 350, 100, 100};

  std::cout << "\n*** Debugging console ***\n";

  /***************************************************************************************************
  * Inizializzazione SDL
  ****************************************************************************************************/

  if ( SDL_Init(SDL_INIT_VIDEO) != 0 )
  {
    std::cout << "\nERROR: could not initialise SDL." << SDL_GetError();
    HasProgramSucceeded = false;
  }
  else
  {
    std::cout << "\nSDL initialised.";
  }

  // Set texture filtering to linear
  if( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) == SDL_TRUE )
  {
    std::cout << "\nLinear texture filtering enabled.";
  }
  else
  {
    std::cout << "\nERROR: linear texture filtering not enabled!";
    HasProgramSucceeded = false;
  }

  Window = SDL_CreateWindow( "Colori", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN );

  if( Window == NULL )
  {
    std::cout << "\nERROR: window could not be created! SDL Error: " << SDL_GetError();
    HasProgramSucceeded = false;
  }
  else
  {
    std::cout << "\nWindow created.";
  }

  // Create renderer for window
  Renderer = SDL_CreateRenderer( Window, INITIALISE_FIRST_ONE_AVAILABLE, SDL_RENDERER_ACCELERATED );

  if( Renderer == NULL )
  {
    std::cout << "\nERROR: renderer could not be created! SDL Error: " << SDL_GetError();
    HasProgramSucceeded = false;
  }
  else
  {
    std::cout << "\nRenderer created.";
  }

  // Initialise renderer color
  SDL_SetRenderDrawColor( Renderer, INIT_R, INIT_G, INIT_B, INIT_A );

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;

  if( !( IMG_Init( imgFlags ) & imgFlags ) )
  {
    std::cout << "\nSDL_image could not initialise! SDL_mage Error: " << IMG_GetError();
    HasProgramSucceeded = false;
  }
  else
  {
    std::cout << "\nSDL_image initialised.";
  }

  /***************************************************************************************************
  * Caricamento immagine e preparazione texture
  ****************************************************************************************************/

  // Load sprite sheet texture
  loadedSurface = IMG_Load( FilePath.c_str() );

  if( loadedSurface == NULL )
  {
    std::cout << "\nERROR: unable to load image \"" << FilePath << "\"! SDL_image Error: " << IMG_GetError();
    HasProgramSucceeded = false;
  }
  else
  {
    std::cout << "\nImage \"" << FilePath << "\" loaded.";
  }

  // Color key image (selezione del ciano come colore trasparente)
  SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, CYAN_R, CYAN_G, CYAN_B ) );

  // Create texture from surface pixels
  Texture = SDL_CreateTextureFromSurface( Renderer, loadedSurface );

  if( Texture == NULL )
  {
    std::cout << "\nERROR: unable to create texture from \"" << FilePath << "\"! SDL Error: " << SDL_GetError();
  }
  else
  {
    std::cout << "\nTexture created from \"" << FilePath << "\"";
  }

  // Get rid of old loaded surface
  SDL_FreeSurface( loadedSurface );
  std::cout << "\nSurface removed.";

  /***************************************************************************************************
  * Main loop
  ****************************************************************************************************/

  // Main loop flag
  bool Quit = false;

  // Event handler
  SDL_Event Event;

  std::cout << "\nStarting main loop...";

  // While application is running
  while( !Quit )
  {
    // Handle events on queue
    while( SDL_PollEvent( &Event ) != 0 )
    {
      // User requests quit
      if( Event.type == SDL_QUIT )
      {
        Quit = true;
      }
      else
      {;} // Continue main loop
    }

    // Clear screen
    SDL_SetRenderDrawColor( Renderer, INIT_R, INIT_G, INIT_B, INIT_A );
    SDL_RenderClear( Renderer );

    SDL_RenderCopy( Renderer, Texture, &RedPoint, &Dest_TopLft ); // Render top left sprite
    SDL_RenderCopy( Renderer, Texture, &GrnPoint, &Dest_TopRgt ); // Render top right sprite
    SDL_RenderCopy( Renderer, Texture, &YlwPoint, &Dest_BotLft ); // Render bottom left sprite
    SDL_RenderCopy( Renderer, Texture, &BluPoint, &Dest_BotRgt ); // Render bottom right sprite

    SDL_RenderCopy( Renderer, Texture, &RedPoint, &Dest_Top ); // Extra
    SDL_RenderCopy( Renderer, Texture, &GrnPoint, &Dest_Rgt ); // Extra
    SDL_RenderCopy( Renderer, Texture, &YlwPoint, &Dest_Lft ); // Extra
    SDL_RenderCopy( Renderer, Texture, &BluPoint, &Dest_Bot ); // Extra
    SDL_RenderCopy( Renderer, Texture, &RedPoint, &Dest_Cnt ); // Extra

    SDL_RenderPresent( Renderer ); // Update screen
  }

  /***************************************************************************************************
  * Close and quit
  ****************************************************************************************************/

  std::cout << "\nPreparing to quit...";

  // Destroy window
  SDL_DestroyRenderer( Renderer );
  SDL_DestroyWindow( Window );
  Window   = NULL;
  Renderer = NULL;

  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();


  //  Integrity check
  if ( HasProgramSucceeded == true )
  {
    std::cout << "\nProgram ended successfully!\n";
  }
  else
  {
    std::cout << "\nERROR: there was a problem during the execution of the program!\n";
  }

  PressEnter();

  return 0;
}