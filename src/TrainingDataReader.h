#ifndef TRAININGDATA_TOOL_TRAININGDATAREADER_H
#define TRAININGDATA_TOOL_TRAININGDATAREADER_H

#include <vector>;
#include <zlib.h>

#include "neural/writer.h"

class TrainingDataReader {
public:
  TrainingDataReader(const std::string &in_directory);
  lczero::V4TrainingData ReadChunk();

private:
  std::vector<std::string> in_files;
  std::vector<std::string>::iterator in_files_it;
  gzFile* file;
};

#endif