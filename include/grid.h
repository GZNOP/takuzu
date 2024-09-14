#ifndef GRID_H
#define GRID_H

#define MIN_GRID_SIZE 4
#define MAX_GRID_SIZE 64
#define N             20

#include <stdbool.h>
#include <stdio.h>

extern bool verbose;

typedef struct {
    int size;    // Number of elements in a row
    char** grid; // Pointer of the grid
} t_grid;

void grid_allocate(t_grid*, int);

void grid_free(t_grid*);

void grid_print(t_grid*, FILE*);

void file_parser(t_grid*, char*);

void grid_copy(t_grid*, t_grid*);

void set_cell(int, int, t_grid*, char);

char get_cell(int, int, t_grid*);

void check_bound_error(int, int, t_grid*);

bool is_consistent(t_grid*);

bool is_valid(t_grid*);

void fill_grid(t_grid*);

void set_empty_grid(t_grid*);

#endif /* GRID_H */