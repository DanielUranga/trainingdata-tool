#include <algorithm>
#include <filesystem>
#include <iostream>

#include "TrainingDataReader.h"

TrainingDataReader::TrainingDataReader(const std::string& in_directory)
    : in_files(), file(nullptr) {
  for (auto& p : std::filesystem::directory_iterator(in_directory)) {
    in_files.push_back(p.path().string());
  }
  std::sort(in_files.begin(), in_files.end());
  in_files_it = in_files.begin();
}

TrainingDataReader::~TrainingDataReader() {
  if (nullptr != file) {
    gzclose(file);
  }
}

std::optional<lczero::V4TrainingData> TrainingDataReader::ReadChunk() {
  const size_t length = sizeof(lczero::V4TrainingData);
  lczero::V4TrainingData buffer{};
  int bytes_read;
  do {
    gzFile currentFile = getCurrentFile();
    if (nullptr == currentFile) {
      return std::nullopt;
    }
    bytes_read = gzread(currentFile, &buffer, length);
  } while (length != bytes_read);
  return std::optional<lczero::V4TrainingData>{buffer};
}
gzFile TrainingDataReader::getCurrentFile() {
  if (nullptr != file && gzeof(file)) {
    gzclose(file);
    file = nullptr;
  }
  if (nullptr == file) {
    if (in_files_it == in_files.end()) {
      return nullptr;
    }
    file = gzopen(in_files_it->c_str(), "r");
    in_files_it++;
  }
  return file;
}
