#ifndef BACKTRACKING_H
#define BACKTRACKING_H

#include <stdlib.h>

#include "grid.h"

typedef enum { MODE_FIRST, MODE_ALL } t_mode;

typedef struct {
    int row;
    int column;
    char choice;
} choice_t;

t_grid* grid_solver(t_grid*, const t_mode, FILE*);
void find_solutionALL(t_grid*, int*, FILE*);
void find_solution1(t_grid*, bool*, t_grid*);

#endif /*BACKTRACKING_H*/