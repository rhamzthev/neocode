#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

/**
 * Buffer Module
 * 
 * Provides text storage and manipulation using a piece table data structure.
 * This component has no dependencies on other modules and represents the Model
 * in the MVC architecture.
 */

// Piece types
typedef enum {
    ORIGINAL,    // Piece points to original file content
    ADD          // Piece points to added content
} PieceType;

// Forward declaration and typedef for Piece
struct Piece;
typedef struct Piece Piece;

// Structure for a piece
struct Piece {
    PieceType type;     // Type of the piece
    size_t start;       // Start offset in the buffer
    size_t length;      // Length of the piece
    Piece* next;        // Next piece in the list
};

// Forward declaration and typedef for Buffer
struct Buffer;
typedef struct Buffer Buffer;

// Structure for the piece table
struct Buffer {
    char* original;     // Original file content
    char* add;          // Buffer for additions
    size_t add_size;    // Current size of add buffer
    size_t add_capacity;// Capacity of add buffer
    Piece* head;        // First piece in the list
    int modified;       // Flag indicating if buffer was modified since last save
};

// Buffer lifecycle
Buffer* buffer_create(const char* content);
void buffer_free(Buffer* buffer);

// Content manipulation
void buffer_insert(Buffer* buffer, size_t pos, const char* text);
void buffer_delete(Buffer* buffer, size_t pos, size_t length);

// Content access
char* buffer_get_content(Buffer* buffer);
size_t buffer_size(Buffer* buffer);

// Buffer state
int buffer_is_modified(Buffer* buffer);
void buffer_set_modified(Buffer* buffer, int modified);

#endif // BUFFER_H