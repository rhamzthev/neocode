#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/select.h>
#include "terminal.h"

#define MAX_SEQUENCE_LENGTH 32

static struct termios orig_termios;

static void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    
    // Disable mouse tracking
    printf(TERM_MOUSE_OFF);
    
    // Restore cursor style and visibility before switching screens
    printf(TERM_CURSOR_SHOW);    // Ensure cursor is visible
    printf(TERM_CURSOR_BLOCK);   // Reset to default block cursor
    printf(TERM_ALT_SCREEN_OFF); // Return to main screen
    
    fflush(stdout); // Ensure all escape sequences are processed
}

void terminal_init() {
    // Save original terminal settings
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    // Configure raw mode
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    
    // Set minimum chars and timeout
    raw.c_cc[VMIN] = 0;  // Return immediately when any char is available
    raw.c_cc[VTIME] = 1; // Wait up to 100ms for input
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    // Switch to alternate screen and set cursor style
    printf(TERM_ALT_SCREEN_ON);
    printf(TERM_CURSOR_HOME);
    printf(TERM_CURSOR_LINE);    // Set to beam style
    
    // Enable mouse tracking
    printf(TERM_MOUSE_ON);
    
    fflush(stdout);
}

void terminal_cleanup() {
    disable_raw_mode();
}

char terminal_read_char() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) {
            perror("read");
            exit(1);
        }
    }
    return c;
}

int terminal_is_quit(char c) {
    return c == KEY_CTRL_Q; // Ctrl+Q
}

void terminal_get_size(size_t* rows, size_t* cols) {
    struct winsize ws;
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        // Fallback to default values if ioctl fails
        *cols = 80;
        *rows = 24;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
    }
}

// Update the escape sequence parser to recognize more sequences

int parse_escape_sequence(const char* sequence, int seq_len) {
    if (seq_len < 2) return KEY_ESC;
    
    // Handle common arrow keys and navigation keys
    if (sequence[0] == ESC[0] && sequence[1] == '[') {
        if (seq_len == 3) {
            switch (sequence[2]) {
                case 'A': return KEY_ARROW_UP;
                case 'B': return KEY_ARROW_DOWN;
                case 'C': return KEY_ARROW_RIGHT;
                case 'D': return KEY_ARROW_LEFT;
                case 'H': return KEY_HOME;
                case 'F': return KEY_END;
            }
        } else if (seq_len > 3) {
            if (sequence[2] == '1' && sequence[3] == '~') return KEY_HOME;
            if (sequence[2] == '4' && sequence[3] == '~') return KEY_END;
            if (sequence[2] == '5' && sequence[3] == '~') return KEY_PAGE_UP;
            if (sequence[2] == '6' && sequence[3] == '~') return KEY_PAGE_DOWN;
            if (sequence[2] == '3' && sequence[3] == '~') return KEY_DELETE;
            if (sequence[2] == '2' && sequence[3] == '~') return KEY_INSERT;
            
            // VSCode control key combinations
            if (seq_len >= 6 && sequence[2] == '1' && sequence[3] == ';' && sequence[4] == '5') {
                switch(sequence[5]) {
                    case 'A': return KEY_CTRL_UP;      // Ctrl+Up
                    case 'B': return KEY_CTRL_DOWN;    // Ctrl+Down
                    case 'C': return KEY_WORD_RIGHT;   // Ctrl+Right
                    case 'D': return KEY_WORD_LEFT;    // Ctrl+Left
                    case 'H': return KEY_CTRL_HOME;    // Ctrl+Home
                    case 'F': return KEY_CTRL_END;     // Ctrl+End
                }
            }
            
            // Some terminals have different sequences for these
            if (seq_len >= 5) {
                if (sequence[2] == '1' && sequence[3] == ';' && sequence[4] == '5' && sequence[5] == 'H')
                    return KEY_CTRL_HOME;
                if (sequence[2] == '1' && sequence[3] == ';' && sequence[4] == '5' && sequence[5] == 'F')
                    return KEY_CTRL_END;
            }
        }
    } else if (sequence[0] == ESC[0] && sequence[1] == 'O') {
        // Handle alternate arrow key representation
        if (seq_len == 3) {
            switch(sequence[2]) {
                case 'A': return KEY_ARROW_UP;
                case 'B': return KEY_ARROW_DOWN;
                case 'C': return KEY_ARROW_RIGHT;
                case 'D': return KEY_ARROW_LEFT;
                case 'H': return KEY_HOME;
                case 'F': return KEY_END;
            }
        }
    }
    
    return KEY_ESC; // Default to ESC if not recognized
}

