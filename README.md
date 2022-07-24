<!-- omit in toc -->
# SDL 2

Repository dedicato agli esperimenti su SDL2, seguendo la guida di [Lazy Foo](https://lazyfoo.net/tutorials/SDL/index.php).

- [Getting Started](#getting-started)
- [Batch Scripts](#batch-scripts)
  - [`Global_Variables_For_Batch_Files.txt`](#global_variables_for_batch_filestxt)
  - [`Build.bat`](#buildbat)
  - [`Run.bat`](#runbat)


## Getting Started

Il sito di Lazy Foo possiede una guida dettagliata su come installare il development kit di SDL2. Questo è il riassunto:

- Scaricare [SDL Development Libraries](http://libsdl.org/download-2.0.php) (versione MinGW).
- Estrarre l'archivio in una cartella a propria scelta.
- Per MinGW-w64, utilizzeremo la versione **x86_64-w64-mingw32** di SDL (non la i686).
- (Facoltativo: in alternativa, usare `Run.bat`, vedi dopo) Inserire nella variabile d'ambiente `PATH` di Windows il percorso al file `x86_64-w64-mingw32\bin\SDL2.dll`.
- In alternativa, è possibile copiare il file `SDL2.dll` nella cartella di progetto.
- Scaricare [l'esempio](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/01_hello_SDL.zip) di Lazy Foo.
- Compilare l'esempio, linkando le librerie di SDL2. In questo repository, la cartella `01_Hello_SDL` contiene il batch script `Build.bat` con le istruzioni già pronte.
- Il batch script `Build.bat` assume che la cartella d'installazione di SDL sia `D:\Dati`. La posizione può essere liberamente modificata nello script.
- Lanciare l'eseguibile appena creato, mediante `Run.bat`.

## Batch Scripts

I progetti utilizzano dei batch script (`.bat`) per la compilazione e l'esecuzione.

### `Global_Variables_For_Batch_Files.txt`

Punto di raccolta centralizzato per tutte le variabili d'ambiente utilizzate nei vari *batch files*, i quali utilizzano un ciclo `for` per leggere questo *file* di testo e generare automaticamente le variabili d'ambiente. Se occorre cambiare una variabile d'ambiente, basta modificare questo *file*, senza bisogno di toccare i *batch files*.

### `Build.bat`

Usato per eseguire il processo di *build* di un progetto o esempio.

Usare l'opzione `-c` per eseguire il *clean*.

### `Run.bat`

Una volta eseguito il *build*, lanciare l'eseguibile con `Run.bat`. Questo *script* aggiunge automaticamente (e temporaneamente) alla variabile d'ambiente `PATH` il percorso alla libreria `SDL2.dll`.