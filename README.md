# Sudoku solver

A simple and practical sudoku solver for boards of any size.

## Build

The building process is straightforward.

```console
$ g++ main.cpp -o solver
```

## Usage

```console
$ solver <input_filename> [flags]
```

### Flags:

| Flag            | Default value | Description                                                                     |
| :-------------- | :------------ | :------------------------------------------------------------------------------ |
| `-o <filename>` | "result.txt"  | Specifies the output file.                                                      |
| `-c <integer>`  | 0             | Determines the maximum number of solutions. Type 0 for removing the limit.      |
| `-d <integer>`  | 3             | Determines the dimension of the sudoku. A standard sudoku has a dimension of 3. |

### Board file

Your board file (which represents the Sudoku board) should consist of $d^4$ characters of the alphabet `123456789abc...xyzABC...XYZ` which represent "hints" and the `.` character which represents an empty cell. Whitespace characters are ignored.

Here are two examples of valid boards for $d=2$:

```
1 . . .
2 3 . 4
. . 3 2
. . . .
```

```
2.....2...344.1.
```
