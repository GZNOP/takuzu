#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "grid.h"
#include "utils.h"

void grid_allocate(t_grid* g, int size) {
    /*
    Allocate a grid of size (size * size)
    size is positive, due to the check in the main prog
    */
    g->size = size;
    g->grid = (char**)malloc(sizeof(char*) * g->size);
    if (g->grid == NULL) {
        perror("grid_allocate");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < g->size; i++) {
        g->grid[i] = (char*)malloc(sizeof(char) * g->size);
        if (g->grid[i] == NULL) {
            perror("grid_allocate");
            exit(EXIT_FAILURE);
        }

        // set the cells with the empty character
        for (int j = 0; j < g->size; j++) {
            g->grid[i][j] = '_';
        }
    }
}

void grid_free(t_grid* g) {
    if (g == NULL) {
        warnx("warning: grid_free: the grid is already null");
        return;
    }
    for (int i = 0; i < g->size; i++) {
        free(g->grid[i]);
    }
    free(g->grid);
}

void grid_print(t_grid* g, FILE* fd) {
    // use stdout if no fd are used
    if (fd == NULL) {
        fd = stdout;
    }
    for (int i = 0; i < g->size; i++) {
        for (int j = 0; j < g->size; j++) {
            fprintf(fd, "%c ", g->grid[i][j]);
        }
        fprintf(fd, "\n");
    }
    fprintf(fd, "\n");
}

bool check_char(const char c) {
    return c == '0' || c == '1' || c == '_';
}

bool check_sep(const char c) {
    /*
    return true if c is a separator for grid file
    */
    return c == ' ' || c == '\t';
}

int number_of_column(char* s) {
    /*
    count the size of the string (stop at # and \n) 
    */

    int i = 0;
    int cpt = 0;
    while (s[i] != '\0' && s[i] != '#' && s[i] != '\n') {

        if (check_char(s[i])) {
            cpt++;
        }
        i++;
    }
    return cpt;
}

bool is_comment_line(char* s) {
    int i = 0;
    while (s[i] != '\0' && s[i] != '\n') {
        if (s[i] == '#') {
            return true;
        } else if (!check_sep(s[i])) {
            return false;
        }
        i++;
    }
    return true;
}

