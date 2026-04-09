SRC = main.c
CC_FLAGS = -Wall -Wextra
TARGET = sudoku

build: $(SRC)
	cc $(SRC) $(CC_FLAGS) -DNDEBUG -O3 -o $(TARGET)

debug: $(SRC)
	cc $(SRC) $(CC_FLAGS) -Werror -g -o $(TARGET) 

.PHONY: build debug
