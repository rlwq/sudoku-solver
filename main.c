#include "stdbool.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"

#define BLK(deg_, p_) ((p_).col / (deg_) + (p_).row / (deg_) * (deg_))
#define CURR_EMPTY(s_) ((s_).empties[(s_).assigned_count])

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

bool can_assign(const SudokuState state[static 1], FieldPos p, size_t v) {
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

void print_sudoku(const SudokuState state[static 1],
                  const char alphabet[static state->deg2]) {
    for (size_t row = 0; row < state->deg2; row++) {
        for (size_t col = 0; col < state->deg2; col++) {
            printf("%c ", alphabet[state->field[row * state->deg2 + col]]);
        }
        printf("\n");
    }
    printf("\n");
}

void fill(SudokuState state[static 1]) {
    for (; state->assigned_count < state->empties_count; state->assigned_count++) {
        bool assigned = false;
        for (size_t c = 0; c < state->deg2; c++) {
            if (!can_assign(state, CURR_EMPTY(*state), c)) continue;
            assign_value(state, CURR_EMPTY(*state), c);
            assigned = true;
            break;
        }
        if (!assigned) break;
    }
}

void iterate(SudokuState state[static 1]) {
    assert(state->assigned_count > 0);

    bool assigned = true;
    do {
        assigned = false;
        state->assigned_count--;
        size_t row = CURR_EMPTY(*state).row,
               col = CURR_EMPTY(*state).col;
        size_t old_v = state->field[row * state->deg2 + col];
        unassign_value(state, CURR_EMPTY(*state), old_v);

        for (size_t new_v = old_v + 1; new_v < state->deg2; new_v++) {
            if (!can_assign(state, CURR_EMPTY(*state), new_v)) continue;
            assigned = true;
            assign_value(state, CURR_EMPTY(*state), new_v);
            state->assigned_count++;
            break; 
        }
    } while(!assigned && state->assigned_count > 0);
}

void search_solutions(SudokuState state[static 1],
                      size_t limit,
                      const char alphabet[static state->deg2]) {
    size_t solutions_found = 0;
    do {
        fill(state);
        if (state->assigned_count == state->empties_count) {
            print_sudoku(state, alphabet);
            solutions_found++;
        }
        iterate(state);
    } while (state->assigned_count && (!limit || solutions_found < limit));
}

// TODO: replace arrays of bools with bitmasks
// TODO: sort empty cells by the amount of possible values
// TODO: -f (format)
// TODO: assert if parsed char is not in alphabet or "Empty Cell Char" is in alphabet
// TODO: assert if input is invalid
int main(int argc, char **argv) {
    size_t deg = 3;
    size_t max_solutions = 0;
    char* alphabet = "123456789ABCDEFG";
    char empty_cell_char = '.'; 
    
    bool arg_err = false;
    for (int i = 1; i < argc; i++) {
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
        else if (!strcmp(argv[i], "-e")) {
            if (++i >= argc) arg_err = true;
            else empty_cell_char = argv[i][0];
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

    search_solutions(sudoku, max_solutions, alphabet);
    free_sudoku_state(sudoku);
    return 0;
}

