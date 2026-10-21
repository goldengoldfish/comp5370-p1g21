
#ifndef ARGS_H
#define ARGS_H

// The actions the user can request.
typedef enum {
    ACTION_INVALID = 0,  // No valid action selected
    ACTION_LIST,         // Display information
    ACTION_CARVE         // Extract and save data
} Action;

// What the action will work on.
typedef enum {
    SCOPE_INVALID = 0,  // No valid scope selected
    SCOPE_PARTS,        // Work with disk partitions
    SCOPE_FILES         // Work with files in a partition
} Scope;

// Stores the information from the user's command.
typedef struct {
    Action action;          // The requested action
    Scope scope;            // Partitions or files
    const char *input_path; // Path to the disk image
} Arguments;

// Reads and checks the command-line arguments.
// Returns 0 if valid, or a nonzero value if invalid.
int parse_arguments(int argc, char *argv[], Arguments *args);

// Displays the available commands.
void print_usage(const char *program_name);

#endif
