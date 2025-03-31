#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "terminal.h"
#include "editor.h"
#include "io.h"
#include "ui.h"

/**
 * Main entry point for the editor
 * Handles initialization, command line arguments, and delegates to editor component
 */
int main(int argc, char *argv[]) {
    // Initialize terminal
    terminal_init();
    
    // Get terminal dimensions
    size_t rows, cols;
    terminal_get_size(&rows, &cols);
    
    if (argc < 2) {
        // No file specified, show welcome screen
        ui_welcome_screen(rows, cols);
        
        // Wait for user to press Ctrl+Q to quit
        while (1) {
            char c = terminal_read_char();
            if (terminal_is_quit(c)) {
                break;
            }
        }
        
        terminal_cleanup();
        return 0;
    }

    // Check if file exists and is regular file
    const char *filename = argv[1];
    if (validate_object(filename) != OBJECT_FILE) {
        terminal_cleanup();
        printf("'%s' is not a regular file\n", filename);
        return 1;
    }

    // Initialize editor with file (will handle file loading, viewport creation etc.)
    EditorState* state = editor_init(filename, rows, cols);
    if (!state) {
        terminal_cleanup();
        fprintf(stderr, "Failed to initialize editor with file: %s\n", filename);
        return 1;
    }
    
    // Run editor main loop (this handles everything through the facade)
    int result = editor_run(state);
    
    // Clean up
    editor_free(state);
    terminal_cleanup();
    
    return result;
}