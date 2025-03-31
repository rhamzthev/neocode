#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <stddef.h>

/**
 * Viewport Module
 * 
 * Manages the visible portion of text content and cursor positioning.
 * This component works with Editor but has no direct knowledge of Buffer.
 * It represents part of the View in the MVC architecture.
 */

// Forward declaration of editor state (to avoid circular dependency)
struct EditorState;
typedef struct EditorState EditorState;

// Forward declaration and typedef for Viewport
struct Viewport;
typedef struct Viewport Viewport;

// Viewport structure that manages the view of content
struct Viewport {
    EditorState* editor;   // Reference to editor state for buffer access
    size_t cursor_x;       // Cursor position in current line
    size_t cursor_y;       // Current line number
    size_t desired_x;      // Desired horizontal position when moving vertically
    size_t scroll_x;       // Horizontal scroll offset
    size_t scroll_y;       // First line shown on screen
    size_t screen_rows;    // Terminal height
    size_t screen_cols;    // Terminal width
    size_t total_lines;    // Total number of lines in buffer
    char* content;         // Content string (owner of memory)
    char** line_cache;     // Cache of line start positions
    size_t line_count;     // Number of lines in cache
};

// Viewport lifecycle
/**
 * Create a new viewport for displaying content
 * @param editor_state The editor state containing buffer information
 * @param rows Viewport height in rows
 * @param cols Viewport width in columns
 * @return A new viewport instance
 */
Viewport* viewport_create(EditorState* editor_state, size_t rows, size_t cols);

/**
 * Free resources used by viewport
 * @param viewport Viewport to free
 */
void viewport_free(Viewport* viewport);

/**
 * Update viewport dimensions
 * @param viewport Viewport to resize
 * @param rows New height in rows
 * @param cols New width in columns
 */
void viewport_resize(Viewport* viewport, size_t rows, size_t cols);

// Cursor movement
/**
 * Move cursor by relative offset
 * @param viewport Viewport to update
 * @param dx Horizontal movement (-1 = left, 1 = right)
 * @param dy Vertical movement (-1 = up, 1 = down)
 */
void viewport_move_cursor(Viewport* viewport, int dx, int dy);

/**
 * Set cursor to absolute position
 * @param viewport Viewport to update
 * @param x Absolute column position
 * @param y Absolute row position
 */
void viewport_set_cursor(Viewport* viewport, size_t x, size_t y);

// Scrolling
/**
 * Scroll viewport by relative offset
 * @param viewport Viewport to scroll
 * @param dx Horizontal scroll amount
 * @param dy Vertical scroll amount
 */
void viewport_scroll(Viewport* viewport, int dx, int dy);

/**
 * Adjust scroll position to ensure cursor is visible
 * @param viewport Viewport to update
 */
void viewport_ensure_cursor_visible(Viewport* viewport);

// Content access (through editor)
/**
 * Get line content at specified line number
 * @param viewport Viewport to query
 * @param line_number Line number to retrieve (0-based)
 * @return Pointer to line text or NULL if line doesn't exist
 */
char* viewport_get_line(Viewport* viewport, size_t line_number);

/**
 * Get length of specified line
 * @param viewport Viewport to query
 * @param line_number Line number to check (0-based)
 * @return Length of the line in characters
 */
size_t viewport_line_length(Viewport* viewport, size_t line_number);

/**
 * Refresh the viewport's content cache
 * @param viewport Viewport to refresh
 */
void viewport_refresh_cache(Viewport* viewport);

// Coordinate mapping
/**
 * Convert screen coordinates to buffer position
 * @param viewport Current viewport
 * @param screen_x Screen X coordinate
 * @param screen_y Screen Y coordinate
 * @return Absolute position in buffer
 */
size_t viewport_screen_to_buffer_pos(Viewport* viewport, size_t screen_x, size_t screen_y);

/**
 * Convert buffer position to screen coordinates
 * @param viewport Current viewport
 * @param buffer_pos Position in buffer
 * @param screen_x Output parameter for screen X coordinate
 * @param screen_y Output parameter for screen Y coordinate
 */
void viewport_buffer_to_screen_pos(Viewport* viewport, size_t buffer_pos, size_t* screen_x, size_t* screen_y);

#endif // VIEWPORT_H