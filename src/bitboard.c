#include <bitboard.h>

void bitboard_Board_reset(bitboard_Board *self) {
  self->white.pawns = BITBOARD_WHITE_PAWNS_START;
  self->white.rooks = BITBOARD_WHITE_ROOKS_START;
  self->white.knights = BITBOARD_WHITE_KNIGHTS_START;
  self->white.bishops = BITBOARD_WHITE_BISHOPS_START;
  self->white.queens = BITBOARD_WHITE_QUEENS_START;
  self->white.king = BITBOARD_WHITE_KING_START;
  bitboard_Side_update(&self->white);

  self->black.pawns = BITBOARD_BLACK_PAWNS_START;
  self->black.rooks = BITBOARD_BLACK_ROOKS_START;
  self->black.knights = BITBOARD_BLACK_KNIGHTS_START;
  self->black.bishops = BITBOARD_BLACK_BISHOPS_START;
  self->black.queens = BITBOARD_BLACK_QUEENS_START;
  self->black.king = BITBOARD_BLACK_KING_START;
  bitboard_Side_update(&self->black);
}

void bitboard_Side_update(bitboard_Side *self) {
  self->allPieces = self->pawns |
                    self->rooks |
                    self->knights |
                    self->bishops |
                    self->queens |
                    self->king;
}

uint8_t bitboard_getRank(uint64_t board) {
  return bitboard_getMostSignificant(board) / 8;
}

uint8_t bitboard_getFile(uint64_t board) {
  return bitboard_getMostSignificant(board) % 8;
}

// returns the bit number of the most significant bit
// (to get the bitboard: 1 << bitboard_getMostSignificant(b))
uint8_t bitboard_getMostSignificant(uint64_t board) {
  uint8_t index = 0;
  while(board >>= 1)
    index++;
  return index;
}

uint64_t bitboard_setSquare(uint64_t original, uint8_t file, uint8_t rank) {
  if(rank >= 8 || file >= 8)
    return original;

  uint8_t exponent = rank * 8 + file;
  uint64_t locationBit = (uint64_t)1 << exponent;

  original |= locationBit;
  return original;
}

// returns the number of bits set in the bitboard
size_t bitboard_countPieces(uint64_t board) {
  size_t count = 0;

  for(int i = 0; i < 64; i++)
    if( (uint64_t)1 << i & board )
      count++;

  return count;
}

uint64_t bitboard_getMostSignificantMask(uint64_t board) {
  return board ? (uint64_t)1 << bitboard_getMostSignificant(board) : 0;
}

uint64_t bitboard_playMove(uint64_t original, uint64_t src, uint64_t dst) {
  if(original & src) {
    original &= ~src;
    original |= dst;
  }
  return original;
}

void bitboard_Board_playMove(bitboard_Board *board,
                             uint64_t src, uint64_t dst) {
  bitboard_Side_playMove(&board->white, src, dst);
  bitboard_Side_playMove(&board->black, src, dst);
}

void bitboard_Side_playMove(bitboard_Side *side, uint64_t src, uint64_t dst) {
  side->pawns = bitboard_playMove(side->pawns, src, dst);
  side->rooks = bitboard_playMove(side->rooks, src, dst);
  side->knights = bitboard_playMove(side->knights, src, dst);
  side->bishops = bitboard_playMove(side->bishops, src, dst);
  side->queens = bitboard_playMove(side->queens, src, dst);
  side->king = bitboard_playMove(side->king, src, dst);
  bitboard_Side_update(side);
}

