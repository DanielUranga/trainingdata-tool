#include "TrainingDataDedup.h"

#include <boost/functional/hash.hpp>
#include <iostream>
#include <unordered_map>

#define ARR_LENGTH(a) (sizeof(a) / sizeof(a[0]))

namespace std {
template <>
struct hash<lczero::V4TrainingData> {
  size_t operator()(const lczero::V4TrainingData& k) const {
    size_t hash = boost::hash_range(k.planes, k.planes + ARR_LENGTH(k.planes));
    boost::hash_combine(hash, k.castling_us_ooo);
    boost::hash_combine(hash, k.castling_us_oo);
    boost::hash_combine(hash, k.castling_them_ooo);
    boost::hash_combine(hash, k.castling_them_oo);
    boost::hash_combine(hash, k.side_to_move);
    boost::hash_combine(hash, k.rule50_count);
    return hash;
  }
};

template <>
struct equal_to<lczero::V4TrainingData> {
  bool operator()(const lczero::V4TrainingData& lhs,
                  const lczero::V4TrainingData& rhs) const {
    return std::equal(lhs.planes, lhs.planes + ARR_LENGTH(lhs.planes),
                      rhs.planes) &&
           lhs.castling_us_ooo == rhs.castling_us_ooo &&
           lhs.castling_us_oo == rhs.castling_us_oo &&
           lhs.castling_them_ooo == rhs.castling_them_ooo &&
           lhs.castling_them_oo == rhs.castling_them_oo &&
           lhs.side_to_move == rhs.side_to_move &&
           lhs.rule50_count == rhs.rule50_count;
  }
};
}  // namespace std

float merge_val(float old_val, size_t old_count, float new_val) {
  return (old_val * old_count + new_val) / static_cast<float>(old_count + 1);
}

void merge_chunks(lczero::V4TrainingData& chunk, size_t old_count,
                  const lczero::V4TrainingData& new_chunk) {
  std::cout << "Merge " << old_count << " - " << chunk.best_q << " - " << new_chunk.best_q << std::endl;
  for (size_t i = 0; i < ARR_LENGTH(chunk.probabilities); ++i) {
    chunk.probabilities[i] = merge_val(chunk.probabilities[i], old_count,
                                       new_chunk.probabilities[i]);
  }
  chunk.best_q = merge_val(chunk.best_q, old_count, new_chunk.best_q);
  chunk.root_q = merge_val(chunk.root_q, old_count, new_chunk.root_q);
  chunk.best_d = merge_val(chunk.best_d, old_count, new_chunk.best_d);
  chunk.root_d = merge_val(chunk.root_d, old_count, new_chunk.root_d);
}

void training_data_dedup(TrainingDataReader& reader,
                         TrainingDataWriter& writer) {
  size_t read_count = 0;
  std::unordered_map<lczero::V4TrainingData, size_t> chunk_map;
  for (size_t i = 0; i < 1000; ++i) {
    if (auto new_chunk = reader.ReadChunk()) {
      auto elem = chunk_map.find(*new_chunk);
      if (elem == chunk_map.end()) {
        chunk_map.emplace(*new_chunk, 1);
      } else {
        lczero::V4TrainingData merged = elem->first;
        size_t old_count = elem->second;
        merge_chunks(merged, elem->second, *new_chunk);
        chunk_map.erase(elem);
        chunk_map.emplace(merged, old_count + 1);
      }
    }
  }
  for (auto& itr : chunk_map) {
    if (itr.second > 1) {
      std::cout << std::hash<lczero::V4TrainingData>()(itr.first) << " : "
                << itr.second << std::endl;
    }
  }
}
