#include "TrainingDataDedup.h"

#include "V4TrainingDataHashUtil.h"

#include <iostream>
#include <unordered_map>

float merge_val(float old_val, size_t old_count, float new_val) {
  return (old_val * old_count + new_val) / static_cast<float>(old_count + 1);
}

void merge_chunks(lczero::V4TrainingData& chunk, size_t old_count,
                  const lczero::V4TrainingData& new_chunk) {
  for (size_t i = 0; i < ARR_LENGTH(chunk.probabilities); ++i) {
    chunk.probabilities[i] = merge_val(chunk.probabilities[i], old_count,
                                       new_chunk.probabilities[i]);
  }

  chunk.best_q = merge_val(chunk.best_q, old_count, new_chunk.best_q);
  chunk.root_q = merge_val(chunk.root_q, old_count, new_chunk.root_q);
  chunk.best_d = merge_val(chunk.best_d, old_count, new_chunk.best_d);
  chunk.root_d = merge_val(chunk.root_d, old_count, new_chunk.root_d);
}

void flush(TrainingDataWriter& writer,
           std::unordered_map<lczero::V4TrainingData, size_t>& chunk_map,
           size_t& unique_count, size_t& total_count) {
  std::cout << "Start writing chunks..." << std::endl;
  writer.EnqueueChunks(chunk_map);
  chunk_map.clear();
  writer.Finalize();
  std::cout << "Total positions: " << total_count
            << ", unique positions: " << unique_count << ", repeated: "
            << (1.0f - static_cast<float>(unique_count) /
                           static_cast<float>(total_count)) *
                   100.0f
            << "%" << std::endl;
  unique_count = 0;
  total_count = 0;
}

void training_data_dedup(TrainingDataReader& reader, TrainingDataWriter& writer,
                         const size_t dedup_uniq_buffersize,
                         const float q_ratio) {
  size_t unique_count = 0;
  size_t total_count = 0;
  std::unordered_map<lczero::V4TrainingData, size_t> chunk_map;

  while (auto new_chunk = reader.ReadChunk()) {
    total_count++;

    // Average Z and Q depending on q_ratio
    auto Z = static_cast<float>(new_chunk->result);
    new_chunk->best_q = new_chunk->best_q * q_ratio + Z * (1.0f - q_ratio);
    new_chunk->root_q = new_chunk->root_q * q_ratio + Z * (1.0f - q_ratio);

    auto elem = chunk_map.find(*new_chunk);
    if (elem == chunk_map.end()) {
      chunk_map.emplace(*new_chunk, 1);
      unique_count++;
    } else {
      lczero::V4TrainingData merged = elem->first;
      size_t old_count = elem->second;
      merge_chunks(merged, elem->second, *new_chunk);
      chunk_map.erase(elem);
      chunk_map.emplace(merged, old_count + 1);
    }
    if (unique_count >= dedup_uniq_buffersize) {
      flush(writer, chunk_map, unique_count, total_count);
    }
  }
  flush(writer, chunk_map, unique_count, total_count);
}
