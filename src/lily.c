#include <lily.h>
#include <movegen.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

struct {
  bool thinking;
  char *currentBestMove;
  pthread_t thinkerThread;
  lily_Allocation *allocations;
  pthread_mutex_t allocationLock;
} lily_state = { 0 };

void *lily_think_thread(void *arg) {
  lily_ThinkThreadArgs *options = arg;

  pthread_mutex_lock(&options->thinkingLock);
  lily_state.thinkerThread = pthread_self();

  pthread_mutexattr_t allocationLockAttrs;
  pthread_mutexattr_init(&allocationLockAttrs);
  pthread_mutexattr_settype(&allocationLockAttrs, PTHREAD_MUTEX_RECURSIVE);
  int allocationLock = pthread_mutex_init(&lily_state.allocationLock,
                                          &allocationLockAttrs);
  pthread_mutexattr_destroy(&allocationLockAttrs);

  if(lily_state.thinking || allocationLock != 0) {
    options->thinking = LILY_NOT_THINKING;
    pthread_mutex_unlock(&options->thinkingLock);
    pthread_exit(nullptr);
  }

  options->thinking = LILY_THINKING;
  pthread_mutex_unlock(&options->thinkingLock);

  lily_state.allocations = nullptr;
  lily_state.thinking = true;
  lily_think();

  lily_stop();
  return nullptr;
}

static size_t lily_enumerateMoves(movegen_Turn turn, 
                                  bitboard_Board *board, size_t depth) {
  if(!depth) return 0;

  vec_movegen_Move *moves = lily_alloc(
    sizeof(vec_movegen_Move),
    (lily_PfnDestructor)vec_movegen_Move_delete);
  if(!moves)
    return 0;

  vec_movegen_Move_new(moves);

  // silently returning probably isn't the best idea
  // since it makes lily skip out moves without realising
  if(!movegen_getAll(turn, board, moves)) {
    lily_free(moves);
    return 0;
  }

  size_t count = 0;
  for(size_t i = 0; i < moves->len; i++) {
    uint64_t currentMove;

    while( (currentMove =
            bitboard_getMostSignificantMask(moves->storage[i].destinations)) ) {
      bitboard_Board newBoard;
      memcpy(&newBoard, board, sizeof(newBoard));
      bitboard_Board_playMove(&newBoard, moves->storage[i].source, currentMove);

      // +1 to count the current move as well as the future moves
      count += 1 + lily_enumerateMoves(!turn, &newBoard, depth - 1);

      moves->storage[i].destinations &= ~currentMove;
    }
  }

  lily_free(moves);
  return count;
}

void lily_think() {
  lily_state.currentBestMove = malloc(UCI_MAX_MOVE_LENGTH + 1);
  strcpy(lily_state.currentBestMove, "0000");

  bitboard_Board board;
  bitboard_Board_reset(&board);
  constexpr size_t maxDepth = 100;
  for(int i = 0; i < maxDepth; i++) {
    clock_t start = clock();
    size_t numMoves = lily_enumerateMoves(MOVEGEN_TURN_WHITE, &board, i);
    double time = (double)(clock() - start) / (double)CLOCKS_PER_SEC;

    // UCI_send doesn't support va_lists yet
    UCI_send("info depth %i time %lu nps %lu nodes %lu", i, (uint64_t)(1000.0 * time),
             (uint64_t)(numMoves / time), numMoves);
  }
}

void lily_stop() {
  pthread_testcancel();
  if(!lily_state.thinking)
    return;

  pthread_mutex_lock(&lily_state.allocationLock);
  if(pthread_self() != lily_state.thinkerThread)
    pthread_cancel(lily_state.thinkerThread);

  lily_state.thinking = false;
  lily_sendMove(lily_state.currentBestMove);

  free(lily_state.currentBestMove);
  pthread_mutex_destroy(&lily_state.allocationLock);

  lily_freeAll();
}

void lily_sendMove(char bestmove[UCI_MAX_MOVE_LENGTH]) {
  UCI_send("bestmove %s", bestmove);
}

void lily_freeAll() {
  pthread_testcancel();
  pthread_mutex_lock(&lily_state.allocationLock);
  while(lily_state.allocations)
    lily_free(lily_state.allocations->memory);
  lily_state.allocations = nullptr;
  pthread_mutex_unlock(&lily_state.allocationLock);
}

void lily_free(void *memory) {
  pthread_testcancel();
  pthread_mutex_lock(&lily_state.allocationLock);
  lily_Allocation *previous = nullptr, *current = lily_state.allocations;
  bool foundNode = false;
  while(current) {
    if(current->memory == memory) {
      foundNode = true;
      break;
    }

    previous = current;
    current = current->next;
  }

  if(!foundNode) {
    pthread_mutex_unlock(&lily_state.allocationLock);
    return;
  }

  if(previous)
    previous->next = current->next;
  else
    lily_state.allocations = current->next;

  if(current->destructor)
    current->destructor(current->memory);

  free(current->memory);
  free(current);
  pthread_mutex_unlock(&lily_state.allocationLock);
}

void *lily_alloc(size_t size, lily_PfnDestructor destructor) {
  pthread_testcancel();
  pthread_mutex_lock(&lily_state.allocationLock);
  lily_Allocation *newHead = malloc(sizeof(lily_Allocation));
  if(!newHead)
    return nullptr;

  newHead->memory = malloc(size);
  if(!newHead->memory) {
    free(newHead);
    return nullptr;
  }

  newHead->next = lily_state.allocations;
  lily_state.allocations = newHead;

  newHead->destructor = destructor;

  pthread_mutex_unlock(&lily_state.allocationLock);
  return newHead->memory;
}

