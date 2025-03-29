#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "buffer.h"
#include "viewport.h"
#include "ui.h"
#include "commands.h"
#include "io.h"

// Create and initialize editor state
EditorState* editor_init(const char* filename, size_t rows, size_t cols) {
    EditorState* state = malloc(sizeof(EditorState));
    if (!state) return NULL;
    
    state->filename = filename ? strdup(filename) : NULL;
    state->dirty = 0;
    state->viewport = NULL;
    
    // Load file into buffer if provided
    if (filename) {
        Buffer* buffer = load_file_to_buffer(filename);
        if (!buffer) {
            free(state->filename);
            free(state);
            return NULL;
        }
        
        // Create viewport for the buffer
        state->viewport = viewport_create(buffer, rows, cols);
        if (!state->viewport) {
            buffer_free(buffer);
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
        Buffer* buffer = state->viewport->buffer;
        viewport_free(state->viewport);
        if (buffer) buffer_free(buffer);
    }
    
    free(state->filename);
    free(state);
}

// Editor state queries
size_t editor_get_rows(EditorState* state) {
    return state->viewport ? state->viewport->screen_rows : 0;
}

size_t editor_get_cols(EditorState* state) {
    return state->viewport ? state->viewport->screen_cols : 0;
}

const char* editor_get_filename(EditorState* state) {
    return state->filename;
}

int editor_is_dirty(EditorState* state) {
    return state->dirty;
}

// Process keyboard input
void editor_process_key(EditorState* state, int key) {
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
    }
}

// Process mouse events
void editor_process_mouse(EditorState* state, MouseEvent event) {
    cmd_process_mouse_event(state, event);
}