void file_parser(t_grid* g, char* fname) {
    /*
    Parse the file fname and fill the g grid, 
    Possibility to use comment with '#'
    separator between value of the grid 
    can be ' ' (blank) or '\t' (tabulation)
    don't accept empty line without comment 
    */
    // Opening the file first
    FILE* fd = fopen(fname, "r");
    // checking error
    if (!fd) {
        perror("error when opening the file");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    int j = 0;
    int iline = 0;
    int icol = 0;
    int endf;

    char toprint;

    char* buf = NULL;
    size_t size_buf = 0;
    int size_grid;
    // check the first line
    endf = getline(&buf, &size_buf, fd);
    if (endf == -1) {
        free(buf);
        fclose(fd);
        errx(EXIT_FAILURE, "error: %s: empty file", fname);
    }
    /*
    skip the first comment lines
    */
    while (endf != -1 && is_comment_line(buf)) {
        endf = getline(&buf, &size_buf, fd);
        iline++;
    }

    /*
    treat the case if there are only comment in the file
    */
    if (endf == -1) {
        free(buf);
        fclose(fd);
        errx(EXIT_FAILURE, "error: %s: no grid in this file", fname);
    }

    //getting the grid size
    size_grid = number_of_column(buf);
    if (!control_size_grid(size_grid)) {
        free(buf);
        fclose(fd);
        errx(EXIT_FAILURE, "error: %s: line %d is malformed", fname, iline + 1);
    }

    // Now we have the supposed size, so we can allocate the grid
    grid_allocate(g, size_grid);

    iline++;

    // parsing each line
    while (endf != -1) {
        icol = 0;
        j = 0;
        // treat the line if it's not a comment line
        if (!is_comment_line(buf)) {

            // check the number of line to parse
            if (i >= size_grid) {
                free(buf);
                fclose(fd);
                grid_free(g);
                errx(EXIT_FAILURE, "error: %s: too many lines to parse, required %d according to the column", fname, size_grid);
            }

            // check each char until the end of the line
            while (buf[icol] != '\n' && buf[icol] != '\0' && buf[icol] != '#') {

                // if a valid char, add it to the grid
                if (check_char(buf[icol])) {

                    // case where we have too much / less valid char on the line
                    if (j >= size_grid) {
                        free(buf);
                        fclose(fd);
                        grid_free(g);
                        errx(EXIT_FAILURE, "error: %s: line %d is malformed", fname, iline + 1);
                    }

                    g->grid[i][j] = buf[icol];
                    j++;
                    // if not  a separator char : error
                } else if (!check_sep(buf[icol])) {
                    toprint = buf[icol]; // because we free
                    free(buf);
                    fclose(fd);
                    grid_free(g);
                    errx(EXIT_FAILURE, "error: %s: wrong character '%c' at line %d", fname, toprint, iline + 1);
                }

                icol++;
            }
            i++;
        }
        iline++;
        endf = getline(&buf, &size_buf, fd);
    }

    if (i < size_grid) {
        grid_free(g);
        free(buf);
        fclose(fd);
        errx(EXIT_FAILURE, "error: %s: grid has %d missing line(s)!", fname, size_grid - i);
    }

    free(buf);
    fclose(fd);
}

void grid_copy(t_grid* gs, t_grid* gd) {
    // check if the source grid is null
    if (gs == NULL) {
        errx(EXIT_FAILURE, "error: grid_copy: gs is null");
    }
    grid_allocate(gd, gs->size);

    // deep copy
    for (int i = 0; i < gd->size; i++) {
        for (int j = 0; j < gd->size; j++) {
            gd->grid[i][j] = gs->grid[i][j];
        }
    }
}

void check_bound_error(int i, int j, t_grid* g) {
    // this function is used to generate error
    if (g == NULL) {
        errx(EXIT_FAILURE, "error: set_cell: the grid is not allocated");
    }

    int size = g->size;
    if (i < 0 || i >= size) {
        grid_free(g);
        errx(EXIT_FAILURE, "error: index out of range %d", i);
    }
    if (j < 0 || j >= size) {
        grid_free(g);
        errx(EXIT_FAILURE, "error: index out of range %d", j);
    }
}

void set_cell(int i, int j, t_grid* g, char v) {
    // set the char v in the cell i,j of the grid g
    check_bound_error(i, j, g);

    if (!check_char(v)) {
        errx(EXIT_FAILURE, "error: set_cell: invalid inserted char %c", v);
    }

    g->grid[i][j] = v;
    if (verbose) {
        printf("verbose: set_cell: cell (%u, %u) set to %c\n", i, j, v);
    }
}

char get_cell(int i, int j, t_grid* g) {
    // return the value of the cell i,j of the grid g
    check_bound_error(i, j, g);
    return g->grid[i][j];
}

static bool same_line(int l1, int l2, t_grid* g) {
    // check if line l1 is equal to line l2
    check_bound_error(l1, l2, g);
    for (int j = 0; j < g->size; j++) {
        if (g->grid[l1][j] == '_' || g->grid[l2][j] == '_') {
            return false;
        }
        if (g->grid[l1][j] != g->grid[l2][j]) {
            return false;
        }
    }
    return true;
}

static bool check_line(t_grid* g) {
    // compare all the line to check if two are similar
    for (int i = 0; i < g->size; i++) {
        for (int j = i + 1; j < g->size; j++) {
            if (same_line(i, j, g)) {
                if (verbose) {
                    printf("verbose: check_line: similar line %u %u\n", i, j);
                }
                return false;
            }
        }
    }
    return true;
}

static bool same_column(int c1, int c2, t_grid* g) {
    // check if column c1 is equal to column c2
    check_bound_error(c1, c2, g);
    for (int i = 0; i < g->size; i++) {
        if (g->grid[i][c1] == '_' || g->grid[i][c2] == '_') {
            return false;
        }
        if (g->grid[i][c1] != g->grid[i][c2]) {
            return false;
        }
    }
    return true;
}

static bool check_column(t_grid* g) {
    // compare all the column to check if two are similar
    for (int i = 0; i < g->size; i++) {
        for (int j = i + 1; j < g->size; j++) {
            if (same_column(i, j, g)) {
                if (verbose) {
                    printf("verbose: check_column: similar column %u %u\n", i, j);
                }
                return false;
            }
        }
    }
    return true;
}

static bool check_count(t_grid* g) {
    /*
    This function will check if there is the same amount of 0 and 1 
    for each line / col
    work if the grid is filled
    */

    /*
    optimisation idea:
        1 list containing the count of 0 for each column
        1 list containing the count of 0 for each line
        after bowsing the grid, check the counter for each list. 
    */
    unsigned int n = g->size;

    // line check
    unsigned int count0;
    unsigned int count1;
    for (unsigned int i = 0; i < n; i++) {
        count0 = 0;
        count1 = 0;
        for (unsigned int j = 0; j < n; j++) {
            if (g->grid[i][j] == '0') {
                count0++;
            } else if (g->grid[i][j] == '1') {
                count1++;
            }
        }

        if ((count0 > n / 2) || (count1 > n / 2)) {
            if (verbose) {
                printf("verbose: check_count: disproportion value at line %u\n", i);
            }
            return false;
        }
    }

    // column check
    for (unsigned int i = 0; i < n; i++) {
        count0 = 0;
        count1 = 0;
        for (unsigned int j = 0; j < n; j++) {
            if (g->grid[j][i] == '0') {
                count0++;
            } else if (g->grid[j][i] == '1') {
                count1++;
            }
        }

        if ((count0 > n / 2) || (count1 > n / 2)) {
            if (verbose) {
                printf("verbose: check_count: disproportion of value at column %u\n", i);
            }
            return false;
        }
    }

    return true;
}

static bool check_consecutive_4line(t_grid* g) {
    // Check if there are three consecutive 0 or 1 on the lines of the grid
    unsigned int n = g->size;

    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < n - 2; j++) {
            // check each [..., x, y, z, ...] if (x = y = z != '_') then it's false
            if (g->grid[i][j] != '_' && g->grid[i][j] == g->grid[i][j + 1] && g->grid[i][j] == g->grid[i][j + 2]) {
                if (verbose) {
                    printf("verbose: check_consecutive: 3 consecutive '%c' in line %u\n", g->grid[i][j], i);
                }
                return false;
            }
        }
    }
    return true;
}

