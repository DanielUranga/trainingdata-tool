# CMakeList.txt : CMake project for trainingdata-tool, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE sources src/*.cpp src/*.h)

set (
    lc0
    "lc0/src/chess/bitboard.cc"
    "lc0/src/chess/board.cc"
    "lc0/src/chess/position.cc"
    "lc0/src/neural/encoder.cc"
    "lc0/src/neural/writer.cc"
    "lc0/src/utils/commandline.cc"
    "lc0/src/utils/filesystem.win32.cc"
    "lc0/src/utils/random.cc"
)

AUX_SOURCE_DIRECTORY(zlib zlib) 

# Add source to this project's executable.
add_executable(main ${sources} ${lc0} ${zlib})

include_directories(
    "lc0/src"
    "lc0/src/chess"
    "lc0/src/neural"
    "zlib"
)

# TODO: Add tests and install targets if needed.
