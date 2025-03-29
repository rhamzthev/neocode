#include <stdlib.h>
#include <string.h>
#include "viewport.h"

static void update_line_cache(Viewport* viewport) {
    // Free previous content and cache if they exist
    if (viewport->content) {
        free(viewport->content);
    }
    if (viewport->line_cache) {
        free(viewport->line_cache);
    }
    
    // Get fresh content from buffer
    viewport->content = buffer_get_content(viewport->buffer);
    if (!viewport->content) return;

    // Count lines and allocate cache
    size_t capacity = 1024;
    viewport->line_cache = malloc(sizeof(char*) * capacity);
    if (!viewport->line_cache) {
        free(viewport->content);
        viewport->content = NULL;
        return;
    }
    
    viewport->line_count = 0;
    
    // First line always starts at the beginning
    viewport->line_cache[viewport->line_count++] = viewport->content;

    // Find start of each line
    for (char* p = viewport->content; *p; p++) {
        if (*p == '\n') {
            if (viewport->line_count >= capacity) {
                capacity *= 2;
                char** new_cache = realloc(viewport->line_cache, sizeof(char*) * capacity);
                if (!new_cache) break;
                viewport->line_cache = new_cache;
            }
            viewport->line_cache[viewport->line_count++] = p + 1;
        }
    }

    viewport->total_lines = viewport->line_count;
    // Do NOT free content here - we keep it for the lifetime of the viewport
}

Viewport* viewport_create(Buffer* buffer, size_t rows, size_t cols) {
    Viewport* viewport = malloc(sizeof(Viewport));
    if (!viewport) return NULL;

    viewport->buffer = buffer;
    viewport->cursor_x = 0;
    viewport->cursor_y = 0;
    viewport->scroll_x = 0;
    viewport->scroll_y = 0;
    viewport->screen_rows = rows;
    viewport->screen_cols = cols;
    viewport->content = NULL;    // Initialize to NULL
    viewport->line_cache = NULL;
    viewport->line_count = 0;

    update_line_cache(viewport);
    return viewport;
}

void viewport_free(Viewport* viewport) {
    if (!viewport) return;
    if (viewport->content) {
        free(viewport->content);
    }
    if (viewport->line_cache) {
        free(viewport->line_cache);
    }
    free(viewport);
}

void viewport_resize(Viewport* viewport, size_t rows, size_t cols) {
    viewport->screen_rows = rows;
    viewport->screen_cols = cols;
    viewport_ensure_cursor_visible(viewport);
}

void viewport_move_cursor(Viewport* viewport, int dx, int dy) {
    // Calculate new position
    int new_x = (int)viewport->cursor_x + dx;
    int new_y = (int)viewport->cursor_y + dy;

    // Clamp Y position
    if (new_y < 0) new_y = 0;
    if (new_y >= (int)viewport->total_lines) {
        new_y = viewport->total_lines - 1;
    }

    // Clamp X position based on line length
    size_t line_len = viewport_line_length(viewport, new_y);
    if (new_x < 0) new_x = 0;
    if (new_x > (int)line_len) new_x = line_len;

    viewport->cursor_x = new_x;
    viewport->cursor_y = new_y;
    viewport_ensure_cursor_visible(viewport);
}

void viewport_set_cursor(Viewport* viewport, size_t x, size_t y) {
    viewport->cursor_x = x;
    viewport->cursor_y = y;
    viewport_ensure_cursor_visible(viewport);
}

