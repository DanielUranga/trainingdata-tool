#include "PGNMoveInfo.h"

#include <cstring>

PGNMoveInfo::PGNMoveInfo(char* move, char* comment, char* nag) {
  strncpy(this->move, move, PGN_STRING_SIZE);
  strncpy(this->comment, comment, PGN_STRING_SIZE);
  strncpy(this->nag, nag, PGN_STRING_SIZE);
}