static bool check_consecutive_4col(t_grid* g) {
    // Check if there are three consecutive 0 or 1 on the columns of the grid
    unsigned int n = g->size;

    for (unsigned int j = 0; j < n; j++) {
        for (unsigned int i = 0; i < n - 2; i++) {
            // check each [..., x, y, z, ...] if (x = y = z != '_') then it's false
            if (g->grid[i][j] != '_' && g->grid[i][j] == g->grid[i + 1][j] && g->grid[i][j] == g->grid[i + 2][j]) {
                if (verbose) {
                    printf("verbose: check_consecutive: 3 consecutive '%c' in column %u\n", g->grid[i][j], j);
                }
                return false;
            }
        }
    }
    return true;
}

static bool check_consecutive(t_grid* g) {
    // check if there are consecutive 0 / 1 on the grid
    return check_consecutive_4line(g) && check_consecutive_4col(g);
}

static bool is_full(t_grid* g) {
    // check if the grid is full
    unsigned int n = g->size;
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < n; j++) {
            if (g->grid[i][j] == '_') {
                if (verbose) {
                    printf("verbose: if_full: '_' char at line %u, column %u\n", i, j);
                }
                return false;
            }
        }
    }
    return true;
}

bool is_consistent(t_grid* g) {
    return check_consecutive(g) && check_line(g) && check_column(g) && check_count(g);
}

bool is_valid(t_grid* g) {
    return is_full(g) && is_consistent(g);
}

void fill_grid(t_grid* g) {
    /*
    this function will fill the empty grid g with a N% of 0-1, the grid has
    to be consistent
    */
    if (g == NULL) {
        errx(EXIT_FAILURE, "error: fill_grid: g is Null\n");
    }

    unsigned int size = g->size;
    unsigned count = 0;
    unsigned int n_cell_to_fill = (size * size * N) / 100; // just a cross product
    unsigned int i = random() % size;
    unsigned int j = random() % size;
    unsigned int c;

    while (count < n_cell_to_fill) {

        i = random() % size;
        j = random() % size;
        c = random() % 2;

        if (g->grid[i][j] == '_') {
            count++;
        }

        if (c) {
            set_cell(i, j, g, '1');
        } else {
            set_cell(i, j, g, '0');
        }

        if (!is_consistent(g)) {
            set_cell(i, j, g, '_');
            count--;
        }
        //grid_print(g, NULL);
    }
}

void set_grid_empty(t_grid* g) {
    for (int i = 0; i < g->size; i++) {
        for (int j = 0; j < g->size; j++) {
            set_cell(i, j, g, '_');
        }
    }
}