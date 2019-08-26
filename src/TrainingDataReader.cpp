#include <filesystem>

#include "TrainingDataReader.h"

TrainingDataReader::TrainingDataReader(const std::string& in_directory) : in_files(), gzFile(nullptr) {
  auto dir_it = std::filesystem::directory_iterator(path);
  in_files.insert(in_files.end(), dir_it.begin(), dir_it.end());
  in_files_it = in_files.begin();
}

lczero::V4TrainingData TrainingDataReader::ReadChunk() {
  if (nullptr == gzFile) {
    gzFile = gzopen(file_name, "r");
  }
}
