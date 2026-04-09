#include "stdbool.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define BLK(deg_, p_) ((p_).col / (deg_) + (p_).row / (deg_) * (deg_))

typedef struct {
    size_t row, col;
} FieldPos;

typedef struct {
    size_t deg, deg2, deg4;
    size_t *field;
    FieldPos *empties;
    size_t empties_count;
    size_t assigned_count;
    bool *rows_opts, *cols_opts, *blks_opts;
} SudokuState;

void free_sudoku_state(SudokuState *state) {
    free(state->rows_opts);
    free(state->cols_opts);
    free(state->blks_opts);
    free(state->field);
    free(state->empties);
    free(state);
}

SudokuState *alloc_sudoku_state(size_t deg) {
    SudokuState *state = malloc(sizeof(SudokuState));

    if(!state) return NULL;

    state->deg = deg;
    state->deg2 = deg * deg;
    state->deg4 = deg * deg * deg * deg;
    state->empties_count = 0;
    state->assigned_count = 0;

    state->field = malloc(sizeof(size_t) * state->deg4);
    state->rows_opts = malloc(sizeof(bool) * state->deg4);
    state->cols_opts = malloc(sizeof(bool) * state->deg4);
    state->blks_opts = malloc(sizeof(bool) * state->deg4);
    state->empties = malloc(sizeof(FieldPos) * state->deg4);

    if (!(state->field && state->rows_opts &&
          state->cols_opts && state->blks_opts &&
          state->empties)) {
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

size_t from_alphabet(char c, const char *alphabet) {
    size_t i = 0;
    while (alphabet[i] != c && alphabet[i] != '\0') i++;
    return i;
}

size_t to_alphabet(size_t v, const char *alphabet) {
    return alphabet[v];
}

void print_sudoku(const SudokuState state[static 1]) {
    for (size_t row = 0; row < state->deg2; row++) {
        for (size_t col = 0; col < state->deg2; col++) {
            // TODO: rewrite this using to_alphabet();
            printf("%zu ", state->field[row * state->deg2 + col] + 1);
        }
        printf("\n");
    }
    printf("\n");
}

void fill(SudokuState state[static 1]) {
    for (; state->assigned_count < state->empties_count; state->assigned_count++) {
        bool can_assign = false;
        for (size_t c = 0; c < state->deg2; c++) {
            if (!is_possible(state, state->empties[state->assigned_count], c)) continue;
            assign_value(state, state->empties[state->assigned_count], c);
            can_assign = true;
            break;
        }
        if (!can_assign) break;
    }
}

void iterate(SudokuState state[static 1]) {
    bool assigned = true;
    do {
        assigned = false;
        state->assigned_count--; //  TODO: unsafe
        size_t row = state->empties[state->assigned_count].row,
               col = state->empties[state->assigned_count].col;
        size_t old_v = state->field[row * state->deg2 + col];
        unassign_value(state, state->empties[state->assigned_count], old_v);

        for (size_t new_v = old_v + 1; new_v < state->deg2; new_v++) {
            if (!is_possible(state, state->empties[state->assigned_count], new_v)) continue;
            assigned = true;
            assign_value(state, state->empties[state->assigned_count], new_v);
            state->assigned_count++;
            break; 
        }
    } while(!assigned && state->assigned_count > 0);
}

void search_solutions(SudokuState state[static 1]) {
    do {
        fill(state);
        if (state->assigned_count == state->empties_count) print_sudoku(state);
        iterate(state);
    } while (state->assigned_count);
}

int main(int argc, char **argv) {
    size_t deg = 3;
    size_t max_solutions = 10;
    char* alphabet = "123456789ABCDEFG";
    char empty_cell_char = '.'; 
    
    bool arg_err = false;
    for (size_t i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-d")) {
            if (++i >= argc) arg_err = true;
            else deg = atoi(argv[i]);  // TODO: use strtoul instead
        } 
        else if (!strcmp(argv[i], "-a")) {
            if (++i >= argc) arg_err = true;
            else alphabet = argv[i];
        }
        else if (!strcmp(argv[i], "-m")) {
            if (++i >= argc) arg_err = true;
            else max_solutions = atoi(argv[i]);  // TODO: use strtoul instead
        }
        else {
            arg_err = true;
            break;
        }
    }

    if (arg_err) {
        printf("USAGE: ...\n");
        return 1;
    }

    SudokuState *sudoku = alloc_sudoku_state(deg);

    if (!sudoku) return 1;

    for (size_t row = 0; row < sudoku->deg2; row++) {
        for (size_t col = 0; col < sudoku->deg2; col++) {
            char c;
            scanf(" %c", &c);
            if (c == empty_cell_char) {
                sudoku->empties[sudoku->empties_count++] = (FieldPos) { .row = row, .col = col };
                continue;
            }

            assign_value(sudoku, (FieldPos) { .row = row, .col = col }, from_alphabet(c, alphabet));
        }
    }

    search_solutions(sudoku);
    free_sudoku_state(sudoku);
    return 0;
}

