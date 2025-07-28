#ifndef _BITBOARD_H_
#define _BITBOARD_H_

#include <stdint.h>
#include <stddef.h>

/*
 * bitboard structure:
 * bit = 1 << (8 * rank + file)
 * where rank ranges from 0 to 7 (0 = 1st rank; 7 = 8th rank)
 * and file also ranges from 0 to 7 (0 = rank a; 7 = rank h)
 *
 * this has a small caveat that to humans, the files appear to be backwards.
 * This is because the leftmost files get smaller bits, so it looks like they
 * are on the right. This is only really a concern for the starting square
 * constants where the bitboard states are set manually
*/

typedef struct {
  uint64_t pawns;
  uint64_t rooks;
  uint64_t knights;
  uint64_t bishops;
  uint64_t queens;
  uint64_t king;
  uint64_t allPieces;
} bitboard_Side;

#define BITBOARD_WHITE_PAWNS_START   0xff'00
#define BITBOARD_WHITE_ROOKS_START   0b10000001
#define BITBOARD_WHITE_KNIGHTS_START 0b01000010
#define BITBOARD_WHITE_BISHOPS_START 0b00100100
#define BITBOARD_WHITE_QUEENS_START  0b00001000
#define BITBOARD_WHITE_KING_START    0b00010000

#define BITBOARD_BLACK_PAWNS_START   0x00'ff'000000000000
#define BITBOARD_BLACK_ROOKS_START   0x81'00'000000000000
#define BITBOARD_BLACK_KNIGHTS_START 0x42'00'000000000000
#define BITBOARD_BLACK_BISHOPS_START 0x24'00'000000000000
#define BITBOARD_BLACK_QUEENS_START  0x08'00'000000000000
#define BITBOARD_BLACK_KING_START    0x40'00'000000000000

typedef struct {
  bitboard_Side white;
  bitboard_Side black;
} bitboard_Board;

// returns the bitboard with the requested square set, or-ed with the
// current bitboard. If the requested square is invalid, the original board
// is returned
uint64_t bitboard_setSquare(uint64_t original, uint8_t file, uint8_t rank);

// sets the every piece in the board to the starting position
void bitboard_Board_reset(bitboard_Board *);
void bitboard_Board_playMove(bitboard_Board *, uint64_t src, uint64_t dst);

uint64_t bitboard_playMove(uint64_t original, uint64_t src, uint64_t dst);

// updates the allPieces field if the other fields were changed manually
void bitboard_Side_update(bitboard_Side *);
void bitboard_Side_playMove(bitboard_Side *, uint64_t src, uint64_t dst);

// returns the rank/file of the piece referred to by the most significant bit
// of the bitboard
uint8_t bitboard_getRank(uint64_t);
uint8_t bitboard_getFile(uint64_t);

// returns the bit number of the most significant bit
uint8_t bitboard_getMostSignificant(uint64_t);

// returns the bitboard containing only the most significant bit of the
// original, or 0 if no bits are set
uint64_t bitboard_getMostSignificantMask(uint64_t);

// returns the number of bits set in the bitboard
size_t bitboard_countPieces(uint64_t);

#endif

