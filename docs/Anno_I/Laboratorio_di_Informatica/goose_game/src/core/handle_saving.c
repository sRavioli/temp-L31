// Copyright (c) 2023 @authors. GNU GPLv3
// @authors
//    Amorese Emanuele
//    Blanco Lorenzo
//    Cannito Antonio
//    Fidanza Simone
//    Lecini Fabio

#include "../common/inc/types/board.h"
#include "../common/inc/types/gamestate.h"
#include "../common/inc/types/gamestates.h"
#include "../common/inc/types/player.h"
#include "../common/inc/types/players.h"

#include "../common/inc/error.h"
#include "../common/inc/logger.h"
#include "../common/inc/term.h"

#include "../inc/globals.h"

#include "../inc/handle_game.h"
#include "../inc/handle_saving.h"
#include "../inc/private/handle_saving.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *BORDERS[8] = DEFAULT_BORDERS;

// for debugging purposes
void print_gamestates(GameStates gss) {
  printf("GameStates gss: {\n");
  printf("  saved games: %i\n", gss.num_games);
  int i = 0;
  while (i < gss.num_games) {
    GameState gs = gss.gamestates[i];
    Board board = gss.gamestates[i].board;
    Players pls = gss.gamestates[i].pls;
    printf("  GameState[] gss.gamestates[%i]: {\n", i);
    printf("    game name: '%s'\n", gs.game_name);
    printf("    Board gss.gamestates[%i].board: {\n", i);
    printf("      dimension: %i\n", board.dim);
    printf("      squares[]: [");
    int j = 0;
    while (j < board.dim) {
      printf("%i", board.squares[j]);
      if (j < board.dim - 1) {
        printf(", ");
      }
      j = j + 1;
    }
    printf("]\n");
    printf("    }\n");
    printf("    Players[] gss.gamestates[%i].pls: {\n", i);
    printf("      players number: %i\n", pls.players_num);
    int k = 0;
    while (k < pls.players_num) {
      printf("      Player gss.gamestates[%i].pls.players[%i]: {\n", i, k);
      printf("        id: %i\n", pls.players[k].id);
      printf("        score: %i\n", pls.players[k].score);
      printf("        position: %i\n", pls.players[k].position);
      printf("        turns blocked: %i\n", pls.players[k].turns_blocked);
      printf("        username: %s\n", pls.players[k].username);
      k = k + 1;
    }
    printf("    }\n");
    printf("  }\n");
    i = i + 1;
  }
  printf("}\n");

  wait_keypress("press to continue");
}

int is_file_empty(FILE *fp) {
  logger.enter_fn(__func__);
  logger.log("checking if the file is empty");

  int res = FALSE;
  fseek(fp, 0L, SEEK_END);  // move to the end of the file
  if (ftell(fp) == 0L) {
    logger.log("file is empty");
    res = TRUE;
  }
  fseek(fp, 0L, SEEK_SET);  // restore pointer position to top
  logger.log("restored pointer position");

  logger.exit_fn();
  return res;
}

int choose_save(GameStates gss) {
  logger.enter_fn(__func__);
  logger.log("attempting to ask user to choose a save");

  printf("Please select a save from the following ones:\n");
  int num_saves = get_num_games(&gss);
  int i = 0;
  while (i < num_saves) {
    GameState gs = *get_gamestate(&gss, i);
    Players pls = get_players(&gs);
    Board board = get_board(&gs);

    printf("%i) \"%s\":\n", i, get_game_name(&gs));
    printf("    * players (%i): [", get_players_num(&pls));
    int j = 0;
    while (j < get_players_num(&pls)) {
      Player pl = *get_player(&pls, j);
      printf("%s (pos. %i)", get_username(&pl), get_position(&pl));
      if (j < get_players_num(&pls) - 1) {
        printf(", ");
      }
      j = j + 1;
    }
    printf("]\n");
    printf("    * board with %i squares\n", get_dim(&board));
    i = i + 1;
  }

  printf("> ");
  int idx;
  char input[MAX_BUFFER_LEN];
  do {
    fgets(input, MAX_BUFFER_LEN - 1, stdin);
    input[strlen(input) - 1] = '\0';
    idx = atoi(input);

    if (idx < NO_SAVED_GAMES || idx > num_saves - 1) {
      clear_line();
      printf("That is not a valid game, retry.\n");
      printf("> ");
    }
  } while (idx < NO_SAVED_GAMES || idx > num_saves - 1);
  wait_keypress("%i", idx);

  logger.exit_fn();
  return idx;
}

