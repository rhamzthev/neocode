#include <stddef.h>
#include "commands.h"
#include "editor.h"
#include "viewport.h"
#include "ui.h"

// Move cursor to start of current line
void cmd_move_to_start_of_line(EditorState* state) {
    viewport_set_cursor(state->viewport, 0, state->viewport->cursor_y);
    ui_render(state);
}

// Move cursor to end of current line
void cmd_move_to_end_of_line(EditorState* state) {
    size_t line_len = viewport_line_length(state->viewport, state->viewport->cursor_y);
    viewport_set_cursor(state->viewport, line_len, state->viewport->cursor_y);
    ui_render(state);
}

// Move cursor to start of document
void cmd_move_to_start_of_document(EditorState* state) {
    viewport_set_cursor(state->viewport, 0, 0);
    ui_render(state);
}

// Move cursor to end of document
void cmd_move_to_end_of_document(EditorState* state) {
    size_t last_line = state->viewport->total_lines > 0 ? state->viewport->total_lines - 1 : 0;
    size_t last_line_len = viewport_line_length(state->viewport, last_line);
    viewport_set_cursor(state->viewport, last_line_len, last_line);
    ui_render(state);
}

// Move cursor by word
void cmd_move_word(EditorState* state, int direction) {
    Viewport* viewport = state->viewport;
    size_t cursor_x = viewport->cursor_x;
    size_t cursor_y = viewport->cursor_y;
    
    // Get current line
    char* line = viewport_get_line(viewport, cursor_y);
    if (!line) return;
    
    size_t line_len = viewport_line_length(viewport, cursor_y);
    
    if (direction > 0) {  // Move forward one word
        // Skip current word
        while (cursor_x < line_len && (line[cursor_x] != ' ' && line[cursor_x] != '\t' && line[cursor_x] != '\n')) {
            cursor_x++;
        }
        
        // Skip spaces
        while (cursor_x < line_len && (line[cursor_x] == ' ' || line[cursor_x] == '\t')) {
            cursor_x++;
        }
        
        // If we're at the end of line and not last
        if (cursor_x >= line_len && cursor_y < viewport->total_lines - 1) {
            cursor_y++;
            cursor_x = 0;
        }
    } else {  // Move backward one word
        // Skip spaces
        while (cursor_x > 0 && (line[cursor_x - 1] == ' ' || line[cursor_x - 1] == '\t')) {
            cursor_x--;
        }
        
        // Skip current word
        while (cursor_x > 0 && (line[cursor_x - 1] != ' ' && line[cursor_x - 1] != '\t' && line[cursor_x - 1] != '\n')) {
            cursor_x--;
        }
        
        // If we're at the start of line and not first
        if (cursor_x == 0 && cursor_y > 0) {
            cursor_y--;
            line = viewport_get_line(viewport, cursor_y);
            line_len = viewport_line_length(viewport, cursor_y);
            cursor_x = line_len;
        }
    }
    
    viewport_set_cursor(viewport, cursor_x, cursor_y);
    ui_render(state);
}

// Handle page up/down movement
void cmd_page_move(EditorState* state, int direction) {
    Viewport* viewport = state->viewport;
    size_t screen_rows = viewport->screen_rows - 1; // Account for status bar
    
    // Move cursor by screen height
    for (size_t i = 0; i < screen_rows; i++) {
        viewport_move_cursor(viewport, 0, direction);
    }
    
    ui_render(state);
}

// Process a mouse event (clicked position or scrolling)
void cmd_process_mouse_event(EditorState* state, MouseEvent event) {
    Viewport* viewport = state->viewport;
    
    switch (event.type) {
        case MOUSE_PRESS:
        case MOUSE_DRAG:
            // Convert screen position to buffer position, accounting for line numbers and padding
            if (event.x >= LINE_NUMBER_WIDTH + LINE_NUMBER_PADDING) {
                size_t buffer_x = event.x - (LINE_NUMBER_WIDTH + LINE_NUMBER_PADDING) + viewport->scroll_x;
                size_t buffer_y = event.y + viewport->scroll_y;
                
                // Constraint 1: Ensure the line exists in the buffer
                if (buffer_y >= viewport->total_lines) {
                    // Click is below the actual content, do nothing
                    return;
                }
                
                // Get the length of the line at the clicked position
                size_t line_len = viewport_line_length(viewport, buffer_y);
                
                // Constraint 2 & 3: Ensure cursor doesn't go beyond the end of the line
                if (buffer_x > line_len) {
                    buffer_x = line_len;
                }
                
                // Update cursor position
                viewport_set_cursor(viewport, buffer_x, buffer_y);
                ui_render(state);
            }
            break;
            
        case MOUSE_WHEEL_UP:
            // Scroll up (3 lines at a time)
            viewport_scroll(viewport, 0, -3);
            ui_render(state);
            break;
            
        case MOUSE_WHEEL_DOWN:
            // Scroll down (3 lines at a time)
            viewport_scroll(viewport, 0, 3);
            ui_render(state);
            break;
            
        default:
            break;
    }
}