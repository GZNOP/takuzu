#include <stdbool.h>
#include <stdio.h>

#include "backtracking.h"
#include "euristic.h"
#include "grid.h"

// static void fill_first_found_cell(t_grid* g, const char v) {
//     unsigned int n = g->size;
//     for (unsigned int i = 0; i < n; i++) {
//         for (unsigned int j = 0; j < n; j++) {
//             if (g->grid[i][j] == '_') {
//                 set_cell(i, j, g, v);
//             }
//         }
//     }
// }

// static void find_solution(t_grid* g, int* nb_sol) {

//     apply_euristics(g);
//     t_grid g1, g2;

//     if (!is_consistent(g)){
//         return;
//     }

//     if (is_valid(g)) {
//         (*nb_sol)++;
//         grid_print(g, NULL);
//     }

//     grid_copy(g, &g1);
//     fill_first_found_cell(&g1, '0');
//     find_solution(&g1, nb_sol);
//     grid_free(&g1);

//     grid_copy(g, &g2);
//     fill_first_found_cell(&g2, '1');
//     find_solution(&g2, nb_sol);
//     grid_free(&g2);
//     return;
// }

// void resolve_all(t_grid * g){
//     int nb_sol = 0;
//     find_solution(g, &nb_sol);

//     printf("%d solutions found\n", nb_sol);
// }

static void grid_choice_apply(t_grid* g, const choice_t choice) {
    //printf("%d %d %c\n",choice.row, choice.column, choice.choice);
    set_cell(choice.row, choice.column, g, choice.choice);
}

static void grid_choice_print(choice_t choice, FILE* fd) {
    if (fd == NULL) {
        fd = stdout;
    }
    fprintf(fd, "verbose: choice: row = %d and column = %d\n", choice.row, choice.column);
}

static int filled_cell_around(t_grid* g, int i, int j) {
    int cpt = 0;
    check_bound_error(i, j, g);

    // top left
    if (i > 0 && j > 0 && g->grid[i - 1][j - 1] != '_') {
        cpt++;
    }

    // top middle
    if (i > 0 && g->grid[i - 1][j] != '_') {
        cpt++;
    }

    // top right
    if (i > 0 && j < g->size - 1 && g->grid[i - 1][j + 1] != '_') {
        cpt++;
    }

    //middle left
    if (j > 0 && g->grid[i][j - 1] != '_') {
        cpt++;
    }

    // middle right
    if (j < g->size - 1 && g->grid[i][j + 1] != '_') {
        cpt++;
    }

    // bottom left
    if (i < g->size - 1 && j > 0 && g->grid[i + 1][j - 1] != '_') {
        cpt++;
    }

    // middle bottom
    if (i < g->size - 1 && g->grid[i + 1][j] != '_') {
        cpt++;
    }

    // bottom right
    if (i < g->size - 1 && j < g->size - 1 && g->grid[i + 1][j + 1] != '_') {
        cpt++;
    }

    return cpt;
}

static choice_t grid_choice(t_grid* g) {
    /* we will try to chose the cell close to filled cell 
       to have a better usage of the euristics.
       To do that, we will store the current counter of the filled cells of the
       8 cells around the cell studied*/

    choice_t choice;
    int maxi = 0;
    int tmp;
    for (int i = 0; i < g->size; i++) {
        for (int j = 0; j < g->size; j++) {
            if (g->grid[i][j] == '_') {
                tmp = filled_cell_around(g, i, j);
                if (tmp > maxi) {
                    choice.row = i;
                    choice.column = j;
                    maxi = tmp;
                }
            }
        }
    }

    return choice;
}

void find_solution1(t_grid* g, bool* has_sol, t_grid* sol) {
    if (!is_consistent(g) || *has_sol) {
        return;
    }

    if (is_valid(g)) {
        if (sol != NULL) {
            grid_copy(g, sol);
        }
        *has_sol = true;
        return;
    }

    apply_euristics(g);

    // here, g is still consistent but not valid, so we keep trying to find solutions
    t_grid g1, g2;
    choice_t choice = grid_choice(g);
    if (verbose) {
        grid_choice_print(choice, NULL);
    }

    grid_copy(g, &g1);
    choice.choice = '0';
    grid_choice_apply(&g1, choice);
    find_solution1(&g1, has_sol, sol);
    grid_free(&g1);

    grid_copy(g, &g2);
    choice.choice = '1';
    grid_choice_apply(&g2, choice);
    find_solution1(&g2, has_sol, sol);
    grid_free(&g2);

    return;
}

void find_solutionALL(t_grid* g, int* nb_sol, FILE* fd) {
    if (!is_consistent(g)) {
        return;
    }
    if (is_valid(g)) {
        (*nb_sol)++;
        if (fd != NULL) {
            grid_print(g, fd);
        }
        return;
    }

    apply_euristics(g);

    // here, g is still consistent but not valid, so we keep trying to find solutions
    t_grid g1, g2;
    choice_t choice = grid_choice(g);
    if (verbose) {
        grid_choice_print(choice, NULL);
    }

    grid_copy(g, &g1);
    choice.choice = '0';
    grid_choice_apply(&g1, choice);
    find_solutionALL(&g1, nb_sol, fd);
    grid_free(&g1);

    grid_copy(g, &g2);
    choice.choice = '1';
    grid_choice_apply(&g2, choice);
    find_solutionALL(&g2, nb_sol, fd);
    grid_free(&g2);

    return;
}

t_grid* grid_solver(t_grid* tosolve, const t_mode mode, FILE* fd) {

    bool has_sol = false;
    t_grid g;
    // copy because the first euristic is applied on the grid, we don't want to modify it.
    grid_copy(tosolve, &g);

    int nbsol = 0;
    t_grid* sol = malloc(sizeof(t_grid));
    if (sol == NULL) {
        perror("grid_solver: ");
    }

    switch (mode) {
        case MODE_FIRST:
            find_solution1(&g, &has_sol, sol);
            if (has_sol) {
                printf("solution found\n");
                return sol;
            }
            return NULL;

        case MODE_ALL:
            find_solutionALL(&g, &nbsol, fd);
            fprintf(fd, "Number of solutions : %d\n", nbsol);
            return NULL;
    }
    return NULL;
}