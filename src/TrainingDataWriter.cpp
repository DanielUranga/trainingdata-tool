#include "TrainingDataWriter.h"

TrainingDataWriter::TrainingDataWriter(size_t max_files_per_directory, size_t chunks_per_file) :
  files_written(0),
  max_files_per_directory(max_files_per_directory),
  chunks_per_file(chunks_per_file)
{
};

void TrainingDataWriter::EnqueueChunks(const std::vector<lczero::V4TrainingData> &chunks) {
  for (auto &chunk : chunks) {
    chunks_queue.push(chunk);
  }
  WriteQueuedChunks(chunks_per_file);
}

void TrainingDataWriter::WriteQueuedChunks(size_t min_chunks) {
  while (chunks_queue.size() > min_chunks) {
    lczero::TrainingDataWriter writer(
            files_written,
            "supervised-" + std::to_string(files_written / max_files_per_directory)
    );
    for (size_t i = 0; i < chunks_per_file && !chunks_queue.empty(); ++i) {
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