void viewport_scroll(Viewport* viewport, int dx, int dy) {
    // Calculate new scroll position
    int new_scroll_x = (int)viewport->scroll_x + dx;
    int new_scroll_y = (int)viewport->scroll_y + dy;

    // Clamp horizontal scroll (prevent negative scrolling)
    if (new_scroll_x < 0) new_scroll_x = 0;
    
    // Clamp vertical scroll (prevent negative scrolling)
    if (new_scroll_y < 0) new_scroll_y = 0;
    
    // Handle vertical scrolling limits
    if (viewport->total_lines <= viewport->screen_rows - 1) {
        // If content fits entirely in the viewport (accounting for status bar),
        // don't allow any scrolling
        new_scroll_y = 0;
    } else {
        // Allow scrolling until the last line appears at the top of the viewport
        // (subtract 1 to convert from count to index)
        size_t max_scroll_y = viewport->total_lines - 1;
        if (new_scroll_y > (int)max_scroll_y) {
            new_scroll_y = (int)max_scroll_y;
        }
    }

    viewport->scroll_x = new_scroll_x;
    viewport->scroll_y = new_scroll_y;
}

void viewport_ensure_cursor_visible(Viewport* viewport) {
    // Account for status bar in available rows
    size_t visible_rows = viewport->screen_rows - 1;
    
    // Vertical scrolling - ensure cursor is visible
    if (viewport->cursor_y < viewport->scroll_y) {
        // Cursor is above viewport, scroll up
        viewport->scroll_y = viewport->cursor_y;
    } else if (viewport->cursor_y >= viewport->scroll_y + visible_rows) {
        // Cursor is below viewport, scroll down
        viewport->scroll_y = viewport->cursor_y - visible_rows + 1;
    }

    // Horizontal scrolling
    if (viewport->cursor_x < viewport->scroll_x) {
        viewport->scroll_x = viewport->cursor_x;
    } else if (viewport->cursor_x >= viewport->scroll_x + viewport->screen_cols) {
        viewport->scroll_x = viewport->cursor_x - viewport->screen_cols + 1;
    }
    
    // Double-check bounds to ensure we respect file boundaries
    if (viewport->scroll_y < 0) {
        viewport->scroll_y = 0;
    }
    
    // Make sure we don't scroll past the last possible position
    if (viewport->total_lines > 0) {
        // Allow scrolling until the last line is the first visible line
        size_t max_scroll_y = viewport->total_lines - 1;
        if (viewport->scroll_y > max_scroll_y) {
            viewport->scroll_y = max_scroll_y;
        }
    } else {
        viewport->scroll_y = 0;
    }
    
    if (viewport->scroll_x < 0) {
        viewport->scroll_x = 0;
    }
}

char* viewport_get_line(Viewport* viewport, size_t line_number) {
    if (line_number >= viewport->line_count) return NULL;
    return viewport->line_cache[line_number];
}

size_t viewport_line_length(Viewport* viewport, size_t line_number) {
    if (line_number >= viewport->line_count) return 0;
    
    char* line_start = viewport->line_cache[line_number];
    char* line_end;
    
    if (line_number + 1 < viewport->line_count) {
        line_end = viewport->line_cache[line_number + 1] - 1; // Before the newline
    } else {
        line_end = line_start + strlen(line_start);
    }
    
    return line_end - line_start;
}

void viewport_refresh_cache(Viewport* viewport) {
    update_line_cache(viewport);
}

size_t viewport_screen_to_buffer_pos(Viewport* viewport, size_t screen_x, size_t screen_y) {
    size_t file_y = screen_y + viewport->scroll_y;
    if (file_y >= viewport->line_count) return 0;

    char* line = viewport_get_line(viewport, file_y);
    size_t pos = line - viewport->line_cache[0]; // Offset from start of buffer
    return pos + screen_x + viewport->scroll_x;
}

void viewport_buffer_to_screen_pos(Viewport* viewport, size_t buffer_pos, size_t* screen_x, size_t* screen_y) {
    // Find the line containing the buffer position
    size_t line = 0;
    while (line + 1 < viewport->line_count && 
           buffer_pos >= (size_t)(viewport->line_cache[line + 1] - viewport->line_cache[0])) {
        line++;
    }

    // Calculate screen coordinates
    *screen_y = line - viewport->scroll_y;
    *screen_x = buffer_pos - (viewport->line_cache[line] - viewport->line_cache[0]) - viewport->scroll_x;
}