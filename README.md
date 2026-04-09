# Sudoku Solver

A fast, simple and practical sudoku solver for boards of any size.

## Build

The building process is straightforward.

```bash
make build
```

## Usage

* Run the program and enter the board manually:
  ```bash
  sudoku [flags]
  ```

* ...or use piping to enter the board from a file:
  ```bash
  cat ./board.txt | sudoku [flags]
  ```


### Flags

| Flag          | Default value        | Description                                                        |
| ------------- | -------------------- | ------------------------------------------------------------------ |
| `-d <int>`    | `3`                  | Sudoku degree (block size). A standard sudoku has a degree of `3`. |
| `-m <int>`    | `0`                  | Maximum solutions to find. Use `-m 0` to remove the limit.         |
| `-e <char>`   | `'.'`                | Character used to indicate an empty cell.                          |
| `-a <string>` | `"123456789ABCDEFG"` | Characters used in the Sudoku board.                               |

### Board format

The expected Sudoku board should consist of $d^4$ non-whitespace characters of the specified alphabet and the character of an empty cell. Whitespace characters are ignored.

Here are two equvalent examples of valid boards for `degree 2`:

```plain
1 .  . .
2 3  . 4

. .  3 2
. .  . .
```

```plain
2.....2...344.1.
```
