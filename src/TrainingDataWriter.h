#ifndef TRAININGDATA_TOOL_TRAININGDATAWRITER_H
#define TRAININGDATA_TOOL_TRAININGDATAWRITER_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "neural/encoder.h"
#include "neural/network.h"
#include "neural/writer.h"

#include "V4TrainingDataHashUtil.h"

class TrainingDataWriter {
 public:
  TrainingDataWriter(size_t max_files_per_directory, size_t chunks_per_file,
                     std::string dir_prefix = "supervised-");

  void EnqueueChunks(const std::vector<lczero::V4TrainingData>& chunks);
  void EnqueueChunks(
      const std::unordered_map<lczero::V4TrainingData, size_t>& chunks);

  void Finalize();

 private:
  void WriteQueuedChunks(size_t min_chunks);

  std::queue<lczero::V4TrainingData> chunks_queue;
  size_t files_written;
  size_t max_files_per_directory;
  size_t chunks_per_file;
  const std::string dir_prefix;
};

#endif
