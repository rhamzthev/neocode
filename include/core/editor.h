#ifndef EDITOR_H
#define EDITOR_H

#include <stddef.h>
#include "terminal.h"

/**
 * Editor Module
 * 
 * Central component that coordinates all editor operations and state.
 * Acts as a facade for main.c and mediates between Buffer and Viewport.
 * Coordinates commands, file operations, and user input handling.
 */

// Forward declarations to avoid including headers directly
struct Buffer;
struct Viewport;
typedef struct Buffer Buffer;
typedef struct Viewport Viewport;

/**
 * Editor state structure
 * Contains all data needed to manage the editing session
 */
typedef struct EditorState {
    Buffer* buffer;      // Text content being edited
    Viewport* viewport;  // View of the content
    char* filename;      // Current file being edited
    int dirty;           // Whether content has been modified
    size_t rows;         // Terminal row count
    size_t cols;         // Terminal column count
} EditorState;

// Editor lifecycle
/**
 * Initialize editor with optional file
 * @param filename File to open or NULL for empty buffer
 * @param rows Terminal height
 * @param cols Terminal width
 * @return New editor state or NULL on error
 */
EditorState* editor_init(const char* filename, size_t rows, size_t cols);

/**
 * Free editor resources
 * @param state Editor state to free
 */
void editor_free(EditorState* state);

/**
 * Run editor main loop
 * @param state Editor state
 * @return 0 on success, non-zero on error
 */
int editor_run(EditorState* state);

// File operations
/**
 * Open a file in the editor
 * @param state Editor state
 * @param filename Path to file
 * @return 1 on success, 0 on failure
 */
int editor_open_file(EditorState* state, const char* filename);

/**
 * Save current buffer to file
 * @param state Editor state
 * @param filename Path to save to (NULL to use current filename)
 * @return 1 on success, 0 on failure
 */
int editor_save_file(EditorState* state, const char* filename);

// Content manipulation
/**
 * Insert text at cursor position
 * @param state Editor state
 * @param text Text to insert
 */
void editor_insert_text(EditorState* state, const char* text);

/**
 * Delete text at cursor position
 * @param state Editor state
 * @param amount Amount of characters to delete
 */
void editor_delete_text(EditorState* state, size_t amount);

/**
 * Insert a new line at cursor position
 * @param state Editor state
 */
void editor_insert_newline(EditorState* state);

// Input handling
/**
 * Process keyboard input
 * @param state Editor state
 * @param key Key code from terminal
 * @return 0 to continue, non-zero to exit editor
 */
int editor_process_key(EditorState* state, int key);

/**
 * Process mouse event
 * @param state Editor state
 * @param event Mouse event from terminal
 */
void editor_process_mouse(EditorState* state, MouseEvent event);

/**
 * Handle terminal resize
 * @param state Editor state
 * @param rows New row count
 * @param cols New column count
 */
void editor_resize(EditorState* state, size_t rows, size_t cols);

// Terminal management functions
/**
 * Initialize terminal and get dimensions
 * @param rows Output parameter for terminal height
 * @param cols Output parameter for terminal width
 */
void editor_initialize_terminal(size_t* rows, size_t* cols);

/**
 * Clean up terminal resources
 */
void editor_cleanup_terminal(void);

/**
 * Show welcome screen and wait for quit key
 * @param rows Terminal height
 * @param cols Terminal width
 */
void editor_show_welcome_screen(size_t rows, size_t cols);

/**
 * Check if a file is valid for editing
 * @param filename Path to file
 * @return 1 if file is valid, 0 otherwise
 */
int editor_validate_file(const char* filename);

// State access
/**
 * Get buffer content as a string
 * @param state Editor state
 * @return Buffer content (caller must free)
 */
char* editor_get_content(EditorState* state);

/**
 * Get cursor position
 * @param state Editor state
 * @param x Output parameter for cursor column
 * @param y Output parameter for cursor row
 */
void editor_get_cursor_position(EditorState* state, size_t* x, size_t* y);

/**
 * Set cursor position
 * @param state Editor state
 * @param x New cursor column
 * @param y New cursor row
 */
void editor_set_cursor_position(EditorState* state, size_t x, size_t y);

/**
 * Get editor dimensions
 * @param state Editor state
 * @param rows Output parameter for row count
 * @param cols Output parameter for column count
 */
void editor_get_dimensions(EditorState* state, size_t* rows, size_t* cols);

/**
 * Check if buffer has unsaved changes
 * @param state Editor state
 * @return 1 if buffer is modified, 0 otherwise
 */
int editor_is_dirty(EditorState* state);

/**
 * Get current filename
 * @param state Editor state
 * @return Filename or NULL if no file is open
 */
const char* editor_get_filename(EditorState* state);

/**
 * Refresh viewport to reflect buffer changes
 * @param state Editor state
 */
void editor_refresh_view(EditorState* state);

#endif // EDITOR_H