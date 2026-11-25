# Snake Game — C++17 / Raylib 5.5

Native game for Windows 10/11, designed to match the reference image: an 808 × 808 window, dark green HUD, a 17 × 15 board with 44 px cells, a rounded continuous blue snake, and a primitive-drawn apple. The project does not use HTML, JavaScript, or another game engine.

## Screenshots

Screenshots captured layout/UI and gameplay of the SnakeGame on PC.

<tr>
    <td align="center"><img src="https://i.postimg.cc/CLZtyyrT/snakegameexam.png" width="1280" height="720" alt="SnakeGame" />
</tr>

## Download and play

### Option 1 — Play right away (no build tools needed)

1. Open the repository page on GitHub, click the green **Code** button, then **Download ZIP**.
2. Extract the ZIP anywhere, for example to your Desktop.
3. Open the extracted `SnakeGame` folder and double-click **`SnakeGame.exe`**.

Or, from a PowerShell window in that folder:

```powershell
.\SnakeGame.exe
```

Keep the `assets` folder next to `SnakeGame.exe` if you move the game somewhere else. If Windows SmartScreen warns about an unrecognized app, click **More info → Run anyway** (the executable is not code-signed).

When the game opens, pick a mode on the start screen:

| Mode | Lives | What happens when you crash |
|---|---|---|
| **Easy Mode** | 3 hearts (top-left, next to the score and best score) | You lose one heart and the snake respawns at the start. **Your score is kept.** Losing all three hearts ends the game. |
| **Hard Mode** | 1 heart | Classic rules: one crash is Game Over and the score resets to 0. |

Choose with the mouse, with **W/S** or **↑/↓** plus **Enter**, or press **1** (Easy) / **2** (Hard).

### Option 2 — Clone with Git and build from source

```powershell
git clone https://github.com/nguyenphigiabao/SnakeGame.git
cd SnakeGame
powershell -ExecutionPolicy Bypass -File .\build.ps1
.\build\bin\SnakeGame.exe
```

Requirements: Windows 10/11, **CMake 3.20+** and **MinGW-w64** (`g++`) on your PATH. The first build needs Internet access because Raylib 5.5 is downloaded automatically. `build.ps1` uses the portable CMake in `.tools` if CMake is not on PATH.

> **Important:** C++ changes only take effect after rebuilding. If you edit the source, run `build.ps1` again and launch `build\bin\SnakeGame.exe` (the `SnakeGame.exe` at the project root is a prebuilt copy and is not updated automatically).

If PowerShell reports that running scripts is disabled, the `-ExecutionPolicy Bypass` form above only affects that one command. If the font is missing, the game falls back to the default font; audio is synthesized in memory and needs no sound files.

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
| Mouse, W/S or ↑/↓ + Enter, 1 / 2 | Choose Easy or Hard Mode on the start screen |
| WASD or arrow keys | Start the game and change direction |
| Space | Start / continue after losing a heart / pause / resume; restart after game over |
| R | Reset to the Ready state |
| Esc | Pause while playing; return to Ready from pause or a result screen; back to mode selection from Ready |
| F11 or the fullscreen icon | Toggle fullscreen |
| M or the speaker icon | Toggle all audio |
| X on the HUD | Save the score and close the game |

The Pause screen provides Resume and Restart. Game Over and Victory provide Play Again (same mode) and Change Mode. The game pauses automatically when the window loses focus. During resizing, the square play area stays centered and keeps its aspect ratio; unused space is filled with a dark background.

## Gameplay and animation

- **Easy / Hard Mode:** Easy gives 3 lives shown as hearts in the HUD; a crash costs one heart (with a small pop animation), respawns a fresh snake and keeps the score and speed. Key presses are ignored for 0.6 s after a respawn so a held key does not restart instantly. Hard Mode keeps the original one-life rules. Both modes share the same best score.
- **Responsive controls:** if a turn is pressed shortly after the snake left a cell (the first 35% of a step, `cfg::TurnGrace`), that move is re-planned so the snake turns immediately instead of waiting for the next tick. Deliberate corners are never undone. VSync is disabled because Windows drivers queue 1–3 vsynced frames of input lag; the frame rate is capped at 144 FPS or the monitor refresh rate instead.
- Logic uses `std::array<Cell, 255>` with **1-based** coordinates. The head starts rotating as soon as a turn is registered. Otherwise the body turns at the next cell boundary. One more turn can be buffered for a quick second corner, and a newer legal follow-up replaces it. Direct 180-degree reversals are rejected.
- The initial tick interval is 0.155 seconds, reduced by 0.0025 seconds per point and capped at 0.075 seconds. Rendering is capped at 144 FPS (or the monitor refresh rate) without VSync.
- Previous and current positions are stored for every segment and interpolated between ticks. The head and eyes rotate along the shortest arc, while thick lines and round joints create a continuous body. A new tail segment starts at the old tail position to prevent a visual jump during growth.
- The canvas is rendered at 2× resolution and downsampled for smoother curves. Shapes use Raylib primitives and require no sprite images.
- Apples are selected from empty cells, including when the board is nearly full. Self-collision checks exclude the tail cell that is leaving on a normal move.
- Apples animate in and out with particles; the score has a small bounce effect, buttons animate on hover/press, and result popups fade in.
- Five sounds are generated with sine waves and envelopes: start, eat, high-score, game over, and UI click. Gameplay continues if no audio device is available.
- `highscore.txt` is stored **next to the executable**, validated for values from 0 to 252, updated when a record is broken, and saved before exit. The game writes a temporary file and replaces the destination on Windows.

## Structure

- `src/main.cpp`: window, frame loop, render canvas, and mouse scaling.
- `game`: state management (mode selection, lives, respawn), fixed timestep, input, instant turns, score, and persistence.
- `snake`: body array, direction queue, collisions, interpolation, and drawing.
- `board`, `apple`: board rendering, apple spawning, and drawing.
- `ui`, `animation`, `audio`: mode-selection screen, HUD with hearts, popups, effects, and synthesized audio.
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

Logic tests cover initialization, reversal rejection, rapid input buffering, growth, wall and self-collision, tail-cell release, 1,000 valid apple spawns, a full board, and invalid score files. Runtime tests cover the mode-selection screen, instant turns, Easy Mode lives and respawn, Hard Mode's single life, eating and scoring, pause, restart, Game Over, Victory, resizing, fullscreen, and restoring the window size. Screenshots are written to `build/qa/`: `menu.png`, `ready.png`, `easy-life-lost.png`, `paused.png`, `resized.png`, `fullscreen.png`, `turn.png`, `game-over.png`, and `victory.png`.

Automated checks do not replace listening to the audio or evaluating long-session controls by hand. FPS has not been benchmarked on low-end hardware. CMake may display a deprecation warning from Raylib 5.5's own CMakeLists; this is not a build failure.

## Assets and licenses

Raylib: [raylib 5.5](https://github.com/raysan5/raylib/tree/5.5), distributed under the zlib license. API references use the [Raylib cheatsheet](https://www.raylib.com/cheatsheet/raylib_cheatsheet_v5.5.pdf).

The Atkinson Hyperlegible font is provided by the Braille Institute under the SIL Open Font License 1.1; the full license is in `assets/fonts/OFL.txt`. The font source is [Google Fonts](https://github.com/google/fonts/tree/main/ofl/atkinsonhyperlegible). All other graphics and sounds are generated by the project code.
