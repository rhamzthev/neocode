#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <stddef.h>
#include "buffer.h"

typedef struct {
    Buffer* buffer;         // Reference to text buffer
    size_t cursor_x;        // Cursor position in current line
    size_t cursor_y;        // Current line number
    size_t scroll_x;        // Horizontal scroll offset
    size_t scroll_y;        // First line shown on screen
    size_t screen_rows;     // Terminal height
    size_t screen_cols;     // Terminal width
    size_t total_lines;     // Total number of lines in buffer
    char* content;          // Content string (owner of memory)
    char** line_cache;      // Cache of line start positions
    size_t line_count;      // Number of lines in cache
} Viewport;

// Viewport management
Viewport* viewport_create(Buffer* buffer, size_t rows, size_t cols);
void viewport_free(Viewport* viewport);
void viewport_resize(Viewport* viewport, size_t rows, size_t cols);

// Cursor movement
void viewport_move_cursor(Viewport* viewport, int dx, int dy);
void viewport_set_cursor(Viewport* viewport, size_t x, size_t y);

// Scrolling
void viewport_scroll(Viewport* viewport, int dx, int dy);
void viewport_ensure_cursor_visible(Viewport* viewport);

// Content access
char* viewport_get_line(Viewport* viewport, size_t line_number);
size_t viewport_line_length(Viewport* viewport, size_t line_number);
void viewport_refresh_cache(Viewport* viewport);

// Screen coordinates
size_t viewport_screen_to_buffer_pos(Viewport* viewport, size_t screen_x, size_t screen_y);
void viewport_buffer_to_screen_pos(Viewport* viewport, size_t buffer_pos, size_t* screen_x, size_t* screen_y);

#endif // VIEWPORT_H