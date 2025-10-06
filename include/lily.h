#ifndef _LILY_H_
#define _LILY_H_

#include <uci.h>
#include <pthread.h>
#include <bitboard.h>
#include <movegen.h>

typedef enum {
  LILY_NO_STATUS = 0,
  LILY_NOT_THINKING,
  LILY_THINKING,
} lily_ThinkingState;

// initialised by caller
typedef struct {
  lily_ThinkingState thinking;
  pthread_mutex_t thinkingLock;
  bitboard_Board *board;
  movegen_Turn turn;
} lily_ThinkThreadArgs;

typedef void (*lily_PfnDestructor)(void *);
typedef struct lily_Allocation {
  void *memory;
  lily_PfnDestructor destructor;  // must NOT destroy the object, only memory
                                  // that it owns
  struct lily_Allocation *next;   // a hash map would be better for this than a
                                  // linked list, but that's for the future
} lily_Allocation;

void *lily_alloc(size_t size, lily_PfnDestructor destructor);
void lily_freeAll();
void lily_free(void *);

void lily_think();
void lily_sendMove(char bestMove[UCI_MAX_MOVE_LENGTH]);

void lily_stop();

// takes pointer to initialised lily_ThinkThreadArgs structure
void *lily_think_thread(void *arg);

#endif

