# trainingdata-tool
Tool to generate [lc0](https://github.com/LeelaChessZero/lc0) training data. Useful for [Supervised Learning](https://github.com/dkappe/leela-chess-weights/wiki/Supervised-Learning) from PGN games.

## Usage
Pass the PGN input file and it will output training data in the same way lc0 selfplay does. Example:
```
trainingdata-tool 2008_SCT_LadiesOpen.pgn
```

There are 4 options suported so far:
 - `-v`: Verbose mode
 - `-lichess-mode`: Lichess mode. Will extract SF evaluation score from Lichess commented games. Non-commented games will be filtered out.
 - `-games-per-dir <integer number>`: Max games to store in a single directory, when that number is reached a new directory is created to store the new games to avoid stressing the file system too much.
 - `-max-games-to-convert <integer number>`: Stop after this many ga

 Example:
 ```
 trainingdata-tool -max-games-to-convert 1000 -games-per-dir 500 -v -lichess-mode
Max games to convert set to: 1000
Max games per directory set to: 500
Verbose mode ON
Lichess mode ON
 ```
