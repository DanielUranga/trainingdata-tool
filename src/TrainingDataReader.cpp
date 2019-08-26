#include <filesystem>
#include <iostream>

#include "TrainingDataReader.h"

TrainingDataReader::TrainingDataReader(const std::string& in_directory)
    : in_files(), file(nullptr) {
  for (auto& p : std::filesystem::directory_iterator(in_directory)) {
    in_files.push_back(p.path());
    std::cout << p.path() << std::endl;
  }
  in_files_it = in_files.begin();
}

std::optional<lczero::V4TrainingData> TrainingDataReader::ReadChunk() {
  while (nullptr == file) {
    if (in_files_it == in_files.end()) {
      return std::nullopt;
    }
    file = gzopen(in_files_it->c_str(), "r");
    in_files_it++;
  }
  const size_t length = sizeof(lczero::V4TrainingData);
  lczero::V4TrainingData buffer{};
  int bytes_read = gzread(file, &buffer, length);
  if (length == bytes_read) {
    return std::optional<lczero::V4TrainingData>{buffer};
  } else {
    std::cout << "Not enough bytes " << bytes_read << std::endl;
    return std::nullopt;
  }
}
