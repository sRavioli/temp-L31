// Copyright (c) 2023 @authors. GNU GPLv3.
// @authors
//    Amorese Emanuele
//    Blanco Lorenzo
//    Cannito Antonio
//    Fidanza Simone
//    Lecini Fabio

#include "../inc/math.h"

int proportion(const int numerator, const int known_value,
               const int denominator) {
  return (numerator * known_value) / denominator;
}

int roll_dice() {
  return (rand() % MAX_DICE_THROW + MIN_DICE_THROW) +  // NOLINT
         (rand() % MAX_DICE_THROW + MIN_DICE_THROW);   // NOLINT
}

void swap_int(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}
