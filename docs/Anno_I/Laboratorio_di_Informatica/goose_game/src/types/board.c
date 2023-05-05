// Copyright (c) 2023 @authors. GNU GPLv3.
// @authors
//    Amorese Emanuele
//    Blanco Lorenzo
//    Cannito Antonio
//    Fidanza Simone
//    Lecini Fabio

#include "./board.h"
#include <stdlib.h>

int get_square(const Board *board, int position) {
  return board->squares[position];
}
void set_square(Board *board, int position, int value) {
  board->squares[position] = value;
}

int get_dim(const Board *board) { return board->dim; }
void set_dim(Board *board, int dimension) { board->dim = dimension; }
