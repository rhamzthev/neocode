#ifndef COMMANDS_H
#define COMMANDS_H

#include "editor.h"
#include "terminal.h"

/**
 * Commands Module
 *
 * Provides editor command implementations that are executed through the editor.
 * Commands feed into the Editor component, which acts as a facade for the rest
 * of the system.
 */

// Navigation commands
/**
 * Move cursor to start of current line
 * @param state Editor state
 */
void cmd_move_to_start_of_line(EditorState* state);

/**
 * Move cursor to end of current line
 * @param state Editor state
 */
void cmd_move_to_end_of_line(EditorState* state);

/**
 * Move cursor to start of document
 * @param state Editor state
 */
void cmd_move_to_start_of_document(EditorState* state);

/**
 * Move cursor to end of document
 * @param state Editor state
 */
void cmd_move_to_end_of_document(EditorState* state);

/**
 * Move cursor by word
 * @param state Editor state
 * @param direction Direction to move: 1 for forward, -1 for backward
 */
void cmd_move_word(EditorState* state, int direction);

/**
 * Move cursor by page (screen height)
 * @param state Editor state
 * @param direction Direction to move: 1 for down, -1 for up
 */
void cmd_page_move(EditorState* state, int direction);

// Mouse handling
/**
 * Process a mouse event
 * @param state Editor state
 * @param event Mouse event to process
 */
void cmd_process_mouse_event(EditorState* state, MouseEvent event);

#endif // COMMANDS_H