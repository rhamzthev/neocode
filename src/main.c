#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"

/**
 * Main entry point for the editor
 * Handles initialization, command line arguments, and delegates to editor component
 */
int main(int argc, char *argv[]) {
    // Initialize terminal and get dimensions
    size_t rows, cols;
    editor_initialize_terminal(&rows, &cols);
    
    if (argc < 2) {
        // No file specified, show welcome screen
        editor_show_welcome_screen(rows, cols);
        editor_cleanup_terminal();
        return 0;
    }

    // Check if file exists and is regular file
    const char *filename = argv[1];
    if (!editor_validate_file(filename)) {
        editor_cleanup_terminal();
        printf("'%s' is not a regular file\n", filename);
        return 1;
    }

    // Initialize editor with file
    EditorState* state = editor_init(filename, rows, cols);
    if (!state) {
        editor_cleanup_terminal();
        fprintf(stderr, "Failed to initialize editor with file: %s\n", filename);
        return 1;
    }
    
    // Run editor main loop
    int result = editor_run(state);
    
    // Clean up
    editor_free(state);
    editor_cleanup_terminal();
    
    return result;
}