#ifndef TRAININGDATA_TOOL_TRAININGDATAWRITER_H
#define TRAININGDATA_TOOL_TRAININGDATAWRITER_H

#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "neural/encoder.h"
#include "neural/network.h"
#include "neural/writer.h"

#define CHUNKS_PER_FILE 1024

class TrainingDataWriter {
public:
    TrainingDataWriter(size_t max_files_per_directory);

    void EnqueueChunks(const std::vector<lczero::V4TrainingData> &chunks);

    void Finalize();

private:
    void WriteQueuedChunks(size_t min_chunks = CHUNKS_PER_FILE);

    std::queue<lczero::V4TrainingData> chunks_queue;
    size_t files_written;
    size_t  max_files_per_directory;
};


#endif
