#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "buffer.h"
#include "viewport.h"
#include "ui.h"
#include "commands.h"
#include "io.h"
#include "terminal.h"

// Create and initialize editor state
EditorState* editor_init(const char* filename, size_t rows, size_t cols) {
    EditorState* state = malloc(sizeof(EditorState));
    if (!state) return NULL;
    
    // Initialize state
    state->filename = filename ? strdup(filename) : NULL;
    state->buffer = NULL;
    state->viewport = NULL;
    state->dirty = 0;
    state->rows = rows;
    state->cols = cols;
    
    // Load file into buffer if provided
    if (filename) {
        if (!editor_open_file(state, filename)) {
            free(state->filename);
            free(state);
            return NULL;
        }
    } else {
        // Create empty buffer
        state->buffer = buffer_create("");
        if (!state->buffer) {
            free(state->filename);
            free(state);
            return NULL;
        }
        
        // Create viewport
        state->viewport = viewport_create(state, rows, cols);
        if (!state->viewport) {
            buffer_free(state->buffer);
            free(state->filename);
            free(state);
            return NULL;
        }
    }
    
    return state;
}

// Free editor resources
void editor_free(EditorState* state) {
    if (!state) return;
    
    if (state->viewport) {
        viewport_free(state->viewport);
    }
    
    if (state->buffer) {
        buffer_free(state->buffer);
    }
    
    free(state->filename);
    free(state);
}

// Run editor main loop
int editor_run(EditorState* state) {
    if (!state) return -1;
    
    // Initial render
    ui_render(state);
    
    // Main input loop
    while (1) {
        // Check for terminal size changes
        size_t new_rows, new_cols;
        terminal_get_size(&new_rows, &new_cols);
        
        if (new_rows != state->rows || new_cols != state->cols) {
            editor_resize(state, new_rows, new_cols);
        }
        
        // Read input event (key or mouse)
        InputEvent event = terminal_read_event();
        
        switch (event.type) {
            case EVENT_KEY:
                if (terminal_is_quit(event.key)) {
                    return 0;  // Exit the editor
                }
                if (editor_process_key(state, event.key)) {
                    return 0;  // Exit signaled by key handler
                }
                break;
                
            case EVENT_MOUSE:
                editor_process_mouse(state, event.mouse);
                break;
                
            default:
                break;
        }
    }
    
    return 0;
}

// File operations
int editor_open_file(EditorState* state, const char* filename) {
    if (!state || !filename) return 0;
    
    // Check if file exists
    ObjectType type = validate_object(filename);
    if (type != OBJECT_FILE) {
        return 0;
    }
    
    // Read file content
    size_t size;
    char* content = io_read_file(filename, &size);
    if (!content) {
        return 0;
    }
    
    // Create new buffer
    Buffer* new_buffer = buffer_create(content);
    free(content); // buffer_create makes a copy
    
    if (!new_buffer) {
        return 0;
    }
    
    // Clean up existing resources
    if (state->buffer) {
        buffer_free(state->buffer);
    }
    
    if (state->viewport) {
        viewport_free(state->viewport);
    }
    
    if (state->filename) {
        free(state->filename);
    }
    
    // Update state
    state->buffer = new_buffer;
    state->filename = strdup(filename);
    state->dirty = 0;
    
    // Create viewport for the buffer
    state->viewport = viewport_create(state, state->rows, state->cols);
    if (!state->viewport) {
        buffer_free(state->buffer);
        state->buffer = NULL;
        free(state->filename);
        state->filename = NULL;
        return 0;
    }
    
    return 1;
}

int editor_save_file(EditorState* state, const char* filename) {
    if (!state || !state->buffer) return 0;
    
    const char* save_filename = filename ? filename : state->filename;
    if (!save_filename) {
        return 0; // No filename specified
    }
    
    // Get buffer content
    char* content = buffer_get_content(state->buffer);
    if (!content) {
        return 0;
    }
    
    // Write to file
    size_t size = buffer_size(state->buffer);
    int success = io_write_file(save_filename, content, size);
    free(content);
    
    if (success) {
        // If using a new filename, update state
        if (filename && (!state->filename || strcmp(filename, state->filename) != 0)) {
            free(state->filename);
            state->filename = strdup(filename);
        }
        
        // Mark buffer as no longer modified
        buffer_set_modified(state->buffer, 0);
        state->dirty = 0;
    }
    
    return success;
}

// Content manipulation
void editor_insert_text(EditorState* state, const char* text) {
    if (!state || !state->buffer || !state->viewport || !text) return;
    
    // Calculate buffer position from cursor
    size_t buffer_pos = viewport_screen_to_buffer_pos(
        state->viewport, 
        state->viewport->cursor_x, 
        state->viewport->cursor_y
    );
    
    // Insert text at buffer position
    buffer_insert(state->buffer, buffer_pos, text);
    state->dirty = 1;
    
    // Update cursor position if needed
    size_t len = strlen(text);
    for (size_t i = 0; i < len; i++) {
        if (text[i] == '\n') {
            viewport_move_cursor(state->viewport, 0, 1);
            viewport_set_cursor(state->viewport, 0, state->viewport->cursor_y);
        } else {
            viewport_move_cursor(state->viewport, 1, 0);
        }
    }
    
    // Refresh viewport to reflect changes
    editor_refresh_view(state);
}

