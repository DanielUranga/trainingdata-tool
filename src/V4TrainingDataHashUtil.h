#ifndef TRAININGDATA_TOOL_V4TRAININGDATAHASHUTIL_H
#define TRAININGDATA_TOOL_V4TRAININGDATAHASHUTIL_H

#include <boost/functional/hash.hpp>

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

#endif  // TRAININGDATA_TOOL_V4TRAININGDATAHASHUTIL_H
