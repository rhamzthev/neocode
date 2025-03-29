#include "io.h"
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

ObjectType validate_object(const char *object_name) {
    struct stat object_stat;
    
    if (stat(object_name, &object_stat) != 0) {
        printf("Error: '%s' does not exist.\n", object_name);
        return OBJECT_INVALID;
    }
    
    if (S_ISREG(object_stat.st_mode)) {
        return OBJECT_FILE;
    } else if (S_ISDIR(object_stat.st_mode)) {
        return OBJECT_DIR;
    }
    
    printf("Error: '%s' is neither a regular file nor a directory.\n", object_name);
    return OBJECT_INVALID;
}

// Keeping the old function for compatibility
int print_file_contents(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file '%s'\n", filename);
        return 0;
    }

    int c;
    int prev = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n' && prev != '\r') {
            // For LF without CR, print both CR and LF
            printf("\r\n");
        } else if (c != '\n' || prev != '\r') {
            // Print all other characters except LF when it follows CR
            // (we already printed it with the CR)
            putchar(c);
        }
        prev = c;
    }

    fclose(file);
    return 1;
}

// New function to load file into a buffer
Buffer* load_file_to_buffer(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for file content
    char *content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    // Read file content
    size_t bytes_read = fread(content, 1, file_size, file);
    content[bytes_read] = '\0';  // Null-terminate the string
    fclose(file);

    // Create buffer from content
    Buffer *buffer = buffer_create(content);
    free(content);
    
    return buffer;
}