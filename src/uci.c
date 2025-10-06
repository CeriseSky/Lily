#include <uci.h>
#include <stdio.h>
#include <lily.h>
#include <pthread.h>
#include <stdarg.h>

UCI_State UCI_state = { 0 };

void UCI_init() {
  UCI_state.running = true;
}

void UCI_send(char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  putchar('\n');
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

static void UCI_isready(vec_charptr *args) {
  UCI_sendRaw("readyok");
}

static void UCI_go(vec_charptr *args) {
  lily_ThinkThreadArgs state;
  state.thinking = LILY_NO_STATUS;
  state.board = &UCI_state.board;
  state.turn = UCI_state.turn;
  if(pthread_mutex_init(&state.thinkingLock, nullptr))
    return;

  if(pthread_create(&UCI_state.thinker, nullptr, lily_think_thread, &state))
    return;

  // I'll be honest I don't really know if the mutex is necessary but it's the
  // safest bet I think
  while(true) {
    pthread_mutex_lock(&state.thinkingLock);

    if(state.thinking == LILY_NOT_THINKING)
        return;
    if(state.thinking == LILY_THINKING)
      break;

    pthread_mutex_unlock(&state.thinkingLock);
  }

  pthread_mutex_destroy(&state.thinkingLock);
}

static void UCI_stop(vec_charptr *args) {
  lily_stop();
}

static void UCI_quit(vec_charptr *args) {
  UCI_stop(nullptr);
  UCI_state.running = false;
}

static void UCI_position(vec_charptr *args) {
  // TODO: support castling and en passant

  if(args->len < 2 || strcmp(args->storage[1], "startpos")) return;
  bitboard_Board_reset(&UCI_state.board);
  UCI_state.turn = MOVEGEN_TURN_WHITE;

  if(args->len >= 3 && strcmp(args->storage[2], "moves")) return;

  for(size_t i = 3; i < args->len; i++) {
    if(strlen(args->storage[i]) < 4) return;
    uint64_t src = bitboard_setSquare(0, args->storage[i][0] - 'a',
                                         args->storage[i][1] - '1');
    uint64_t dst = bitboard_setSquare(0, args->storage[i][2] - 'a',
                                         args->storage[i][3] - '1');
    UCI_state.turn = !UCI_state.turn;

    bitboard_Board_playMove(&UCI_state.board, src, dst);
  }
}

UCI_Command UCI_commands[] = {
  { .token = "uci", .handler = UCI_uci },
  { .token = "quit", .handler = UCI_quit },
  { .token = "isready", .handler = UCI_isready },
  { .token = "go", .handler = UCI_go },
  { .token = "stop", .handler = UCI_stop },
  { .token = "position", .handler = UCI_position },
};
const size_t UCI_numCommands = sizeof(UCI_commands) / sizeof(UCI_Command);

