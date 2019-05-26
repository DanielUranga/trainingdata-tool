#include "TrainingDataWriter.h"

TrainingDataWriter::TrainingDataWriter(size_t max_files_per_directory) : files_written(0), max_files_per_directory(
        max_files_per_directory) {
};

void TrainingDataWriter::EnqueueChunks(const std::vector<lczero::V4TrainingData> &chunks) {
  for (auto &chunk : chunks) {
    chunks_queue.push(chunk);
  }
  WriteQueuedChunks();
}

void TrainingDataWriter::WriteQueuedChunks(size_t min_chunks) {
  while (chunks_queue.size() > min_chunks) {
    lczero::TrainingDataWriter writer(
            files_written,
            "supervised-" + std::to_string(files_written / max_files_per_directory)
    );
    for (size_t i = 0; i < CHUNKS_PER_FILE && !chunks_queue.empty(); ++i) {
      writer.WriteChunk(chunks_queue.front());
      chunks_queue.pop();
    }
    writer.Finalize();
    files_written++;
  }
}

void TrainingDataWriter::Finalize() {
  WriteQueuedChunks(0);
}
