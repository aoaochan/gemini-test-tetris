# gemini-test-tetris

This repository is for testing the gemini-cli.

## Project Overview

- **Project:** Console Tetris Game
- **Language:** C (C23 Standard)
- **Platform:** Windows (uses `windows.h` and `conio.h`)
- **Build System:** Meson
- **Main Source File:** `main.c`
- **Output Executable:** `asdf.exe` (located in the `builddir` directory)

## Development Workflow

- **To compile the project:**
  ```bash
  meson compile -C builddir
  ```
- **To run the game:**
  ```bash
  ./builddir/asdf
  ```
- **To run tests (if any):**
  ```bash
  meson test -C builddir
  ```

## Game Controls
- **A:** Move Left
- **D:** Move Right
- **S:** Move Down
- **W:** Rotate
- **Q:** Hard Drop