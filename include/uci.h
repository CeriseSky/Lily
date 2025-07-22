#ifndef _UCI_H_
#define _UCI_H_

#include <vector.h>

#define UCI_MAX_MOVE_LENGTH 5 // longest move is promotion (e.g. e7e8q)

typedef struct {
  const char *token;
  void (*handler)(vec_charptr *args);
} UCI_Command;

typedef struct {
  bool running;
} UCI_State;

void UCI_init();

extern UCI_State UCI_state;
extern UCI_Command UCI_commands[];
extern const size_t UCI_numCommands;

#endif

