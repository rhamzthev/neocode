#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "terminal.h"
#include "editor.h"
#include "ui.h"

// Get terminal dimensions
void get_terminal_dimensions(size_t* rows, size_t* cols) {
    terminal_get_size(rows, cols);
}

int main(int argc, char *argv[]) {
    // Initialize terminal
    terminal_init();
    
    // Get terminal dimensions
    size_t rows, cols;
    get_terminal_dimensions(&rows, &cols);
    
    if (argc < 2) {
        // No file specified, show welcome screen
        ui_welcome_screen(rows, cols);
        
        // Wait for user input
        while (1) {
            char c = terminal_read_char();
            if (terminal_is_quit(c)) {
                break;
            }
        }
        
        terminal_cleanup();
        return 0;
    }

    const char *filename = argv[1];
    ObjectType type = validate_object(filename);
    
    if (type != OBJECT_FILE) {
        terminal_cleanup();
        printf("'%s' is not a regular file\n", filename);
        return 1;
    }

    // Initialize editor with file
    EditorState* state = editor_init(filename, rows, cols);
    if (!state) {
        terminal_cleanup();
        return 1;
    }
    
    // Initial render
    ui_render(state);

    // Main input loop
    while (1) {
        // Check for terminal size changes
        size_t new_rows, new_cols;
        terminal_get_size(&new_rows, &new_cols);
        
        if (new_rows != editor_get_rows(state) || new_cols != editor_get_cols(state)) {
            viewport_resize(state->viewport, new_rows, new_cols);
            ui_render(state);
        }
        
        // Read input event (key or mouse)
        InputEvent event = terminal_read_event();
        
        switch (event.type) {
            case EVENT_KEY:
                if (terminal_is_quit(event.key)) {
                    goto cleanup;  // Exit the editor
                }
                editor_process_key(state, event.key);
                break;
                
            case EVENT_MOUSE:
                editor_process_mouse(state, event.mouse);
                break;
                
            default:
                break;
        }
    }

cleanup:
    // Clean up
    editor_free(state);
    terminal_cleanup();
    
    return 0;
}