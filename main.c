#include "stdbool.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"

#define BLK(deg_, p_) ((p_).col / (deg_) + (p_).row / (deg_) * (deg_))

typedef struct {
    size_t row, col;
} FieldPos;

typedef struct {
    size_t deg, deg2, deg4;
    size_t *field;
    FieldPos *empty_cells;
    size_t empty_cells_count;
    bool *rows_opts, *cols_opts, *blks_opts;
} SudokuState;

void free_sudoku_state(SudokuState *state) {
    free(state->rows_opts);
    free(state->cols_opts);
    free(state->blks_opts);
    free(state->field);
    free(state->empty_cells);
    free(state);
}

SudokuState *alloc_sudoku_state(size_t deg) {
    SudokuState *state = malloc(sizeof(SudokuState));

    if(!state) return NULL;

    state->deg = deg;
    state->deg2 = deg * deg;
    state->deg4 = deg * deg * deg * deg;
    state->empty_cells_count = 0;

    state->field = malloc(sizeof(size_t) * state->deg4);
    state->rows_opts = malloc(sizeof(bool) * state->deg4);
    state->cols_opts = malloc(sizeof(bool) * state->deg4);
    state->blks_opts = malloc(sizeof(bool) * state->deg4);
    state->empty_cells = malloc(sizeof(FieldPos) * state->deg4);

    if (!(state->field && state->rows_opts &&
          state->cols_opts && state->blks_opts &&
          state->empty_cells)) {
        free_sudoku_state(state);
        return NULL;
    }

    for (size_t i = 0; i < state->deg4; i++) {
        state->rows_opts[i] = true;
        state->cols_opts[i] = true;
        state->blks_opts[i] = true;
    }

    return state;
}

bool is_possible(const SudokuState state[static 1], FieldPos p, size_t v) {
    return state->rows_opts[p.row * state->deg2 + v] &&
        state->cols_opts[p.col * state->deg2 + v] &&
        state->blks_opts[BLK(state->deg, p) * state->deg2 + v];
}

void assign_value(SudokuState state[static 1], FieldPos p, size_t v) {
    state->rows_opts[p.row * state->deg2 + v] = false;
    state->cols_opts[p.col * state->deg2 + v] = false;
    state->blks_opts[BLK(state->deg, p) * state->deg2 + v] = false;
    state->field[p.row * state->deg2 + p.col] = v;
}

void unassign_value(SudokuState state[static 1], FieldPos p, size_t v) {
    state->rows_opts[p.row * state->deg2 + v] = true;
    state->cols_opts[p.col * state->deg2 + v] = true;
    state->blks_opts[BLK(state->deg, p) * state->deg2 + v] = true;
}

void print_sudoku(const SudokuState state[static 1]) {
    for (size_t row = 0; row < state->deg2; row++) {
        for (size_t col = 0; col < state->deg2; col++)
            printf("%zu ", state->field[row * state->deg2 + col] + 1);
        printf("\n");
    }
    printf("\n");
}

void dfs(SudokuState state[static 1], size_t curr) {
    if (curr == state->empty_cells_count) {
        print_sudoku(state);
        return;
    }

    for (size_t c = 0; c < state->deg2; c++) {
        if (!is_possible(state, state->empty_cells[curr], c))
            continue;
        assign_value(state, state->empty_cells[curr], c);
        dfs(state, curr + 1);
        unassign_value(state, state->empty_cells[curr], c);
    }
}

int main(int argc, char **argv) {
    size_t deg = 3;
    char *default_alphabet = "123456789ABCDEFG";

    bool arg_err = false;
    for (size_t i = 1; i < argc; i++) {
        if (argv[i][0] != '-') arg_err = true;

        if (argv[i][1] == 'd') {
            i++;
            deg = atoll(argv[i]);
        } else arg_err = true;
    }

    SudokuState *sudoku = alloc_sudoku_state(deg);
    for (size_t row = 0; row < sudoku->deg2; row++) {
        for (size_t col = 0; col < sudoku->deg2; col++) {
            char c;
            scanf(" %c", &c);
            if (c == '.') {
                sudoku->empty_cells[sudoku->empty_cells_count++] = (FieldPos) { .row = row, .col = col };
                continue;
            }

            size_t cv = c - '1';
            assign_value(sudoku, (FieldPos) { .row = row, .col = col }, cv);
        }
    }

    dfs(sudoku, 0);
    free_sudoku_state(sudoku);
    return 0;
}

