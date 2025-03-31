#ifndef IO_H
#define IO_H

#include <stddef.h>

/**
 * IO Module
 * 
 * Provides file system operations for reading and writing files.
 * This is a self-contained module with no dependencies on other
 * components of the system.
 */

// Object type enumeration
typedef enum {
    OBJECT_INVALID = 0,  // Invalid or non-existent object
    OBJECT_FILE = 1,     // Regular file
    OBJECT_DIR = 2       // Directory
} ObjectType;

/**
 * Validate if a path points to a file or directory
 * 
 * @param object_name Path to check
 * @return ObjectType indicating file, directory, or invalid
 */
ObjectType validate_object(const char *object_name);

/**
 * Load file contents into a newly allocated string
 * 
 * @param filename Path of file to load
 * @param size Pointer to store the size of loaded content
 * @return Newly allocated string with file contents (caller must free), or NULL on error
 */
char* io_read_file(const char *filename, size_t *size);

/**
 * Write content to a file
 * 
 * @param filename Path to write to
 * @param content Content to write
 * @param size Length of content
 * @return 1 on success, 0 on failure
 */
int io_write_file(const char *filename, const char *content, size_t size);

/**
 * Check if a file exists
 * 
 * @param filename Path to check
 * @return 1 if file exists, 0 otherwise
 */
int io_file_exists(const char *filename);

/**
 * Get the last modification time of a file
 * 
 * @param filename Path to check
 * @return Timestamp of last modification, or 0 on error
 */
unsigned long io_file_mtime(const char *filename);

/**
 * Get the file size in bytes
 * 
 * @param filename Path to check
 * @return File size in bytes, or -1 on error
 */
long io_file_size(const char *filename);

/**
 * Check if a path is a directory
 * 
 * @param path Path to check
 * @return 1 if directory, 0 otherwise
 */
int io_is_directory(const char *path);

#endif // IO_H