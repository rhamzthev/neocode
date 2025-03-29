#ifndef EDITOR_H
#define EDITOR_H

#include <stddef.h>
#include "viewport.h"
#include "terminal.h"

// Editor state structure
typedef struct {
    Viewport* viewport;
    char* filename;
    int dirty;  // File modified flag
} EditorState;

// Editor initialization
EditorState* editor_init(const char* filename, size_t rows, size_t cols);
void editor_free(EditorState* state);

// Editor state queries
size_t editor_get_rows(EditorState* state);
size_t editor_get_cols(EditorState* state);
const char* editor_get_filename(EditorState* state);
int editor_is_dirty(EditorState* state);

// Key handling functions
void editor_process_key(EditorState* state, int key);
void editor_process_mouse(EditorState* state, MouseEvent event);

#endif // EDITOR_H