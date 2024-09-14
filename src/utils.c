#include <err.h>
#include <stdbool.h>
#include <stdio.h>

#include "utils.h"

void display_help(char* prog_name) {
    // Display the help when -h option is used
    printf("Usage:\t%s [-a|-o FILE|-v|-h] FILE...\n", prog_name);
    printf("\t%s -g[SIZE] [-u|-o FILE|-v|-h]\n", prog_name);
    printf("Solve or generate takuzu grids of size: 4, 8 16, 32, 64\n");
    printf("-a, --all search for all possible solutions\n");
    printf("-g[N], --generate[=N] generate a grid of size NxN (default:8)\n");
    printf("-o FILE, --output FILE write output to FILE\n");
    printf("-u, --unique generate a grid with unique solution\n");
    printf("-v, --verbose verbose output\n");
    printf("-h, --help display this help and exit\n");
}

int control_size_grid(int n) {
    /* 
    Control if the integer n can be used as a grid size (i.e  4, 8, 16, 32, 64)
    if not use default value (i.e  8)
    */
    switch (n) {
        case 4:
            break;
        case 8:
            break;
        case 16:
            break;
        case 32:
            break;
        case 64:
            break;
        default:
            return false;
    }
    return true;
}
