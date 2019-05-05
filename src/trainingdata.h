#if !defined(TRAININGDATA_H_INCLUDED)
#define TRAININGDATA_H_INCLUDED

#include "neural/encoder.h"
#include "neural/network.h"
#include "neural/writer.h"

lczero::V4TrainingData get_v4_training_data(
        lczero::GameResult game_result, const lczero::PositionHistory& history,
        lczero::Move played_move, lczero::MoveList legal_moves, float Q);

#endif
