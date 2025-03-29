#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>

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
#define KEY_CTRL_S    19
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

// Combined control key sequences
#define KEY_CTRL_HOME     1020  // Ctrl+Home
#define KEY_CTRL_END      1021  // Ctrl+End

// Extended control key combinations
#define KEY_CTRL_UP      1022  // Ctrl+Up
#define KEY_CTRL_DOWN    1023  // Ctrl+Down

// Mouse event types
typedef enum {
    MOUSE_NONE = 0,
    MOUSE_PRESS,
    MOUSE_RELEASE,
    MOUSE_DRAG,
    MOUSE_WHEEL_UP,
    MOUSE_WHEEL_DOWN
} MouseEventType;

// Mouse event structure
typedef struct {
    MouseEventType type;
    size_t x;
    size_t y;
    int button; // 0 = left, 1 = middle, 2 = right, etc.
} MouseEvent;

// Input event type
typedef enum {
    EVENT_NONE,
    EVENT_KEY,
    EVENT_MOUSE,
    EVENT_RESIZE
} EventType;

// Input event structure
typedef struct {
    EventType type;
    union {
        int key;  // Changed from char to int to support special keys
        MouseEvent mouse;
    };
} InputEvent;

// Function declarations
void terminal_init(void);
void terminal_cleanup(void);
char terminal_read_char(void);
int terminal_is_quit(char c);
void terminal_get_size(size_t* rows, size_t* cols);

// New functions for mouse support
InputEvent terminal_read_event(void);
int terminal_is_mouse_sequence(const char* sequence);
MouseEvent terminal_parse_mouse_sequence(const char* sequence);

#endif // TERMINAL_H