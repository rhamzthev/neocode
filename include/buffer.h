#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

// Piece types
typedef enum {
    ORIGINAL,    // Piece points to original file content
    ADD         // Piece points to added content
} PieceType;

// Structure for a piece
typedef struct Piece {
    PieceType type;     // Type of the piece
    size_t start;       // Start offset in the buffer
    size_t length;      // Length of the piece
    struct Piece* next; // Next piece in the list
} Piece;

// Structure for the piece table
typedef struct {
    char* original;     // Original file content
    char* add;          // Buffer for additions
    size_t add_size;    // Current size of add buffer
    size_t add_capacity;// Capacity of add buffer
    Piece* head;        // First piece in the list
} Buffer;

// Buffer operations
Buffer* buffer_create(const char* content);
void buffer_free(Buffer* buffer);
void buffer_insert(Buffer* buffer, size_t pos, const char* text);
void buffer_delete(Buffer* buffer, size_t pos, size_t length);
char* buffer_get_content(Buffer* buffer);
size_t buffer_size(Buffer* buffer);

#endif // BUFFER_H