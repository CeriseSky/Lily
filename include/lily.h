#ifndef _LILY_H_
#define _LILY_H_

#include <uci.h>
#include <pthread.h>

typedef enum {
  LILY_NO_STATUS = 0,
  LILY_NOT_THINKING,
  LILY_THINKING,
} lily_ThinkingState;

// initialised by caller
typedef struct {
  lily_ThinkingState thinking;
  pthread_mutex_t thinkingLock;
} lily_ThinkThreadArgs;

void lily_think();
void lily_sendMove(char bestMove[UCI_MAX_MOVE_LENGTH]);

void lily_stop();

// takes pointer to initialised lily_ThinkThreadArgs structure
void *lily_think_thread(void *arg);

#endif

