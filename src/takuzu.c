#include <err.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "backtracking.h"
#include "euristic.h"
#include "grid.h"
#include "takuzu.h"
#include "utils.h"

bool verbose = 0;

int main(int argc, char* argv[]) {
    srand(time(NULL));
    bool all = false, unique = false, g_mode = false, i_flag = false, o_flag = false;
    t_grid g;

    static struct option long_options[] = {
        {    "help",       no_argument, NULL, 'h'},
        {     "all",       no_argument, NULL, 'a'},
        { "verbose",       no_argument, NULL, 'v'},
        {"generate", optional_argument, NULL, 'g'},
        {  "output", optional_argument, NULL, 'o'},
        {  "unique",       no_argument, NULL, 'u'},
        {      NULL,                 0, NULL,   0}  // the end of the struct
    };

    int opt;
    char size_grid = 8; // use of char because the grid max size is 64
    char o_file[256];
    char i_file[256];
    FILE* fd_output = NULL; // the file where the solution will be written

    while ((opt = getopt_long(argc, argv, "havg:o:u", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h': // HELP
                display_help(argv[0]);
                exit(EXIT_SUCCESS);
            case 'v': // VERBOSE
                verbose = true;
                break;
            case 'o': // OUTPUT FILE
                o_flag = true;
                if (strlen(optarg) > 256) {
                    errx(EXIT_FAILURE, "error: -o FILE: filename with too many characters");
                }
                strcpy(o_file, optarg);
                break;
            case 'a': // ALL OPTION
                all = true;
                break;
            case 'u': // UNIQUE OPTION
                unique = true;
                break;
            case 'g': // GENERATION MODE
                g_mode = true;
                if (!optarg) {
                    break;
                }
                size_grid = atoi(optarg);
                if (!control_size_grid(size_grid)) {
                    warnx("warning: invalid size grid, using default value!");
                    size_grid = 8;
                }
                warnx("info: size grid set to %d", size_grid);
                break;
            default:
                break;
        }
    }

    // If not in generation mode then verify the FILE to use in input

    if (optind < argc) {
        i_flag = true;

        if (!g_mode) {
            if (strlen(argv[optind]) > 256) {
                errx(EXIT_FAILURE, "error: input FILE: filename with too many characters");
            }
            strcpy(i_file, argv[optind]);
        } else {
            warnx("warning: input FILE set, but will not be used due to generation mode!");
        }
    }

    // Verification about -o option
    if (o_flag) {
        warnx("info: -o option, using '%s' as output file", o_file);
        fd_output = fopen(o_file, "w");
        if (fd_output == NULL) {
            perror("Error while opening output file");
        }
    } else {
        warnx("warning: no -o option, using standard output");
        fd_output = stdout;
    }

    if (g_mode) {
        if (all) {
            warnx("warning: option 'all' conflict with generation mode, disabling it!");
            all = false;
        }

        if (verbose) {
            warnx("info: executing in generation mode");
        }

        grid_allocate(&g, size_grid);
        fill_grid(&g);

        t_grid cpy;

        if (unique) {
            int nb = 0;
            // Just to have the nb of solution of g

            grid_copy(&g, &cpy);

            find_solutionALL(&cpy, &nb, NULL);

            printf("verbose: generating grid with unique solution...\n");

            // generate grid until it has 1 solution
            while (nb != 1) {
                nb = 0;

                grid_free(&cpy);
                grid_free(&g);
                grid_allocate(&g, size_grid);
                fill_grid(&g);

                grid_copy(&g, &cpy);
                find_solutionALL(&cpy, &nb, NULL);
            }

            printf("verbose: generation done\n");

            grid_free(&cpy);

            grid_print(&g, fd_output);

        }

        else {
            // Just to have the nb of solution of g
            // We copy the random grid and then try to find solution on it
            bool has_sol = false;
            grid_copy(&g, &cpy);
            find_solution1(&cpy, &has_sol, NULL);

            if (verbose) {
                printf("verbose: generating grid with multiple solution...\n");
            }

            // generate grid until it has 1 solution
            while (has_sol == false) {
                grid_free(&g);
                grid_free(&cpy);

                grid_allocate(&g, size_grid);
                fill_grid(&g);
                grid_copy(&g, &cpy);

                find_solution1(&cpy, &has_sol, NULL);
            }

            grid_free(&cpy);

            if (verbose) {
                printf("verbose: generation done\n");
            }

            grid_print(&g, fd_output);
        }

        // Doing some stuff about generation mode
    } else {
        if (unique) {
            warnx("warning: option 'unique' conflict with solver mode, disabling it!");
            unique = false;
        }

        if (!i_flag) {
            errx(EXIT_FAILURE, "error: no input grid given!");
        }

        file_parser(&g, i_file);

        if (verbose) {
            printf("verbose: trying to find solution for :\n");
            grid_print(&g, NULL);
        }
        if (all) {
            grid_solver(&g, MODE_ALL, fd_output);
        } else {
            t_grid* sol = grid_solver(&g, MODE_FIRST, fd_output);
            if (sol == NULL) {
                // no solution
                fprintf(fd_output, "No solution for the input grid\n");
            } else {
                // 1 solution
                fprintf(fd_output, "found a solution for the input grid :\n\n");
                grid_print(sol, fd_output);
            }
            grid_free(sol);
            free(sol);
        }

        warnx("info: executing in solver mode");
        // Doing some stuff about solver mode
    }

    grid_free(&g);
    return 0;
}
