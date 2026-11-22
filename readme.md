# Sudoku Suite (C++ / SFML)

A C++ Sudoku project containing three applications:

-   **Sudoku Game** --- a playable Sudoku application.
-   **Sudoku Solver** --- enter a Sudoku grid and solve it using the
    project's solver.
-   **Launcher** --- a menu that opens the game or solver.

The project supports multiple grid sizes, including **4×4, 9×9, 16×16
and 25×25** (where implemented by the application).

## Requirements

Before building the project, install:

1.  **A C++ compiler** with C++17 support (for example, MinGW-w64/GCC).
2.  **SFML 3.1.0** (or the SFML 3 version targeted by your source code).
3.  **Git Bash or another terminal** if you want to use the commands
    below as written.

Make sure the SFML package matches your compiler, architecture
(64-bit/32-bit), and runtime. A mismatched SFML build can cause linker
errors.

### Install and configure SFML

1.  Download SFML from the official website:
    https://www.sfml-dev.org/download/
2.  Choose the package matching your compiler and architecture.
3.  Extract the downloaded archive to a folder on your computer.
4.  Locate the extracted SFML folder. It should contain `include` and
    `lib` directories.
5.  In the build commands below, replace `SFML_PATH` with the location
    of that folder.

For example, if SFML is extracted to `C:/Libraries/SFML-3.1.0`, use:

-   Include directory: `C:/Libraries/SFML-3.1.0/include`
-   Library directory: `C:/Libraries/SFML-3.1.0/lib`

**Important:** `SFML_PATH` is a placeholder, not a literal folder name.
Change it to the actual location on your computer. Keep the path in
forward-slash format in Git Bash, or use a correctly quoted Windows
path.

You may also need the SFML runtime DLL files beside the generated `.exe`
files, or available through your system's `PATH`, depending on how SFML
was installed. If Windows reports that an SFML DLL is missing when you
launch an application, copy the required DLLs from SFML's `bin`
directory beside the executable.

## Project files

The build commands expect these source files to be in the current
terminal directory:

-   `gameApp.cpp`
-   `sudokuGame.cpp`
-   `sudoku_solver_app.cpp`
-   `sudokuBoard.cpp`
-   `sudokuSolver.cpp`
-   `unordered_map.cpp`
-   `LinkedList.cpp`
-   `launcher.cpp`

They also expect the corresponding header files to be included by the
source files.

## Build and run

Open Git Bash in the project directory. First replace `SFML_PATH` in
each command with your actual SFML installation directory.

### 1. Build and run the Sudoku Game

Compile:

``` bash
g++ -std=c++17 gameApp.cpp sudokuGame.cpp sudokuBoard.cpp sudokuSolver.cpp unordered_map.cpp LinkedList.cpp \
  -I"SFML_PATH/include" -L"SFML_PATH/lib" \
  -lsfml-graphics -lsfml-window -lsfml-system \
  -o gameApp.exe
```

Run:

``` bash
./gameApp.exe
```

### 2. Build and run the Sudoku Solver

Compile:

``` bash
g++ -std=c++17 sudoku_solver_app.cpp sudokuBoard.cpp sudokuSolver.cpp unordered_map.cpp LinkedList.cpp \
  -I"SFML_PATH/include" -L"SFML_PATH/lib" \
  -lsfml-graphics -lsfml-window -lsfml-system \
  -o sudoku_solver_app.exe
```

Run:

``` bash
./sudoku_solver_app.exe
```

### 3. Build and run the Launcher

Compile:

``` bash
g++ -std=c++17 launcher.cpp \
  -I"SFML_PATH/include" -L"SFML_PATH/lib" \
  -lsfml-graphics -lsfml-window -lsfml-system \
  -o launcher.exe
```

Run:

``` bash
./launcher.exe
```

**Build order:** Build `gameApp.exe` and `sudoku_solver_app.exe` before
running the launcher. The launcher opens these executable files, so they
must be present in the location expected by `launcher.cpp` (normally the
same directory as `launcher.exe`).

## 25×25 test puzzle

The following is a nearly complete 25×25 Sudoku test grid. The value
`-1` represents an empty cell. Use it to test the solver if your
application accepts grid input in this format.

``` text
-1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
 6 -1  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25  1  2  3  4  5
11 12 -1 14 15 16 17 18 19 20 21 22 23 24 25  1  2  3  4  5  6  7  8  9 10
16 17 18 -1 20 21 22 23 24 25  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
21 22 23 24 -1  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20

 2  3  4  5  6 -1  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25  1
 7  8  9 10 11 12 -1 14 15 16 17 18 19 20 21 22 23 24 25  1  2  3  4  5  6
12 13 14 15 16 17 18 -1 20 21 22 23 24 25  1  2  3  4  5  6  7  8  9 10 11
17 18 19 20 21 22 23 24 -1  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
22 23 24 25  1  2  3  4  5 -1  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21

 3  4  5  6  7  8  9 10 11 12 -1 14 15 16 17 18 19 20 21 22 23 24 25  1  2
 8  9 10 11 12 13 14 15 16 17 18 -1 20 21 22 23 24 25  1  2  3  4  5  6  7
13 14 15 16 17 18 19 20 21 22 23 24 -1  1  2  3  4  5  6  7  8  9 10 11 12
18 19 20 21 22 23 24 25  1  2  3 -1  5  6  7  8  9 10 11 12 13 14 15 16 17
23 24 25  1  2  3  4  5  6  7  8  9 -1 11 12 13 14 15 16 17 18 19 20 21 22

 4  5  6  7  8  9 10 11 12 13 14 15 16 17 -1 19 20 21 22 23 24 25  1  2  3
 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 -1 25  1  2  3  4  5  6  7  8
14 15 16 17 18 19 20 21 22 23 24 25  1  2  3  4 -1  6  7  8  9 10 11 12 13
19 20 21 22 23 24 25  1  2  3  4  5  6  7  8  9 10 -1 12 13 14 15 16 17 18
24 25  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 -1 18 19 20 21 22 23

 5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 -1 22 23 24 25  1  2  3  4
10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25  1 -1  3  4  5  6  7  8  9
15 16 17 18 19 20 21 22 23 24 25  1  2  3  4  5  6  7 -1  9 10 11 12 13 14
20 21 22 23 24 25  1  2  3  4  5  6  7  8  9 10 11 12 13 -1 15 16 17 18 19
25  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 -1 21 22 23 24
```

## Troubleshooting

-   **`SFML/...hpp: No such file or directory`:** Check that `SFML_PATH`
    points to the extracted SFML folder and that its `include` directory
    exists.
-   **Undefined reference / linker errors:** Check the `-L` path,
    library names, compiler compatibility, and that the SFML libraries
    are present in `lib`.
-   **Missing DLL error on Windows:** Put the required SFML DLL files
    from SFML's `bin` directory beside the executable, or add that
    directory to `PATH`.
-   **Launcher opens but a game/solver does not:** Confirm both
    application executables have been built and that their names and
    locations match the paths used in `launcher.cpp`.
-   **Executable is not found:** Run the command from the project
    directory and check that the `.exe` file was created successfully.

## Notes

-   Compile all project files with the same compiler/toolchain used to
    build SFML.
-   If you change the location of SFML, update the include and library
    paths in the commands.
-   If your source code uses a different SFML version, install the
    matching version and adjust the paths accordingly.
