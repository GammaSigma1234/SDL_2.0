<!-- omit in toc -->
# SDL 2

Repository dedicato agli esperimenti su SDL2, seguendo la guida di [Lazy Foo](https://lazyfoo.net/tutorials/SDL/index.php).

- [Getting Started](#getting-started)
- [Compilazione](#compilazione)
  - [Batch Scripts](#batch-scripts)
    - [`Global_Variables_For_Batch_Files.txt` (Inutilizzato)](#global_variables_for_batch_filestxt-inutilizzato)
    - [`Build.bat`](#buildbat)
    - [`Run.bat`](#runbat)
  - [Make Files](#make-files)


## Getting Started

Il sito di Lazy Foo possiede una guida dettagliata su come installare il development kit di SDL2. Questo è il riassunto:

- Scaricare [SDL Development Libraries](http://libsdl.org/download-2.0.php) (versione MinGW).
- Estrarre l'archivio in una cartella a propria scelta.
- Per MinGW-w64, utilizzeremo la versione **x86_64-w64-mingw32** di SDL (non la i686).
- (Facoltativo: in alternativa, usare `Run.bat`, vedi dopo) Inserire nella variabile d'ambiente `PATH` di Windows il percorso al file `x86_64-w64-mingw32\bin\SDL2.dll`.
- In alternativa, è possibile copiare il file `SDL2.dll` nella cartella di progetto.
- Scaricare [l'esempio](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/01_hello_SDL.zip) di Lazy Foo.
- Compilare l'esempio, linkando le librerie di SDL2. In questo *repository*, la cartella `01_Hello_SDL` contiene il *batch script* `Build.bat` con le istruzioni già pronte.
- Il *batch script* `Build.bat` assume che la cartella d'installazione di SDL sia `D:\Dati`. La posizione può essere liberamente modificata nello *script*.
- Lanciare l'eseguibile appena creato, mediante `Run.bat`.

## Compilazione

La compilazione può avvenire sia mediante i *batch script* (opera mia) oppure i *make file* messi a disposizione da Lazy Foo.

### Batch Scripts

I progetti utilizzano dei *batch script* (`.bat`) per la compilazione e l'esecuzione. Questi *batch file* sono stati aggiunti in seguito da me, cioè non sono parte del lavoro originale di Lazy Foo.

#### `Global_Variables_For_Batch_Files.txt` (Inutilizzato)

**ATTENZIONE**: questo metodo non è più utilizzato. Si è trattato di un esperimento interessante, ma a partire dal 30/07/2022 ogni *batch script* è autonomo rispetto a tutti gli altri. Questo per fornire maggiore flessibilità nelle opzioni di compilazione di ogni esempio.

Originariamente era un punto di raccolta centralizzato per tutte le variabili d'ambiente utilizzate nei vari *batch files*, i quali utilizzavano un ciclo `for` per leggere questo *file* di testo e generare automaticamente le variabili d'ambiente. Utile perché se occorreva cambiare una variabile d'ambiente, bastava modificare questo *file*, senza bisogno di toccare i *batch files*.

#### `Build.bat`

Usato per eseguire il processo di *build* di un progetto o esempio.

Usare l'opzione `-c` per eseguire il *clean*.

#### `Run.bat`

Una volta eseguito il *build*, lanciare l'eseguibile con `Run.bat`. Questo *script* aggiunge automaticamente (e temporaneamente) alla variabile d'ambiente `PATH` il percorso alle librerie dinamiche necessarie, come ad esempio `SDL2.dll` o `SDL2_image.dll`.

### Make Files

Mediante il tutorial, Lazy Foo spiega come impostare un minimo `makefile` per compilare un singolo esempio. L'unico elemento diverso tra il `makefile` di Lazy Foo e i miei *batch script* è la rimozione dell'istruzione `-Wl,-subsystem,windows`, la quale sopprime la *console* di Windows durante l'esecuzione di un esempio. La *console* è invece utile in fase di apprendimento e *debugging*.