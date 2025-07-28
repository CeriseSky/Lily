#ifndef _MOVEGEN_H_
#define _MOVEGEN_H_

#include <stdint.h>
#include <bitboard.h>
#include <vector.h>

// contains information for all valid moves a piece can make
typedef struct {
  uint64_t source;
  uint64_t destinations; // bitboard where all valid destination squares are set
} movegen_Move;

VEC_PROTO(movegen_Move)

#define MOVEGEN_TURN_WHITE false
#define MOVEGEN_TURN_BLACK true
typedef bool movegen_Turn;

// returns false if there was an error. If there was an error, the caller should
// assume the moves vector is corrupted and delete it
bool movegen_getAll(movegen_Turn, bitboard_Board *, vec_movegen_Move *);
bool movegen_getKnights(movegen_Turn, bitboard_Board *, vec_movegen_Move *);

#endif

