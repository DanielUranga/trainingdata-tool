#include "chess/position.h"
#include "move.h"
#include "move_do.h"
#include "move_gen.h"
#include "move_legal.h"
#include "neural/encoder.h"
#include "neural/network.h"
#include "neural/writer.h"
#include "pgn.h"
#include "polyglot_lib.h"
#include "san.h"
#include "square.h"
#include "util.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>

#include "PGNGame.h"
#include "TrainingDataWriter.h"
#include "TrainingDataReader.h"

size_t max_files_per_directory = 10000;
int64_t max_games_to_convert = 10000000;
size_t chunks_per_file = 4096;

inline bool file_exists(const std::string &name) {
  std::ifstream f(name.c_str());
  return f.good();
}

void convert_games(const std::string& pgn_file_name, Options options) {
  int game_id = 0;
  pgn_t pgn[1];
  pgn_open(pgn, pgn_file_name.c_str());
  TrainingDataWriter writer(max_files_per_directory, chunks_per_file);
  while (pgn_next_game(pgn) && game_id < max_games_to_convert) {
    PGNGame game(pgn);
    writer.EnqueueChunks(game.getChunks(options));
    game_id++;
    if (game_id % 1000 == 0) {
      std::cout << game_id << " games written." << std::endl;
    }
  }
  writer.Finalize();
  std::cout << "Finished writing " << game_id << " games." << std::endl;
  pgn_close(pgn);
}

int main(int argc, char *argv[]) {
  lczero::InitializeMagicBitboards();
  polyglot_init();
  Options options;
  for (size_t idx = 0; idx < argc; ++idx) {
    if (0 == static_cast<std::string>("-v").compare(argv[idx])) {
      std::cout << "Verbose mode ON" << std::endl;
      options.verbose = true;
    } else if (0 ==
               static_cast<std::string>("-lichess-mode").compare(argv[idx])) {
      std::cout << "Lichess mode ON" << std::endl;
      options.lichess_mode = true;
    } else if (0 ==
               static_cast<std::string>("-files-per-dir").compare(argv[idx])) {
      max_files_per_directory = std::atoi(argv[idx + 1]);
      std::cout << "Max files per directory set to: " << max_files_per_directory
                << std::endl;
    } else if (0 == static_cast<std::string>("-max-games-to-convert")
            .compare(argv[idx])) {
      max_games_to_convert = std::atoi(argv[idx + 1]);
      std::cout << "Max games to convert set to: " << max_games_to_convert
                << std::endl;
    } else if (0 == static_cast<std::string>("-chunks-per-file")
            .compare(argv[idx])) {
      chunks_per_file = std::atoi(argv[idx + 1]);
      std::cout << "Chunks per file set to: " << chunks_per_file << std::endl;
    }
  }

  for (size_t idx = 1; idx < argc; ++idx) {
    if (!file_exists(argv[idx])) continue;
    if (options.verbose) {
      std::cout << "Opening \'" << argv[idx] << "\'" << std::endl;
    }

    convert_games(argv[idx], options);
  }
}
