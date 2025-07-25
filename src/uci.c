#include <uci.h>
#include <stdio.h>
#include <lily.h>
#include <pthread.h>

UCI_State UCI_state = { 0 };

void UCI_init() {
  UCI_state.running = true;
}

void UCI_send(vec_charptr *tokens) {
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

static void UCI_isready(vec_charptr *args) {
  UCI_sendRaw("readyok");
}

static void UCI_go(vec_charptr *args) {
  lily_ThinkThreadArgs state;
  state.thinking = LILY_NO_STATUS;
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
}

void UCI_stop(vec_charptr *args) {
  lily_stop();
}

static void UCI_quit(vec_charptr *args) {
  UCI_stop(nullptr);
  UCI_state.running = false;
}

UCI_Command UCI_commands[] = {
  { .token = "uci", .handler = UCI_uci },
  { .token = "quit", .handler = UCI_quit },
  { .token = "isready", .handler = UCI_isready },
  { .token = "go", .handler = UCI_go },
  { .token = "stop", .handler = UCI_stop },
};
const size_t UCI_numCommands = sizeof(UCI_commands) / sizeof(UCI_Command);

