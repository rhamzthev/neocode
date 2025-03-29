# Neocode

Neocode is a lightweight terminal-based code editor designed to provide a Visual Studio Code-like experience in the terminal. It offers essential editing features with a clean, efficient interface.

## Features

- **Modern Terminal Interface**: Clean UI with line numbers and status bar
- **Mouse Support**: Click to position cursor, scroll to navigate
- **Efficient Navigation**: 
  - Arrow keys for basic movement
  - Home/End for line navigation
  - Page Up/Down for quick scrolling
  - Ctrl+Left/Right for word-by-word movement
  - Ctrl+Home/End to jump to document start/end
- **Piece Table Implementation**: Optimized for memory efficiency and performance
- **Visual Feedback**: Current line highlighting and status information
- **Cross-Platform**: Works on any POSIX-compliant terminal

## Requirements

- GCC or compatible C compiler
- POSIX-compliant operating system (Linux, macOS, BSD, etc.)
- Terminal with ANSI escape sequence support

## Building

You can build Neocode using the provided Makefile:

```bash
make clean && make
```

Or use the convenient build script:

```bash
chmod +x main.sh
./main.sh
```

## Usage

Run Neocode by specifying a file to open:

```bash
./bin/ncode path/to/your/file.c
```

Or use the convenience script:

```bash
./main.sh path/to/your/file.c
```

When no file is specified, Neocode will show a welcome screen.

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
- **Drag**: Select text (future feature)
- **Scroll wheel**: Scroll document

## Project Structure

```
.
├── bin/             # Binary output directory
├── include/         # Header files
├── obj/            # Object files
├── src/            # Source code
│   ├── buffer.c    # Piece table implementation
│   ├── commands.c  # Editor commands
│   ├── editor.c    # Editor state management
│   ├── io.c        # File input/output handling
│   ├── main.c      # Entry point
│   ├── terminal.c  # Terminal handling
│   ├── ui.c        # User interface rendering
│   └── viewport.c  # View management
├── main.sh         # Build and run script
├── Makefile        # Build configuration
└── README.md       # This file
```

## Implementation Details

Neocode is built with a modular architecture:

- **Buffer**: Uses a piece table data structure for efficient text manipulation
- **Viewport**: Manages the visible portion of text and cursor positioning
- **Terminal**: Handles raw terminal mode, input events, and escape sequences
- **UI**: Implements rendering with double-buffering to avoid flicker
- **Commands**: Implements editor commands (navigation, editing)
- **IO**: Handles file operations

## Future Features

- Syntax highlighting for multiple languages
- Text editing operations (insert, delete)
- Multiple buffers/tabs
- Find and replace
- Configuration options
- Extended mouse support (selection)
- Undo/redo functionality

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests.