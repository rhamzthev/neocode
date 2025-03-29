#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ui.h"
#include "editor.h"
#include "viewport.h"
#include "terminal.h"

#define INITIAL_BUFFER_SIZE 16384  // 16KB initial buffer size

// Create a new screen buffer
ScreenBuffer* screen_buffer_create(size_t capacity) {
    ScreenBuffer* buffer = malloc(sizeof(ScreenBuffer));
    if (!buffer) return NULL;
    
    buffer->content = malloc(capacity);
    if (!buffer->content) {
        free(buffer);
        return NULL;
    }
    
    buffer->capacity = capacity;
    buffer->size = 0;
    buffer->content[0] = '\0';
    
    return buffer;
}

// Free screen buffer
void screen_buffer_free(ScreenBuffer* buffer) {
    if (!buffer) return;
    free(buffer->content);
    free(buffer);
}

// Ensure buffer has enough capacity
static void screen_buffer_ensure_capacity(ScreenBuffer* buffer, size_t additional) {
    if (buffer->size + additional >= buffer->capacity) {
        size_t new_capacity = buffer->capacity * 2;
        while (buffer->size + additional >= new_capacity) {
            new_capacity *= 2;
        }
        
        char* new_content = realloc(buffer->content, new_capacity);
        if (new_content) {
            buffer->content = new_content;
            buffer->capacity = new_capacity;
        }
    }
}

// Append string to buffer
void screen_buffer_append(ScreenBuffer* buffer, const char* str) {
    size_t len = strlen(str);
    screen_buffer_ensure_capacity(buffer, len + 1);
    
    strcpy(buffer->content + buffer->size, str);
    buffer->size += len;
}

// Append formatted string to buffer
void screen_buffer_appendf(ScreenBuffer* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // First, determine length needed
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    
    if (len < 0) {
        va_end(args);
        return;
    }
    
    // Ensure buffer has enough capacity
    screen_buffer_ensure_capacity(buffer, len + 1);
    
    // Write formatted string to buffer
    vsnprintf(buffer->content + buffer->size, buffer->capacity - buffer->size, format, args);
    buffer->size += len;
    
    va_end(args);
}

// Flush buffer to screen
void screen_buffer_flush(ScreenBuffer* buffer) {
    if (!buffer || buffer->size == 0) return;
    
    fwrite(buffer->content, 1, buffer->size, stdout);
    fflush(stdout);
    buffer->size = 0;
    buffer->content[0] = '\0';
}

// Clear buffer
void screen_buffer_clear(ScreenBuffer* buffer) {
    if (!buffer) return;
    buffer->size = 0;
    buffer->content[0] = '\0';
}

// Draw a welcome message when no file is loaded
void ui_welcome_screen(size_t rows, size_t cols) {
    ScreenBuffer* buffer = screen_buffer_create(INITIAL_BUFFER_SIZE);
    if (!buffer) return;
    
    screen_buffer_append(buffer, TERM_CLEAR_SCREEN);
    screen_buffer_append(buffer, TERM_CURSOR_HOME);

    // Calculate center position
    size_t welcome_len = strlen(WELCOME_MESSAGE);
    size_t padding = cols > welcome_len ? (cols - welcome_len) / 2 : 0;
    size_t row = rows / 3;

    // Position cursor and print welcome message
    screen_buffer_appendf(buffer, CSI "%zu;%zuH", row, padding + 1);
    screen_buffer_append(buffer, WELCOME_MESSAGE);

    screen_buffer_append(buffer, TERM_CURSOR_HOME);
    
    screen_buffer_flush(buffer);
    screen_buffer_free(buffer);
}

// Draw status bar at the bottom of the screen
void ui_status_bar(EditorState* state, ScreenBuffer* buffer) {
    Viewport* viewport = state->viewport;
    
    screen_buffer_append(buffer, COLOR_STATUS_BAR);
    
    // Move cursor to status bar position
    screen_buffer_appendf(buffer, CSI "%zu;1H", viewport->screen_rows);
    
    // Left side: filename and status
    char status[255];
    snprintf(status, sizeof(status), " %.20s %s", 
             state->filename ? state->filename : "[No Name]",
             state->dirty ? "[Modified]" : "");
    
    // Right side: cursor position
    char position[255];
    snprintf(position, sizeof(position), "Ln %zu, Col %zu ", 
             viewport->cursor_y + 1, 
             viewport->cursor_x + 1);
    
    // Print status bar with proper padding
    size_t status_len = strlen(status);
    size_t pos_len = strlen(position);
    
    screen_buffer_append(buffer, status);
    
    // Fill middle with spaces
    for (size_t i = status_len; i < viewport->screen_cols - pos_len; i++) {
        screen_buffer_append(buffer, " ");
    }
    
    screen_buffer_append(buffer, position);
    screen_buffer_append(buffer, COLOR_RESET);
}

// Function to render the buffer content through viewport with line numbers
void ui_render(EditorState* state) {
    Viewport* viewport = state->viewport;
    
    ScreenBuffer* buffer = screen_buffer_create(INITIAL_BUFFER_SIZE);
    if (!buffer) return;
    
    // Use TERM_CURSOR_HOME instead of clearing the screen
    // This reduces flicker by not clearing the entire screen
    screen_buffer_append(buffer, TERM_CURSOR_HOME);
    
    // Calculate visible region
    size_t visible_rows = viewport->screen_rows - 1; // Reserve one line for status bar
    if (visible_rows > viewport->total_lines - viewport->scroll_y) {
        visible_rows = viewport->total_lines - viewport->scroll_y;
    }
    
    // Render each visible line
    for (size_t i = 0; i < visible_rows; i++) {
        size_t line_num = viewport->scroll_y + i;
        
        // Highlight current line
        if (line_num == viewport->cursor_y) {
            screen_buffer_append(buffer, COLOR_CURRENT_LINE);
        }
        
        // Print line number
        screen_buffer_append(buffer, COLOR_LINE_NUM);
        screen_buffer_appendf(buffer, "%*zu", LINE_NUMBER_WIDTH, line_num + 1);

        // Add configurable padding spaces
        for (size_t p = 0; p < LINE_NUMBER_PADDING; p++) {
            screen_buffer_append(buffer, " ");
        }
        
        screen_buffer_append(buffer, COLOR_RESET);
        
        if (line_num == viewport->cursor_y) {
            screen_buffer_append(buffer, COLOR_CURRENT_LINE);
        }
        
        char* line = viewport_get_line(viewport, line_num);
        
        if (line) {
            // Calculate visible segment of the line
            size_t line_len = viewport_line_length(viewport, line_num);
            size_t visible_len = line_len;

            // Account for both line number width AND padding when calculating visible area
            if (visible_len > viewport->screen_cols - (LINE_NUMBER_WIDTH + LINE_NUMBER_PADDING) - viewport->scroll_x) {
                visible_len = viewport->screen_cols - (LINE_NUMBER_WIDTH + LINE_NUMBER_PADDING) - viewport->scroll_x;
            }
            
            // Skip part of line before scroll_x
            if (viewport->scroll_x < line_len) {
                line += viewport->scroll_x;
                
                // Print visible part of the line
                for (size_t j = 0; j < visible_len; j++) {
                    if (line[j] == '\n' || line[j] == '\r') break;
                    screen_buffer_appendf(buffer, "%c", line[j]);
                }
            }
        }
        
        screen_buffer_append(buffer, COLOR_RESET);
        
        // Clear to end of line and move to next line
        screen_buffer_append(buffer, TERM_CLEAR_LINE);
        if (i < visible_rows - 1) {
            screen_buffer_append(buffer, "\r\n");
        }
    }
    
    // Fill any remaining rows with blank lines (removing the tildes)
    for (size_t i = visible_rows; i < viewport->screen_rows - 1; i++) {
        // Just use blank lines for all rows beyond the content
        // No more tildes - just clear each line
        screen_buffer_append(buffer, TERM_CLEAR_LINE);
        
        if (i < viewport->screen_rows - 2) {
            screen_buffer_append(buffer, "\r\n");
        }
    }
    
    // Draw status bar
    ui_status_bar(state, buffer);
    
    // Position cursor
    screen_buffer_appendf(buffer, CSI "%zu;%zuH", 
                         viewport->cursor_y - viewport->scroll_y + 1, 
                         viewport->cursor_x - viewport->scroll_x + LINE_NUMBER_WIDTH + LINE_NUMBER_PADDING + 1);
    
    // Flush the buffer to the screen
    screen_buffer_flush(buffer);
    screen_buffer_free(buffer);
}