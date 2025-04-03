# Neocode Editor

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/username/ncode)
[![Version](https://img.shields.io/badge/version-0.1.0-orange.svg)](https://github.com/username/ncode/releases)

## Description

Neocode is a lightweight, terminal-based text editor written in C that focuses on efficiency and minimal resource usage. It uses a piece table data structure for text manipulation, providing efficient editing operations even for large files. Designed with a clean, modular architecture, Neocode offers a responsive editing experience with essential features for developers and writers who prefer working in terminal environments.

![Screenshot from 2025-04-03 11-40-03](https://github.com/user-attachments/assets/b07a51b8-462a-471a-b33e-6ea2e9462843)


## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Technologies](#technologies)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)
- [Status](#status)

## Installation

### Prerequisites

```
- GCC or compatible C compiler
- Make build system
- POSIX-compliant operating system (Linux, macOS, etc.)
```

### Dependencies

```
- Standard C libraries only (no external dependencies)
```

### Installation Steps

```bash
# Step 1: Clone the repository
git clone https://github.com/username/neocode.git

# Step 2: Navigate to the project directory
cd neocode

# Step 3: Build the project
make

# Step 4: Run the editor
./bin/ncode [filename]
```

### Platform-Specific Instructions

#### Linux
The editor should build and run on most Linux distributions without modification.

#### macOS
Compatible with macOS. Build using the standard steps above.

#### Windows
Not directly supported. Consider using WSL (Windows Subsystem for Linux) for Windows compatibility.

### Troubleshooting

If you encounter build errors, ensure your compiler supports C99 standards and that you have proper permissions to create files in the bin/ and obj/ directories.

## Usage

### Basic Example

```bash
# Open a file for editing
./bin/ncode myfile.txt

# Create a new file (will be saved when you exit)
./bin/ncode newfile.txt
```

### Navigation Controls

- **Arrow Keys**: Move cursor
- **Page Up/Down**: Scroll page by page
- **Home/End**: Move to start/end of line
- **Ctrl+Home/End**: Move to start/end of document
- **Ctrl+Left/Right**: Move by word
- **Mouse**: Click to position cursor, wheel to scroll

### Quitting

- **Ctrl+Q**: Quit the editor

## Features

### Core Functionality
- Efficient text manipulation using piece table data structure
- Terminal-based user interface with minimal resource usage
- File loading and saving
- Mouse support for navigation and cursor positioning

### Key Highlights
- Line numbers for easy reference
- Status bar showing filename, modification status, and cursor position
- Scrollbar for easier navigation in long files
- Minimal memory footprint even for large files
- Responsive editing experience

### Roadmap
- Text editing and modification
- Search and replace functionality
- Syntax highlighting for common programming languages
- Multiple file editing with tabs or buffers
- Configuration system for key bindings and preferences

## Technologies

### Tech Stack
- **C**: Core programming language
- **POSIX APIs**: Terminal handling and file operations
- **Piece Table**: Efficient text storage and manipulation
- **Make**: Build system

### Project Structure
```
neocode/
├── bin/             # Compiled binary
├── include/         # Header files
│   ├── core/        # Core functionality
│   ├── io/          # Input/output operations
│   ├── ui/          # User interface
│   └── utils/       # Utility functions
├── obj/             # Object files
├── src/             # Source code
│   ├── core/        # Core implementation
│   ├── io/          # I/O implementation
│   ├── ui/          # UI implementation
│   ├── utils/       # Utilities implementation
│   └── main.c       # Entry point
├── .gitignore       # Git ignore file
├── Makefile         # Build configuration
└── README.md        # Project documentation
```

### Architecture
Neocode uses a modular MVC-inspired architecture:
- **Model**: Buffer (piece table) for text storage and manipulation
- **View**: Viewport and UI for displaying and rendering content
- **Controller**: Editor and Commands for handling user input and operations

## Contributing

### How to Submit Issues
Please submit issues through the GitHub issue tracker with a clear description of the problem, expected behavior, and steps to reproduce.

### Pull Request Process
1. Fork the repository and create a feature branch
2. Make your changes with appropriate tests
3. Ensure code follows the project's coding standards
4. Submit a pull request with a clear description of changes

### Coding Standards
- Follow C99 standards
- Use consistent indentation (4 spaces)
- Include comments for non-obvious code sections
- Keep functions small and focused on a single responsibility

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Status

- **Current Version**: v0.1.0
- **Status**: Active Development
- **Last Updated**: April 2025

### Release Notes
Initial alpha release featuring file viewing capabilities, navigation, and the core architecture. Editing capabilities are planned for the next release.

---

Built by Rhamsez Thevenin 🌹
