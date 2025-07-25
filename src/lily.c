#include <lily.h>

struct {
  bool thinking;
  char *currentBestMove;
  pthread_t thinkerThread;
} lily_state = { 0 };

void *lily_think_thread(void *arg) {
  lily_ThinkThreadArgs *options = arg;

  pthread_mutex_lock(&options->thinkingLock);
  lily_state.thinkerThread = pthread_self();

  if(lily_state.thinking) {
    options->thinking = LILY_NOT_THINKING;
    pthread_exit(nullptr);
  }

  options->thinking = LILY_THINKING;
  pthread_mutex_unlock(&options->thinkingLock);

  lily_state.thinking = true;
  lily_think();
  lily_stop();

  return nullptr;
}

void lily_think() {
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);

  lily_state.currentBestMove = malloc(UCI_MAX_MOVE_LENGTH + 1);
  strcpy(lily_state.currentBestMove, "0000");

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
}

void lily_stop() {
  if(!lily_state.thinking)
    return;

  if(pthread_self() != lily_state.thinkerThread)
    pthread_cancel(lily_state.thinkerThread);

  lily_state.thinking = false;
  lily_sendMove(lily_state.currentBestMove);
  free(lily_state.currentBestMove);
}

void lily_sendMove(char bestmove[UCI_MAX_MOVE_LENGTH]) {
  static char *command = "bestmove";

  vec_charptr tokens;
  vec_charptr_new(&tokens);

  if(vec_charptr_push(&tokens, &command) < 0 ||
     vec_charptr_push(&tokens, &bestmove) < 0) {
    vec_charptr_delete(&tokens);
    free(bestmove);
    return;
  }

  UCI_send(&tokens);
  vec_charptr_delete(&tokens);
}

