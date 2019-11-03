# CMakeList.txt : CMake project for trainingdata-tool, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_REQUIRED_FLAGS -std=c++17)

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

set_target_properties(trainingdata-tool PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

if (UNIX)
    target_link_libraries(trainingdata-tool -lpthread -lstdc++fs)
endif(UNIX)

find_package(Boost 1.65.0)

include_directories(
    "lc0/src"
    "lc0/src/chess"
    "lc0/src/neural"
    "polyglot/src"
    "zlib"
    ${Boost_INCLUDE_DIRS}
)

add_compile_definitions(NO_PEXT)

# TODO: Add tests and install targets if needed.