void read_saves(GameStates *gss) {
  logger.enter_fn(__func__);
  logger.log("attempting to read saves");

  FILE *fp;
  if (fopen_s(&fp, SAVED_GAMES_FILE, "rb")) {
    logger.log("file is not readable");
    throw_err(FILE_NOT_READABLE_ERROR);
  }

  if (is_file_empty(fp)) {
    logger.log("no saves in file");
    set_num_games(gss, NO_SAVED_GAMES);
  } else {
    logger.log("reading saves from file");
    fread(gss, sizeof(*gss), 1, fp);
  }
  fclose(fp);

  // print_gamestates(*gss);

  logger.exit_fn();
}

void write_save(GameState gs) {
  logger.enter_fn(__func__);
  logger.log("attempting to save current game");

  GameStates file_gss;
  read_saves(&file_gss);

  int num_saves = get_num_games(&file_gss);
  logger.log("currently present %i saves", num_saves);

  if (num_saves >= NO_SAVED_GAMES && num_saves < MAX_SAVED_GAMES) {
    logger.log("num_saves is inside bounds, appending to struct");
    set_gamestate(&file_gss, gs, get_num_games(&file_gss));
    set_num_games(&file_gss, get_num_games(&file_gss) + 1);
  } else if (num_saves == MAX_SAVED_GAMES) {
    logger.log("max number of saves reached, asking game to overwrite");
    new_screen();
    printf("Max number of saves reached, you need to choose a game to delete");

    int index = choose_save(file_gss);
    set_gamestate(&file_gss, gs, index);  // overwrite at index
  }

  FILE *fp;
  if (fopen_s(&fp, SAVED_GAMES_FILE, "wb")) {
    logger.log("file is not writable");
    throw_err(FILE_NOT_WRITABLE_ERROR);
  }
  fwrite(&file_gss, sizeof(file_gss), 1, fp);
  fclose(fp);

  logger.log("writing saves to file");

  // print_gamestates(file_gss);  // debug
  logger.exit_fn();
}

void save_game(Players *pls, Board *board) {
  char game_name[MAX_BUFFER_LEN];
  printf("Insert the name of the save: ");
  fgets(game_name, MAX_BUFFER_LEN, stdin);
  game_name[strlen(game_name) - 1] = '\0';

  GameState gs;
  set_players(&gs, pls);
  set_board(&gs, board);
  set_game_name(&gs, game_name);

  write_save(gs);
}

void saved_games() {
  logger.enter_fn(__func__);
  logger.log("checking if saves are present");

  GameStates gss;
  read_saves(&gss);

  if (get_num_games(&gss) == 0) {
    printf("No saved games found!");
  } else if (get_num_games(&gss) == 1) {
    printf("Found only the following game: ");
    // only one game, launch that one (print message)
    printf("launch this game? (y/n) ");
  } else {
    new_screen();
    printf("Many games found. ");
    int index = choose_save(gss);

    GameState gs = *get_gamestate(&gss, index);
    Players pls = get_players(&gs);
    Board board = get_board(&gs);

    const char *game_board =
        build_board(get_board(&gs), DEFAULT_COLS, DEFAULT_SQUARE_LEN, BORDERS);

    game_loop(&pls, &board, game_board);
  }

  logger.exit_fn();
}
