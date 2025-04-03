# Neocode

A lightweight terminal-based code editor providing a VS Code-like experience in your terminal. Neocode offers essential editing features with a clean, efficient interface for developers who prefer working in terminal environments.

![Screenshot from 2025-04-03 11-40-03](https://github.com/user-attachments/assets/bcb942ff-d5d8-4188-94ca-1d51d684f36c)

## Features

- **Modern Terminal Interface**: Clean UI with line numbers, status bar, and current line highlighting
- **Mouse Support**: Click to position cursor, scroll to navigate
- **Efficient Navigation**: 
  - Arrow keys for basic movement
  - Home/End for line navigation
  - Page Up/Down for quick scrolling
  - Ctrl+Left/Right for word-by-word movement
  - Ctrl+Home/End to jump to document start/end
- **Memory Efficient**: Piece table implementation for optimized text operations
- **Cross-Platform**: Works on any POSIX-compliant terminal environment
- **Visual Feedback**: Highlights current line and shows document status

## Requirements

- GCC or compatible C compiler
- POSIX-compliant operating system (Linux, macOS, BSD)
- Terminal with ANSI escape sequence support

## Installation

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/neocode.git
   cd neocode
   ```

2. Build using the Makefile:
   ```bash
   make clean && make
   ```

3. Alternatively, use the convenience script:
   ```bash
   chmod +x main.sh
   ./main.sh
   ```

### Running Neocode

Open a file with:
```bash
./bin/ncode path/to/your/file.c
```

Or use the convenience script:
```bash
./main.sh path/to/your/file.c
```

## Usage

### Keyboard Controls

- **Navigation**:
  - Arrow keys: Move cursor
  - Home/End: Move to start/end of line
  - PgUp/PgDn: Move up/down by one screen
  - Ctrl+Left/Right: Move by word
  - Ctrl+Home/End: Move to start/end of document
  
- **Editor Control**:
  - Ctrl+Q: Quit editor

### Mouse Controls

- **Click**: Position cursor
- **Scroll wheel**: Scroll document

## Architecture

Neocode is built with a modular architecture:

```
                    +-------+
                    |  UI   |
                    +---+---+
                        |
                        v
                  +-----------+
                  | Viewport  |
                  +-----+-----+
                        ^
                        |
+---------+       +-----v-----+       +--------+
|Terminal |------>|           |<------| Buffer |
+---------+       |  Editor   |       +--------+
                  |           |
+---------+       |           |
|   IO    |------>|           |
+---------+       +-----------+
                        ^
+---------+             |
|Commands |-------------+
+---------+
```

- **Buffer**: Uses a piece table data structure for efficient text manipulation
- **Editor**: Central coordinator that manages application state
- **Viewport**: Manages the visible portion of text and cursor positioning
- **Terminal**: Handles raw terminal mode, input events, and escape sequences
- **UI**: Implements rendering with double-buffering to avoid flicker
- **Commands**: Implements editor commands (navigation, editing)
- **IO**: Handles file operations

## Project Structure

```
.
├── bin/             # Binary output directory
├── include/         # Header files
│   ├── core/        # Core components (buffer, editor)
│   ├── io/          # Input/output handling
│   ├── ui/          # User interface components
│   └── utils/       # Utility functions
├── obj/             # Object files
├── src/             # Source code
│   ├── core/        # Core implementation
│   ├── io/          # IO implementation
│   ├── ui/          # UI implementation
│   ├── utils/       # Utilities implementation
│   └── main.c       # Entry point
├── main.sh          # Build and run script
├── Makefile         # Build configuration
└── README.md        # This file
```

## Development Status

Neocode is in early development. Current implemented features:

- View and navigate files
- Mouse and keyboard navigation
- Line numbers and status bar

### Future Features

- Syntax highlighting for multiple languages
- Text editing operations (insert, delete)
- Multiple buffers/tabs
- Find and replace
- Configuration options
- Extended mouse support (selection)
- Undo/redo functionality

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests.

### Development Setup

1. Fork the repository
2. Clone your fork
3. Make your changes
4. Submit a pull request

## License

This project is open source and available under the MIT License.

## Acknowledgments

- Inspired by editors like VS Code, Vim, and Nano
- Built with a focus on terminal efficiency and modern UI practices

## Contact

- Maintainer: [Rhamsez Thevenin](mailto:rhamzthev@gmail.com)
- Project Repository: [GitHub](https://github.com/rhamzthev/neocode)
