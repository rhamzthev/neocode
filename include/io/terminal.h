#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>

/**
 * Terminal Module
 * 
 * Provides low-level terminal handling including raw mode, input processing,
 * and terminal capabilities. This is a self-contained module with no dependencies
 * on other components of the system.
 */

// Terminal escape sequences
#define ESC "\033"
#define CSI ESC "["

// Screen commands
#define TERM_CLEAR_SCREEN    CSI "2J"
#define TERM_CLEAR_LINE      CSI "K"
#define TERM_ALT_SCREEN_ON   CSI "?1049h"
#define TERM_ALT_SCREEN_OFF  CSI "?1049l"
#define TERM_CURSOR_HOME     CSI "H"

// Cursor styling
#define TERM_CURSOR_BLOCK    CSI "2 q"
#define TERM_CURSOR_LINE     CSI "6 q"
#define TERM_CURSOR_HIDE     CSI "?25l"
#define TERM_CURSOR_SHOW     CSI "?25h"

// Movement
#define TERM_CURSOR_UP(n)    CSI #n "A"
#define TERM_CURSOR_DOWN(n)  CSI #n "B"
#define TERM_CURSOR_RIGHT(n) CSI #n "C"
#define TERM_CURSOR_LEFT(n)  CSI #n "D"

// Mouse support
#define TERM_MOUSE_ON        CSI "?1000;1006;1015h"  // Enable mouse tracking
#define TERM_MOUSE_OFF       CSI "?1000;1006;1015l"  // Disable mouse tracking

// Special keys
#define KEY_NULL      0
#define KEY_CTRL_A    1
#define KEY_CTRL_B    2
#define KEY_CTRL_C    3
#define KEY_CTRL_D    4
#define KEY_CTRL_E    5
#define KEY_CTRL_F    6
#define KEY_CTRL_G    7
#define KEY_CTRL_H    8     // Backspace
#define KEY_TAB       9
#define KEY_CTRL_J    10    // Line Feed
#define KEY_CTRL_K    11
#define KEY_CTRL_L    12
#define KEY_ENTER     13
#define KEY_CTRL_N    14
#define KEY_CTRL_O    15
#define KEY_CTRL_P    16
#define KEY_CTRL_Q    17    // Quit
#define KEY_CTRL_R    18
#define KEY_CTRL_S    19
#define KEY_CTRL_T    20
#define KEY_CTRL_U    21
#define KEY_CTRL_V    22
#define KEY_CTRL_W    23
#define KEY_CTRL_X    24
#define KEY_CTRL_Y    25
#define KEY_CTRL_Z    26
#define KEY_ESC       27
#define KEY_BACKSPACE 127

// Editor special keys (non-ASCII)
#define KEY_ARROW_UP     1000
#define KEY_ARROW_DOWN   1001
#define KEY_ARROW_LEFT   1002
#define KEY_ARROW_RIGHT  1003
#define KEY_HOME         1004
#define KEY_END          1005
#define KEY_PAGE_UP      1006
#define KEY_PAGE_DOWN    1007
#define KEY_DELETE       1008
#define KEY_INSERT       1009
#define KEY_WORD_LEFT    1010  // Ctrl+Left
#define KEY_WORD_RIGHT   1011  // Ctrl+Right
#define KEY_CTRL_HOME    1020  // Ctrl+Home
#define KEY_CTRL_END     1021  // Ctrl+End
#define KEY_CTRL_UP      1022  // Ctrl+Up
#define KEY_CTRL_DOWN    1023  // Ctrl+Down

/**
 * Mouse event types
 * Represents different mouse interactions that can be captured
 */
typedef enum {
    MOUSE_NONE = 0,    // No mouse event
    MOUSE_PRESS,       // Button press
    MOUSE_RELEASE,     // Button release
    MOUSE_DRAG,        // Mouse drag
    MOUSE_WHEEL_UP,    // Scroll wheel up
    MOUSE_WHEEL_DOWN   // Scroll wheel down
} MouseEventType;

/**
 * Mouse event structure
 * Contains information about a mouse event
 */
typedef struct {
    MouseEventType type;  // Type of mouse event
    size_t x;            // X coordinate (column)
    size_t y;            // Y coordinate (row)
    int button;          // Button number: 0=left, 1=middle, 2=right
} MouseEvent;

/**
 * Input event types
 * Represents different kinds of input that can be received
 */
typedef enum {
    EVENT_NONE,     // No event
    EVENT_KEY,      // Keyboard event
    EVENT_MOUSE,    // Mouse event
    EVENT_RESIZE    // Terminal resize event
} EventType;

/**
 * Input event structure
 * Unified structure for handling all input events
 */
typedef struct {
    EventType type;   // Type of event
    union {
        int key;      // Keyboard key code
        MouseEvent mouse;  // Mouse event data
    };
} InputEvent;

/**
 * Initialize terminal for raw mode
 * Sets up the terminal for direct character input and advanced features
 */
void terminal_init(void);

/**
 * Clean up terminal settings
 * Restores terminal to its original state
 */
void terminal_cleanup(void);

/**
 * Read a single character from terminal
 * Blocks until a character is available
 * @return The character read
 */
char terminal_read_char(void);

/**
 * Try to read a character from terminal without blocking
 * Returns immediately whether a character is available or not
 * @param c Pointer to store the character if available
 * @return 1 if a character was read, 0 if no character was available
 */
int terminal_read_char_nonblock(char* c);

/**
 * Check if a character is the quit command (Ctrl+Q)
 * @param c The character to check
 * @return 1 if it's the quit command, 0 otherwise
 */
int terminal_is_quit(char c);

/**
 * Get terminal dimensions
 * @param rows Pointer to store row count
 * @param cols Pointer to store column count
 */
void terminal_get_size(size_t* rows, size_t* cols);

/**
 * Read a complete input event (key or mouse)
 * This function handles escape sequences and properly identifies special keys
 * @return The input event read
 */
InputEvent terminal_read_event(void);

/**
 * Non-blocking version of terminal_read_event
 * Checks for input and returns immediately whether input is available or not
 * @param event Pointer to store the event if available
 * @return 1 if an event was read, 0 if no event was available
 */
int terminal_read_event_nonblock(InputEvent* event);

/**
 * Check if a sequence is a mouse event
 * @param sequence The character sequence to check
 * @return 1 if it's a mouse sequence, 0 otherwise
 */
int terminal_is_mouse_sequence(const char* sequence);

/**
 * Parse a mouse event sequence
 * @param sequence The mouse sequence to parse
 * @return A MouseEvent structure with the parsed information
 */
MouseEvent terminal_parse_mouse_sequence(const char* sequence);

#endif // TERMINAL_H