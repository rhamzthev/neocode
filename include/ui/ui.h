#ifndef UI_H
#define UI_H

#include <stddef.h>
#include "editor.h"

/**
 * UI Module
 *
 * Handles rendering the editor content and UI elements to the screen.
 * UI interacts with the Editor and Viewport components but does not directly
 * access Buffer or other low-level components.
 */

// UI constants
#define WELCOME_MESSAGE "Welcome to the editor! CTRL-Q to quit."
#define LINE_NUMBER_WIDTH 6       // Width of line number column
#define LINE_NUMBER_PADDING 3     // Padding spaces between line number and content
#define SCROLLBAR_WIDTH 1         // Width of scrollbar in columns

// Color definitions
#define COLOR_RESET       "\x1b[0m"
#define COLOR_LINE_NUM    "\x1b[90m"            // Gray line numbers
#define COLOR_STATUS_BAR  "\x1b[30;47m"         // Black on white
#define COLOR_CURRENT_LINE "\x1b[38;5;236;48;5;255m"  // Subtle highlight
#define COLOR_SCROLLBAR_TRACK "\x1b[48;5;236m"  // Dark gray scrollbar track
#define COLOR_SCROLLBAR_THUMB "\x1b[48;5;248m"  // Light gray scrollbar thumb

/**
 * Screen buffer for double buffering
 * Provides efficient rendering by batching screen updates
 */
typedef struct {
    char* content;       // Buffer content
    size_t size;         // Current size of buffer
    size_t capacity;     // Maximum capacity of buffer
} ScreenBuffer;

// Screen buffer operations
ScreenBuffer* screen_buffer_create(size_t capacity);
void screen_buffer_free(ScreenBuffer* buffer);
void screen_buffer_append(ScreenBuffer* buffer, const char* str);
void screen_buffer_appendf(ScreenBuffer* buffer, const char* format, ...);
void screen_buffer_flush(ScreenBuffer* buffer);
void screen_buffer_clear(ScreenBuffer* buffer);

// UI rendering functions
/**
 * Render the editor content and UI elements
 * @param state Editor state containing viewport and file information
 */
void ui_render(EditorState* state);

/**
 * Display welcome screen when no file is loaded
 * @param rows Terminal row count
 * @param cols Terminal column count
 */
void ui_welcome_screen(size_t rows, size_t cols);

/**
 * Render status bar at the bottom of the screen
 * @param state Editor state
 * @param buffer Screen buffer to write to
 */
void ui_status_bar(EditorState* state, ScreenBuffer* buffer);

/**
 * Render vertical scrollbar
 * @param state Editor state
 * @param buffer Screen buffer to write to
 */
void ui_scrollbar(EditorState* state, ScreenBuffer* buffer);

#endif // UI_H