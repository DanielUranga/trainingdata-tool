#ifndef TRAININGDATA_TOOL_TRAININGDATADEDUP_H
#define TRAININGDATA_TOOL_TRAININGDATADEDUP_H

#include "TrainingDataReader.h"
#include "TrainingDataWriter.h"

void training_data_dedup(TrainingDataReader& reader, TrainingDataWriter& writer,
                         const size_t dedup_uniq_buffersize,
                         const float q_ratio);

#endif
