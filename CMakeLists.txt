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
    "lc0/src/utils/logging.cc"
    "lc0/src/utils/random.cc"
)

if (WIN32)
    set (lc0_filesystem "lc0/src/utils/filesystem.win32.cc")
else (WIN32)
    set (lc0_filesystem "lc0/src/utils/filesystem.posix.cc")
endif (WIN32)

AUX_SOURCE_DIRECTORY(polyglot/src polyglot)
AUX_SOURCE_DIRECTORY(zlib zlib)

# Add source to this project's executable.
add_executable(trainingdata-tool ${sources} ${lc0} ${lc0_filesystem} ${polyglot} ${zlib})

include_directories(
    "lc0/src"
    "lc0/src/chess"
    "lc0/src/neural"
    "polyglot/src"
    "zlib"
)

add_compile_definitions(NO_PEXT)

# TODO: Add tests and install targets if needed.
