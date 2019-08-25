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

lczero::V4TrainingData TrainingDataReader::ReadChunk() {
  if (nullptr == file) {
    file = gzopen(in_files_it->c_str(), "r");
    in_files_it++;
  }
}