void editor_delete_text(EditorState* state, size_t amount) {
    if (!state || !state->buffer || !state->viewport || amount == 0) return;
    
    // Calculate buffer position from cursor
    size_t buffer_pos = viewport_screen_to_buffer_pos(
        state->viewport, 
        state->viewport->cursor_x, 
        state->viewport->cursor_y
    );
    
    // Don't try to delete past the beginning
    if (buffer_pos < amount) {
        amount = buffer_pos;
    }
    
    // Delete text from buffer
    buffer_delete(state->buffer, buffer_pos - amount, amount);
    state->dirty = buffer_is_modified(state->buffer);
    
    // Update cursor position
    for (size_t i = 0; i < amount; i++) {
        viewport_move_cursor(state->viewport, -1, 0);
    }
    
    // Refresh viewport to reflect changes
    editor_refresh_view(state);
}

void editor_insert_newline(EditorState* state) {
    if (!state) return;
    editor_insert_text(state, "\n");
}

// Input handling
int editor_process_key(EditorState* state, int key) {
    if (!state || !state->viewport) return 0;
    
    switch (key) {
        // Basic cursor movement (arrow keys)
        case KEY_ARROW_UP:
            viewport_move_cursor(state->viewport, 0, -1);
            ui_render(state);
            break;
        case KEY_ARROW_DOWN:
            viewport_move_cursor(state->viewport, 0, 1);
            ui_render(state);
            break;
        case KEY_ARROW_LEFT:
            viewport_move_cursor(state->viewport, -1, 0);
            ui_render(state);
            break;
        case KEY_ARROW_RIGHT:
            viewport_move_cursor(state->viewport, 1, 0);
            ui_render(state);
            break;
            
        // Line navigation
        case KEY_HOME:
            cmd_move_to_start_of_line(state);
            break;
        case KEY_END:
            cmd_move_to_end_of_line(state);
            break;
            
        // Document navigation
        case KEY_PAGE_UP:
            cmd_page_move(state, -1);
            break;
        case KEY_PAGE_DOWN:
            cmd_page_move(state, 1);
            break;
            
        // Word navigation
        case KEY_WORD_LEFT:
            cmd_move_word(state, -1);
            break;
        case KEY_WORD_RIGHT:
            cmd_move_word(state, 1);
            break;
        
        // Document start/end
        case KEY_CTRL_HOME:
            cmd_move_to_start_of_document(state);
            break;
        case KEY_CTRL_END:
            cmd_move_to_end_of_document(state);
            break;
            
        // Editing operations
        case KEY_ENTER:
            editor_insert_newline(state);
            break;
        case KEY_BACKSPACE:
        case KEY_CTRL_H:
            editor_delete_text(state, 1);
            break;
        case KEY_DELETE:
            // Delete character at cursor position
            if (state->viewport->cursor_x < viewport_line_length(state->viewport, state->viewport->cursor_y)) {
                viewport_move_cursor(state->viewport, 1, 0);
                editor_delete_text(state, 1);
            } else if (state->viewport->cursor_y < state->viewport->total_lines - 1) {
                // Delete newline by joining lines
                editor_delete_text(state, 1);
            }
            break;
            
        // Handle printable characters
        default:
            if (key >= 32 && key < 127) {
                char c = (char)key;
                editor_insert_text(state, &c);
            }
            break;
    }
    
    return 0; // Continue editing
}

void editor_process_mouse(EditorState* state, MouseEvent event) {
    cmd_process_mouse_event(state, event);
}

void editor_resize(EditorState* state, size_t rows, size_t cols) {
    if (!state) return;
    
    state->rows = rows;
    state->cols = cols;
    
    if (state->viewport) {
        viewport_resize(state->viewport, rows, cols);
        ui_render(state);
    }
}

// State access
char* editor_get_content(EditorState* state) {
    if (!state || !state->buffer) return NULL;
    return buffer_get_content(state->buffer);
}

void editor_get_cursor_position(EditorState* state, size_t* x, size_t* y) {
    if (!state || !state->viewport) {
        if (x) *x = 0;
        if (y) *y = 0;
        return;
    }
    
    if (x) *x = state->viewport->cursor_x;
    if (y) *y = state->viewport->cursor_y;
}

void editor_set_cursor_position(EditorState* state, size_t x, size_t y) {
    if (!state || !state->viewport) return;
    viewport_set_cursor(state->viewport, x, y);
}

void editor_get_dimensions(EditorState* state, size_t* rows, size_t* cols) {
    if (!state) {
        if (rows) *rows = 0;
        if (cols) *cols = 0;
        return;
    }
    
    if (rows) *rows = state->rows;
    if (cols) *cols = state->cols;
}

int editor_is_dirty(EditorState* state) {
    if (!state) return 0;
    
    // Check buffer modified state if available
    if (state->buffer) {
        state->dirty = buffer_is_modified(state->buffer);
    }
    
    return state->dirty;
}

const char* editor_get_filename(EditorState* state) {
    return state ? state->filename : NULL;
}

void editor_refresh_view(EditorState* state) {
    if (!state || !state->viewport) return;
    
    viewport_refresh_cache(state->viewport);
    ui_render(state);
}