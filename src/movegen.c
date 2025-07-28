#include <movegen.h>

VEC_IMPL(movegen_Move)

bool movegen_getKnights(movegen_Turn turn, bitboard_Board *board,
                        vec_movegen_Move *moves) {
  bitboard_Side *myPieces = turn == MOVEGEN_TURN_WHITE ? &board->white :
                                                         &board->black;
  uint64_t myKnights = myPieces->knights;

  movegen_Move currentMove;
  while(myKnights) {
    uint8_t rank = bitboard_getRank(myPieces->knights);
    uint8_t file = bitboard_getFile(myPieces->knights);

    currentMove.source = bitboard_getMostSignificantMask(myKnights);
    currentMove.destinations = 0;

    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 4; j++) {
        // these lines create the L shaped pattern
        int fileOffset = (i ? 1 : 2) * (j / 2 ? -1 : 1);
        int rankOffset = (i ? 2 : 1) * (j % 2 ? -1 : 1);

        uint64_t target = bitboard_setSquare(0, file + fileOffset,
                                             rank + rankOffset);
        if( !(target & myPieces->allPieces) )
          currentMove.destinations |= target;
      }

    if(currentMove.destinations != 0 &&
       vec_movegen_Move_push(moves, &currentMove) < 0)
      return false;

    myKnights &= ~currentMove.source;
  }

  return true;
}

// returns false if there was an error. If there was an error, the caller should
// assume the moves vector is corrupted and delete it
bool movegen_getAll(movegen_Turn turn, bitboard_Board *board,
                    vec_movegen_Move *moves) {
  if(!movegen_getKnights(turn, board, moves)) return false;

  return true;
}

