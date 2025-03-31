#include "io.h"
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/**
 * Check if an object is a file or directory
 */
ObjectType validate_object(const char *object_name) {
    struct stat object_stat;
    
    if (stat(object_name, &object_stat) != 0) {
        return OBJECT_INVALID;
    }
    
    if (S_ISREG(object_stat.st_mode)) {
        return OBJECT_FILE;
    } else if (S_ISDIR(object_stat.st_mode)) {
        return OBJECT_DIR;
    }
    
    return OBJECT_INVALID;
}

/**
 * Read file into a newly allocated string
 */
char* io_read_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size) {
        *size = file_size;
    }

    // Allocate memory for file content
    char *content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }

    // Read file content
    size_t bytes_read = fread(content, 1, file_size, file);
    content[bytes_read] = '\0';  // Null-terminate the string
    fclose(file);
    
    return content;
}

/**
 * Write content to a file
 */
int io_write_file(const char *filename, const char *content, size_t size) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        return 0;
    }

    size_t bytes_written = fwrite(content, 1, size, file);
    fclose(file);
    
    return bytes_written == size ? 1 : 0;
}

/**
 * Check if a file exists
 */
int io_file_exists(const char *filename) {
    return access(filename, F_OK) == 0 ? 1 : 0;
}

/**
 * Get last modification time of a file
 */
unsigned long io_file_mtime(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return (unsigned long)st.st_mtime;
    }
    return 0;
}

/**
 * Get file size in bytes
 */
long io_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

/**
 * Check if path is a directory
 */
int io_is_directory(const char *path) {
    return validate_object(path) == OBJECT_DIR ? 1 : 0;
}