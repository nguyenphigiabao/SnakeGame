# Snake Game — C++17 / Raylib 5.5

Native game for Windows 10/11, designed to match the reference image: an 808 × 808 window, dark green HUD, a 17 × 15 board with 44 px cells, a rounded continuous blue snake, and a primitive-drawn apple. The project does not use HTML, JavaScript, or another game engine.

## Screenshots

Screenshots captured layout/UI and gameplay of the SnakeGame on PC.

<tr>
    <td align="center"><img src="https://i.postimg.cc/CLZtyyrT/snakegameexam.png" width="1280" height="720" alt="SnakeGame" />
</tr>

## Run immediately (no build tools required)

The repository now includes a prebuilt Windows executable at the project root. After downloading **Code → Download ZIP** and extracting it, open PowerShell in the extracted folder and run:

```powershell
.\SnakeGame.exe
```

Keep the `assets` folder beside `SnakeGame.exe`. This method does not require CMake, MinGW, or Visual Studio Code.

## Build from source

The GitHub **Code → Download ZIP** archive contains the source code, not the generated `build` folder or `.exe` file. After extracting it, install CMake 3.20+ and MinGW-w64, then build once with the helper script:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\build.ps1
```

Then run the generated executable:

```powershell
.\build\bin\SnakeGame.exe
```

If PowerShell reports that script execution is disabled, the `Set-ExecutionPolicy` command above enables scripts only for the current terminal. Keep the `assets` folder next to the executable when copying the game elsewhere. If the font is missing, the game falls back to the default font; audio is synthesized in memory and requires no sound files.

## Build

Verified environment: Windows, MinGW-w64 GCC 8.1.0 provided by CodeBlocks, and portable CMake 3.31.6 in `.tools`. The system PATH is not modified. The VS Code configuration uses `g++.exe` and `gdb.exe` from the machine PATH.

You can also build manually. Add CMake to the PATH **for the current terminal only**:

```powershell
$env:Path = "$PWD\.tools\cmake-3.31.6-windows-x86_64\bin;$env:Path"
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j 4
.\build\bin\SnakeGame.exe
```

After the first configuration, you can use:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

On another machine, install CMake >= 3.20 and MinGW-w64 (C/C++ compiler, mingw32-make, and GDB), then add their `bin` directory to PATH. If you do not use the portable CMake package, change `command` in `.vscode/tasks.json` to `cmake`/`ctest`. Raylib 5.5 is downloaded automatically through FetchContent using a pinned tag and SHA-256 verification; the first configuration requires Internet access. An existing Raylib 5.5 installation can also be supplied through `CMAKE_PREFIX_PATH`.

The executable is placed at `build/bin/SnakeGame.exe`. With MinGW, `CMAKE_BUILD_TYPE` selects Debug or Release. In VS Code, install the Microsoft C/C++ extension, press **Ctrl+Shift+B** to build, and press **F5** to debug with GDB.

## Controls

| Key/button | Action |
|---|---|
| WASD or arrow keys | Start the game and change direction |
| Space | Start / pause / resume; restart after game over |
| R | Reset to the Ready state |
| Esc | Pause while playing; return to Ready from pause or a result screen |
| F11 or the fullscreen icon | Toggle fullscreen |
| M or the speaker icon | Toggle all audio |
| X on the HUD | Save the score and close the game |

The Pause screen provides Resume and Restart. Game Over and Victory provide Play Again and Exit. The game pauses automatically when the window loses focus. During resizing, the square play area stays centered and keeps its aspect ratio; unused space is filled with a dark background.

## Gameplay and animation

- Logic uses `std::array<Cell, 255>` with **1-based** coordinates and a direction queue of up to three inputs. Each tick consumes one direction, and an immediate 180-degree reversal is rejected.
- The initial tick interval is 0.155 seconds, reduced by 0.0025 seconds per point and capped at 0.075 seconds. Rendering targets 120 FPS with VSync.
- Previous and current positions are stored for every segment and interpolated between ticks. The head and eyes rotate along the shortest arc, while thick lines and round joints create a continuous body. A new tail segment starts at the old tail position to prevent a visual jump during growth.
- The canvas is rendered at 2× resolution and downsampled for smoother curves. Shapes use Raylib primitives and require no sprite images.
- Apples are selected from empty cells, including when the board is nearly full. Self-collision checks exclude the tail cell that is leaving on a normal move.
- Apples animate in and out with particles; the score has a small bounce effect, buttons animate on hover/press, and result popups fade in.
- Five sounds are generated with sine waves and envelopes: start, eat, high-score, game over, and UI click. Gameplay continues if no audio device is available.
- `highscore.txt` is stored **next to the executable**, validated for values from 0 to 252, updated when a record is broken, and saved before exit. The game writes a temporary file and replaces the destination on Windows.

## Structure

- `src/main.cpp`: window, frame loop, render canvas, and mouse scaling.
- `game`: state management, fixed timestep, input, score, and persistence.
- `snake`: body array, direction queue, collisions, interpolation, and drawing.
- `board`, `apple`: board rendering, apple spawning, and drawing.
- `ui`, `animation`, `audio`: HUD/popups, effects, and synthesized audio.
- `include/config.h`: dimensions, colors, speed, and limits.
- `tests`: logic tests and real-window runtime tests with screenshots.

## Testing

```powershell
ctest --test-dir build --output-on-failure
# Run the real-window test; it closes automatically after a few seconds:
New-Item -ItemType Directory -Force build/qa | Out-Null
Push-Location build/qa
..\bin\SnakeRuntimeTests.exe
Pop-Location
```

Logic tests cover initialization, reversal rejection, rapid input buffering, growth, wall and self-collision, tail-cell release, 1,000 valid apple spawns, a full board, and invalid score files. Runtime tests cover eating and scoring, pause, restart, Game Over, Victory, resizing, fullscreen, and restoring the window size. Screenshots are written to `build/qa/`: `ready.png`, `paused.png`, `resized.png`, `fullscreen.png`, `turn.png`, `game-over.png`, and `victory.png`.

Automated checks do not replace listening to the audio or evaluating long-session controls by hand. FPS has not been benchmarked on low-end hardware. CMake may display a deprecation warning from Raylib 5.5's own CMakeLists; this is not a build failure.

## Assets and licenses

Raylib: [raylib 5.5](https://github.com/raysan5/raylib/tree/5.5), distributed under the zlib license. API references use the [Raylib cheatsheet](https://www.raylib.com/cheatsheet/raylib_cheatsheet_v5.5.pdf).

The Atkinson Hyperlegible font is provided by the Braille Institute under the SIL Open Font License 1.1; the full license is in `assets/fonts/OFL.txt`. The font source is [Google Fonts](https://github.com/google/fonts/tree/main/ofl/atkinsonhyperlegible). All other graphics and sounds are generated by the project code.
