#include <stdio.h>
#include "args.h"

int main(int argc, char **argv) {
    Arguments args;

    if (parse_arguments(argc, argv, &args) != 0) {
        return 1;
    }

    printf("Input file: %s\n", args.input_path);

    return 0;
}
