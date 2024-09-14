#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "euristic.h"
#include "grid.h"

extern bool verbose;

char reverse(const char c) {
    if (c == '1') {
        return '0';
    } else if (c == '0') {
        return '1';
    } else {
        errx(EXIT_FAILURE, "reverse: invalid input char : '%c'", c);
    }
}

static bool euri_consec_line(t_grid* g) {
    // when there are 2 consecutive 0 (resp. 1), the next/previous cell is a 1 (resp. 0) (for lines)
    unsigned int n = g->size;
    bool change = false;
    for (unsigned int i = 0; i < n; i++) {

        // left border, case [x x _ ...]
        if (g->grid[i][2] == '_' && g->grid[i][0] != '_' && g->grid[i][0] == g->grid[i][1]) {
            set_cell(i, 2, g, reverse(g->grid[i][1]));
            change = true;
        }

        // right border, case [... _ x x]
        if (g->grid[i][n - 3] == '_' && g->grid[i][n - 1] != '_' && g->grid[i][n - 1] == g->grid[i][n - 2]) {
            set_cell(i, n - 3, g, reverse(g->grid[i][n - 2]));
            change = true;
        }

        // middle, case [... _ x x _ ...]
        for (unsigned int j = 1; j < n - 2; j++) {
            if (g->grid[i][j] != '_' && g->grid[i][j] == g->grid[i][j + 1]) {
                if (g->grid[i][j - 1] == '_') {
                    set_cell(i, j - 1, g, reverse(g->grid[i][j]));
                    change = true;
                }
                if (g->grid[i][j + 2] == '_') {
                    set_cell(i, j + 2, g, reverse(g->grid[i][j]));
                    change = true;
                }
            }
        }
    }
    if (change && verbose) {
        printf("verbose: euri_consec_line: applied\n");
    }
    return change;
}

static bool euri_consec_col(t_grid* g) {
    // when there are 2 consecutive 0 (resp. 1), the next/previous cell is a 1 (resp. 0) (for columns)
    unsigned int n = g->size;
    bool change = false;
    for (unsigned int j = 0; j < n; j++) {

        // top border, case [x x _ ...]
        if (g->grid[2][j] == '_' && g->grid[0][j] != '_' && g->grid[0][j] == g->grid[1][j]) {
            set_cell(2, j, g, reverse(g->grid[1][j]));
            change = true;
        }

        // bottom border, case [... _ x x]
        if (g->grid[n - 3][j] == '_' && g->grid[n - 1][j] != '_' && g->grid[n - 1][j] == g->grid[n - 2][j]) {
            set_cell(n - 3, j, g, reverse(g->grid[n - 2][j]));
            change = true;
        }

        // middle, case [... _ x x _ ...]
        for (unsigned int i = 1; i < n - 2; i++) {
            if (g->grid[i][j] != '_' && g->grid[i][j] == g->grid[i + 1][j]) {
                if (g->grid[i - 1][j] == '_') {
                    set_cell(i - 1, j, g, reverse(g->grid[i][j]));
                    change = true;
                }
                if (g->grid[i + 2][j] == '_') {
                    set_cell(i + 2, j, g, reverse(g->grid[i][j]));
                    change = true;
                }
            }
        }
    }
    if (change && verbose) {
        printf("verbose: euri_consec_col: applied\n");
    }
    return change;
}

static bool euri_complete_line(t_grid* g) {
    // for each line, if the amount of 0 (resp. 1) is here, fill the rest by 1 (resp. 0)
    unsigned int count0;
    unsigned int count1;
    unsigned int n = g->size;
    char fill_with;
    bool fill;
    bool change = false;

    for (unsigned int i = 0; i < n; i++) {
        fill = false;
        count0 = 0;
        count1 = 0;
        for (unsigned int j = 0; j < n; j++) {
            if (g->grid[i][j] == '1') {
                count1++;
            } else if (g->grid[i][j] == '0') {
                count0++;
            }
        }

        //printf("count0 = %d \n count1 = %d \n", count0, count1);

        if (count0 == n / 2) {
            fill_with = '1';
            fill = true;
        } else if (count1 == n / 2) {
            fill_with = '0';
            fill = true;
        }

        if (fill) {
            for (unsigned int j = 0; j < n; j++) {
                if (g->grid[i][j] == '_') {
                    change = true;
                    set_cell(i, j, g, fill_with);
                }
            }
            if (verbose && change) {
                printf("verbose: euri_complete_line: filling line %u\n", i);
            }
        }
    }
    return change;
}

static bool euri_complete_col(t_grid* g) {
    // for each line, if the amount of 0 (resp. 1) is here, fill the rest by 1 (resp. 0)
    unsigned int count0;
    unsigned int count1;
    unsigned int n = g->size;
    char fill_with;
    bool fill;
    bool change = false;

    for (unsigned int j = 0; j < n; j++) {
        fill = false;
        count0 = 0;
        count1 = 0;
        for (unsigned int i = 0; i < n; i++) {
            if (g->grid[i][j] == '1') {
                count1++;
            } else if (g->grid[i][j] == '0') {
                count0++;
            }
        }

        //printf("count0 = %d \n count1 = %d \n", count0, count1);

        if (count0 == n / 2) {
            fill_with = '1';
            fill = true;
        } else if (count1 == n / 2) {
            fill_with = '0';
            fill = true;
        }

        if (fill) {
            for (unsigned int i = 0; i < n; i++) {
                if (g->grid[i][j] == '_') {
                    change = true;
                    set_cell(i, j, g, fill_with);
                }
            }
            if (verbose && change) {
                printf("verbose: euri_complete_column: filling column %u\n", j);
            }
        }
    }
    return change;
}

static bool euri_middle_one(t_grid* g) {
    /* This euristic treat this type of situation [... 0 _ 0 ...]
        In this case, we know that _ is a 1
        Same principle for the column
    */

    unsigned int n = g->size;
    bool change = false;

    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 1; j < n - 1; j++) {
            if (g->grid[i][j - 1] != '_' && g->grid[i][j - 1] == g->grid[i][j + 1] && g->grid[i][j] == '_') {
                change = true;
                set_cell(i, j, g, reverse(g->grid[i][j - 1]));
            }
        }
    }

    for (unsigned int j = 0; j < n; j++) {
        for (unsigned int i = 1; i < n - 1; i++) {
            if (g->grid[i - 1][j] != '_' && g->grid[i - 1][j] == g->grid[i + 1][j] && g->grid[i][j] == '_') {
                change = true;
                set_cell(i, j, g, reverse(g->grid[i - 1][j]));
            }
        }
    }

    if (verbose && change) {
        printf("verbose: middle_one: applied\n");
    }
    return change;
}

bool apply_euristics(t_grid* g) {
    bool applied = true;
    bool change = false;

    while (is_consistent(g) && applied) {
        applied = euri_consec_line(g);
        applied = euri_consec_col(g) | applied;
        applied = euri_complete_col(g) | applied;
        applied = euri_complete_line(g) | applied;
        applied = euri_consec_line(g) | applied;
        applied = euri_middle_one(g) | applied;
        change = applied | change;
    }
    return change;
}