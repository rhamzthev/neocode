#ifndef COMMANDS_H
#define COMMANDS_H

#include "editor.h"
#include "terminal.h"

// Navigation commands
void cmd_move_to_start_of_line(EditorState* state);
void cmd_move_to_end_of_line(EditorState* state);
void cmd_move_to_start_of_document(EditorState* state);
void cmd_move_to_end_of_document(EditorState* state);
void cmd_move_word(EditorState* state, int direction);
void cmd_page_move(EditorState* state, int direction);

// Mouse handling
void cmd_process_mouse_event(EditorState* state, MouseEvent event);

#endif // COMMANDS_H