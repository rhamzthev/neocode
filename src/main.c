#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For usleep
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
        
        // Non-blocking loop waiting for user to press Ctrl+Q to quit
        int quit = 0;
        while (!quit) {
            char c;
            if (terminal_read_char_nonblock(&c)) {
                if (terminal_is_quit(c)) {
                    quit = 1;
                }
            }
            
            // Add a small sleep to avoid consuming 100% CPU
            usleep(10000); // Sleep for 10ms
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