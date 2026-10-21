
#include <stdio.h>
#include <string.h>
#include "args.h"

// Shows the user how to run the program.
void print_usage(const char *program_name) {
    printf("DiskSleuth - Disk Forensics Tool\n\n");
    printf("Usage:\n");
    printf("  %s list parts <disk_image>\n", program_name);
    printf("  %s carve parts <disk_image>\n", program_name);
    printf("  %s list files <partition_image>\n", program_name);
    printf("  %s carve files <partition_image>\n", program_name);
}

// Reads the command and checks whether it is valid.
int parse_arguments(int argc, char *argv[], Arguments *args) {

    // Make sure the result structure exists.
    if (args == NULL) {
        fprintf(stderr, "Error: Invalid arguments.\n");
        return 1;
    }

    // Set default values before checking the command.
    args->action = ACTION_INVALID;
    args->scope = SCOPE_INVALID;
    args->input_path = NULL;

    // The command needs an action, a scope, and an image path.
    if (argc != 4) {
        fprintf(stderr, "Error: Incorrect number of arguments.\n");
        print_usage(argv[0]);
        return 1;
    }

    // Check what action the user requested.
    if (strcmp(argv[1], "list") == 0) {
        args->action = ACTION_LIST;
    } else if (strcmp(argv[1], "carve") == 0) {
        args->action = ACTION_CARVE;
    } else {
        // Reject actions the program does not recognize.
        fprintf(stderr, "Error: Unknown action '%s'.\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }

    // Check whether the user wants to work with partitions or files.
    if (strcmp(argv[2], "parts") == 0) {
        args->scope = SCOPE_PARTS;
    } else if (strcmp(argv[2], "files") == 0) {
        args->scope = SCOPE_FILES;
    } else {
        // Reject scopes the program does not recognize.
        fprintf(stderr, "Error: Unknown scope '%s'.\n", argv[2]);
        print_usage(argv[0]);
        return 1;
    }

    // Save the path to the disk image or partition image.
    args->input_path = argv[3];

    // Tell the main program that the command passed validation.
    return 0;
}