// Check if the sequence is a mouse event
int terminal_is_mouse_sequence(const char* sequence) {
    if (strlen(sequence) >= 3 && sequence[0] == ESC[0] && sequence[1] == '[' && sequence[2] == '<') {
        return 1; // SGR mouse encoding
    }
    return 0;
}

// Parse SGR-encoded mouse sequence
MouseEvent terminal_parse_mouse_sequence(const char* sequence) {
    MouseEvent event = {MOUSE_NONE, 0, 0, 0};
    
    // SGR encoded mouse event format: ESC[<button;x;y[M|m]
    // Where 'M' indicates press/drag and 'm' indicates release
    
    int button, x, y;
    char final;
    if (sscanf(sequence + 3, "%d;%d;%d%c", &button, &x, &y, &final) != 4) {
        return event;
    }
    
    // Adjust coordinates (1-based to 0-based)
    event.x = x - 1;
    event.y = y - 1;
    
    // Process button value
    if (button == 0) {
        event.button = 0; // Left button
    } else if (button == 1) {
        event.button = 1; // Middle button
    } else if (button == 2) {
        event.button = 2; // Right button
    } else if (button >= 64) {
        // Mouse wheel (64 + button)
        if (button == 64) {
            event.type = MOUSE_WHEEL_UP;
            return event;
        } else if (button == 65) {
            event.type = MOUSE_WHEEL_DOWN;
            return event;
        }
    }
    
    // Determine event type based on final character
    if (final == 'M') {
        if (button & 32) {
            event.type = MOUSE_DRAG;
        } else {
            event.type = MOUSE_PRESS;
        }
    } else if (final == 'm') {
        event.type = MOUSE_RELEASE;
    }
    
    return event;
}

// Read a complete input event (key or mouse)
InputEvent terminal_read_event() {
    InputEvent event = {EVENT_NONE};
    char sequence[MAX_SEQUENCE_LENGTH] = {0};
    int seq_pos = 0;
    
    // Read first character
    char c = terminal_read_char();
    
    if (c == ESC[0]) {
        // Could be an escape sequence
        sequence[seq_pos++] = c;
        
        // Try to read more characters to form a complete sequence
        while (seq_pos < MAX_SEQUENCE_LENGTH - 1) {
            int nread;
            char next_char;
            
            // Short timeout to check if more characters are coming
            struct timeval tv = {0, 10000}; // 10ms timeout
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);
            
            if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) <= 0) {
                // Timeout or error - no more chars to read
                break;
            }
            
            if ((nread = read(STDIN_FILENO, &next_char, 1)) != 1) {
                break;
            }
            
            sequence[seq_pos++] = next_char;
            sequence[seq_pos] = '\0';
            
            // Check if we've read a complete mouse sequence
            if (terminal_is_mouse_sequence(sequence) && 
                (seq_pos >= 3 && (sequence[seq_pos-1] == 'M' || sequence[seq_pos-1] == 'm'))) {
                event.type = EVENT_MOUSE;
                event.mouse = terminal_parse_mouse_sequence(sequence);
                return event;
            }
        }
        
        // If it wasn't a mouse event, check for special keys
        event.type = EVENT_KEY;
        event.key = parse_escape_sequence(sequence, seq_pos);
    } else {
        // Regular key press
        event.type = EVENT_KEY;
        event.key = c;
    }
    
    return event;
}