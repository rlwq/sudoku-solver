# Sudoku solver
A simple and practical sudoku solver for boards of any size.
# Build
The building process is straightforward.
```console
$ g++ main.cpp -o solver
```
# Usage
```console
solver <filename> [flags]
```
### Flags:

|            Flag | Default value | Description |
|----------------:|--------------:|:------------|
| `-o <filename>` |  "result.txt" | specifies the output file. |
|   `-c <number>` |             0 | determines the limit on the number of solutions.</br>Type 0 for removing the limit. |
|   `-d <number>` |             3 | determines the dimension of the sudoku.</br>A standard sudoku has a dimension of 3 |

#### Note: `-c` and `-d` flags are not implemented yet.