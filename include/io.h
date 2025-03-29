#ifndef IO_H
#define IO_H

#include "buffer.h"  // Include buffer.h for the Buffer type

// Object type enumeration
typedef enum {
    OBJECT_INVALID = 0,  // Invalid or non-existent object
    OBJECT_FILE = 1,     // Regular file
    OBJECT_DIR = 2       // Directory
} ObjectType;

// Check if it's a file or directory
ObjectType validate_object(const char *object_name);

// Read and print file contents (legacy)
int print_file_contents(const char *filename);

// Load file into buffer
Buffer* load_file_to_buffer(const char *filename);

#endif // IO_H