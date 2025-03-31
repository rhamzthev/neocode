#include <stdlib.h>
#include <string.h>
#include "buffer.h"

#define INITIAL_ADD_CAPACITY 1024

static Piece* create_piece(PieceType type, size_t start, size_t length) {
    Piece* piece = malloc(sizeof(Piece));
    if (piece) {
        piece->type = type;
        piece->start = start;
        piece->length = length;
        piece->next = NULL;
    }
    return piece;
}

Buffer* buffer_create(const char* content) {
    Buffer* buffer = malloc(sizeof(Buffer));
    if (!buffer) return NULL;

    size_t content_len = strlen(content);
    
    buffer->original = strdup(content);
    buffer->add = malloc(INITIAL_ADD_CAPACITY);
    buffer->add_size = 0;
    buffer->add_capacity = INITIAL_ADD_CAPACITY;
    buffer->modified = 0; // Initialize modified flag to false
    
    if (!buffer->original || !buffer->add) {
        buffer_free(buffer);
        return NULL;
    }

    // Create initial piece pointing to entire original content
    buffer->head = create_piece(ORIGINAL, 0, content_len);
    if (!buffer->head) {
        buffer_free(buffer);
        return NULL;
    }

    return buffer;
}

void buffer_free(Buffer* buffer) {
    if (!buffer) return;

    // Free all pieces
    Piece* current = buffer->head;
    while (current) {
        Piece* next = current->next;
        free(current);
        current = next;
    }

    free(buffer->original);
    free(buffer->add);
    free(buffer);
}

static void ensure_add_capacity(Buffer* buffer, size_t additional) {
    if (buffer->add_size + additional > buffer->add_capacity) {
        size_t new_capacity = buffer->add_capacity * 2;
        while (buffer->add_size + additional > new_capacity) {
            new_capacity *= 2;
        }
        
        char* new_add = realloc(buffer->add, new_capacity);
        if (new_add) {
            buffer->add = new_add;
            buffer->add_capacity = new_capacity;
        }
    }
}

void buffer_insert(Buffer* buffer, size_t pos, const char* text) {
    if (!buffer || !text) return;

    size_t text_len = strlen(text);
    if (text_len == 0) return;

    // Find the piece and offset where we need to insert
    Piece* current = buffer->head;
    size_t current_pos = 0;
    
    while (current && current_pos + current->length < pos) {
        current_pos += current->length;
        current = current->next;
    }

    if (!current) return; // Position out of bounds

    // Ensure we have enough space in add buffer
    ensure_add_capacity(buffer, text_len);

    // Copy new text to add buffer
    memcpy(buffer->add + buffer->add_size, text, text_len);
    
    // Split current piece if necessary
    size_t split_offset = pos - current_pos;
    Piece* new_piece = create_piece(ADD, buffer->add_size, text_len);
    
    if (split_offset > 0) {
        Piece* second_half = create_piece(current->type, 
                                        current->start + split_offset,
                                        current->length - split_offset);
        current->length = split_offset;
        new_piece->next = second_half;
        second_half->next = current->next;
    } else {
        new_piece->next = current->next;
    }
    
    current->next = new_piece;
    buffer->add_size += text_len;
    
    // Set the modified flag
    buffer->modified = 1;
}

char* buffer_get_content(Buffer* buffer) {
    if (!buffer) return NULL;

    // Calculate total size
    size_t total_size = buffer_size(buffer);
    char* result = malloc(total_size + 1);
    if (!result) return NULL;

    // Copy all pieces to result
    size_t pos = 0;
    Piece* current = buffer->head;
    
    while (current) {
        const char* source = (current->type == ORIGINAL) ? 
                            buffer->original : buffer->add;
        memcpy(result + pos, source + current->start, current->length);
        pos += current->length;
        current = current->next;
    }
    
    result[total_size] = '\0';
    return result;
}

size_t buffer_size(Buffer* buffer) {
    if (!buffer) return 0;

    size_t size = 0;
    Piece* current = buffer->head;
    
    while (current) {
        size += current->length;
        current = current->next;
    }
    
    return size;
}

void buffer_delete(Buffer* buffer, size_t pos, size_t length) {
    if (!buffer || length == 0) return;

    // Find the piece containing the start position
    Piece* current = buffer->head;
    size_t current_pos = 0;
    
    while (current && current_pos + current->length <= pos) {
        current_pos += current->length;
        current = current->next;
    }

    if (!current) return; // Position out of bounds

    // Handle deletion that spans multiple pieces
    size_t remaining = length;
    size_t delete_start = pos - current_pos;

    while (current && remaining > 0) {
        size_t delete_amount = current->length - delete_start;
        if (delete_amount > remaining) delete_amount = remaining;

        if (delete_start == 0 && delete_amount == current->length) {
            // Remove entire piece
            Piece* next = current->next;
            if (current == buffer->head) {
                buffer->head = next;
            } else {
                // Find the piece that points to current
                Piece* prev = buffer->head;
                while (prev && prev->next != current) {
                    prev = prev->next;
                }
                if (prev) prev->next = next;
            }
            free(current);
            current = next;
        } else {
            // Adjust piece length
            if (delete_start > 0) {
                // Split piece if deletion is in the middle
                Piece* after = create_piece(current->type,
                                          current->start + delete_start + delete_amount,
                                          current->length - delete_start - delete_amount);
                after->next = current->next;
                current->next = after;
                current->length = delete_start;
            } else {
                // Deletion at start of piece
                current->start += delete_amount;
                current->length -= delete_amount;
            }
            current = current->next;
        }

        remaining -= delete_amount;
        delete_start = 0;
    }
    
    // Set modified flag if any deletion occurred
    if (length > remaining) {
        buffer->modified = 1;
    }
}

// New functions for buffer modified state

int buffer_is_modified(Buffer* buffer) {
    return buffer ? buffer->modified : 0;
}

void buffer_set_modified(Buffer* buffer, int modified) {
    if (buffer) {
        buffer->modified = modified;
    }
}