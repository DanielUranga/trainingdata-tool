#if !defined(PGN_MOVE_INFO_H_INCLUDED)
#define PGN_MOVE_INFO_H_INCLUDED

#include "pgn.h"

struct PGNMoveInfo {
  char move[PGN_STRING_SIZE];
  char comment[PGN_STRING_SIZE];
  char nag[PGN_STRING_SIZE];
  explicit PGNMoveInfo(char* move, char* comment, char* nag);
};

#endif
