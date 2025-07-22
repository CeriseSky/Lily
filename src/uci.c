#include <uci.h>
#include <stdio.h>

UCI_State UCI_state = { 0 };

void UCI_init() {
  UCI_state.running = true;
}

static void UCI_send(vec_charptr *tokens) {
  for(size_t i = 0; i < tokens->len; i++)
    printf("%s ", tokens->storage[i]);
  printf("\n");
  fflush(stdout);
}

static void UCI_sendRaw(char *command) {
  printf("%s\n", command);
  fflush(stdout);
}

static void UCI_uci(vec_charptr *args) {
  UCI_sendRaw("id name Lily");
  UCI_sendRaw("id author CeriseSky");
  UCI_sendRaw("uciok");
}

static void UCI_quit(vec_charptr *args) {
  UCI_state.running = false;
}

static void UCI_isready(vec_charptr *args) {
  UCI_sendRaw("readyok");
}

static void UCI_go(vec_charptr *args) {
  static char *command = "bestmove";

  char *bestmove = malloc(UCI_MAX_MOVE_LENGTH + 1);
  bestmove[UCI_MAX_MOVE_LENGTH] = '\0';
  strcpy(bestmove, "0000");

  vec_charptr tokens;
  vec_charptr_new(&tokens);

  if(vec_charptr_push(&tokens, &command) < 0 ||
     vec_charptr_push(&tokens, &bestmove) < 0) {
    vec_charptr_delete(&tokens);
    return;
  }

  UCI_send(&tokens);
  vec_charptr_delete(&tokens);
  free(bestmove);
}

UCI_Command UCI_commands[] = {
  { .token = "uci", .handler = UCI_uci },
  { .token = "quit", .handler = UCI_quit },
  { .token = "isready", .handler = UCI_isready },
  { .token = "go", .handler = UCI_go },
};
const size_t UCI_numCommands = sizeof(UCI_commands) / sizeof(UCI_Command);

