#ifndef TRAININGDATA_TOOL_TRAININGDATAWRITER_H
#define TRAININGDATA_TOOL_TRAININGDATAWRITER_H

#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "neural/encoder.h"
#include "neural/network.h"
#include "neural/writer.h"

class TrainingDataWriter {
public:
    TrainingDataWriter(size_t max_files_per_directory, size_t chunks_per_file);

    void EnqueueChunks(const std::vector<lczero::V4TrainingData> &chunks);

    void Finalize();

private:
    void WriteQueuedChunks(size_t min_chunks);

    std::queue<lczero::V4TrainingData> chunks_queue;
    size_t files_written;
    size_t max_files_per_directory;
    size_t chunks_per_file;
};


#endif
