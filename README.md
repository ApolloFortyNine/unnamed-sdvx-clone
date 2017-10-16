## Hold up!
You are on the `experimental` branch. **This is not a stable beta.** This version of the game might be unstable and buggy. Do NOT use this branch just for trying out "cutting edge" features. Bug finders only past this point!  
### [Take me back to the stable `master` branch](https://github.com/Drewol/unnamed-sdvx-clone/tree/master)

---

# Unnamed SDVX clone ![language: C/C++](https://img.shields.io/badge/language-C%2FC%2B%2B-green.svg)
A game based on [KShootMania](http://kshoot.client.jp/) and [SDVX](https://www.youtube.com/watch?v=JBHKNl87juA).

#### Demo Videos (Song Select/ Gameplay / Realtime effects):
[![Song Select Video](http://img.youtube.com/vi/GYOHy5CY2pU/1.jpg)](https://www.youtube.com/watch?v=GYOHy5CY2pU)
[![Gameplay Video](http://img.youtube.com/vi/dfEbmBzdvYA/1.jpg)](https://www.youtube.com/watch?v=dfEbmBzdvYA)
[![Realtime Effects Video](http://img.youtube.com/vi/PVIAENg13OU/1.jpg)](https://www.youtube.com/watch?v=PVIAENg13OU)

### Current features:
- Basic GUI (Buttons, Sliders, Scroll Boxes)
- OGG/MP3 Audio streaming (with preloading for gameplay performance)
- Uses KShoot charts (`\*.ksh`) (1.6 supported)
- Functional gameplay and scoring
- Saving of local scores
- Autoplay
- Basic controller support (Still some issues with TC+)
- Changable settings and key mapping in configuration file
- Real time sound effect and effect track support
- Song database cache for near-instant game startup (sqlite3)
- Song database searching
- Linux/Windows support

### Features currently on hold / in progress:
- GUI remake
- Song select UI/Controls to change HiSpeed and other game settings

The folder that is scanned for songs can be changed in the "Main.cfg" file (`songfolder = "path to song folder"`). Make sure you use a *plain* text editor like Notepad, Notepad++ or gedit; NOT a *rich* text editor like Wordpad or Word!  
If something breaks in the song database, delete "maps.db". **Please note this this will also wipe saved scores.**

## Controls
### Game (Customizable, read 'Readme_Input.txt'):
- BTN (White notes): \[D\] \[F\] \[J\] \[K\]
- FX (Yellow notes) = \[C\] \[M\] 
- VOL-L (Cyan laser, Move left / right) = \[W\] \[E\] 
- VOL-R (Magenta laser) = \[O\] \[P\]
- Quit song & go back to song selection \[Esc\]

### Song Select:
- Use the arrow keys to select a song and difficulty
- Use \[Page Down\]/\[Page Up\] to scroll faster
- Press \[Enter\] to start a song
- Press \[Ctrl\]+\[Enter\] to start song with autoplay
- Use the Search bar on the top to search for songs

## How to run:
Just run 'Main_Release' or 'Main_Debug' from within the 'bin' folder. Or, to play a chart immediately:  
### `{Download Location}/bin> Main_{Release or Debug} {path to *.ksh chart} [flags]`

#### Command line flags (all are optional):
- `-mute` - Mutes all audio output
- `-autoplay` - Plays chart automatically, no user input required
- `-autobuttons` Like autoplay, but just for the buttons. You only have to control the lasers
- `-autoskip` - Skips begining of song to the first chart note
- `-convertmaps` - Allows converting of `\*.ksh` charts to a binary format that loads faster (experimental, feature not complete)
- `-debug` - Used to show relevant debug info in game such as hit timings, and scoring debug info
- `-test` - Runs test scene, for development purposes only

## How to build:
Currently, the only way to get the experimental build is to build it:

### Windows:
1. Install [CMake](https://cmake.org/download/)
2. Run `cmake .` from the root of the project
3. Build the generated Visual Studio project 'FX.sln'
4. Run the excutable made in the 'bin' folder

To run from Visual Studio, go to Properties for Main > Debugging > Working Directory and set it to '$(OutDir)' or '..\bin'

### Linux:
1. Install [CMake](https://cmake.org/download/)
2. Check 'build.linux' for libraries to install
3. Run `cmake .` and then `make` from the root of the project
4. Run the excutable made in the 'bin' folder
