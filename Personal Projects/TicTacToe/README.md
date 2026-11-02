# Tic-Tac-Toe

> Historical educational example. See the [repository overview](../../README.md) for context and current documentation status.

A C++17 console game originally written by David Tom in November 2023.
The corrected implementation keeps the existing program structure and adds
repeatable tests for input, board rules, game results, and computer moves.

## Build and run

From this directory, use a C++17 compiler and Make:

```sh
make
./build/tictactoe
```

Or compile the single implementation file directly:

```sh
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -Wshadow Tic-Tac-Toe.cpp -o tictactoe
./tictactoe
```

The tracked `Tic-Tac-Toe.exe` is a legacy binary. It has **not** been rebuilt or
verified for these fixes; compile the current source to use this version.
The game itself has no third-party dependencies.

## Playing

Enter the row count, column count, and number of human players when prompted.
Each dimension can be **2 through 100**, independently. Enter one name token
for each human player; names containing spaces are not supported.

| Human players | X, who starts | O |
| --- | --- | --- |
| 2 | Human | Human |
| 1 | Human | Minimax computer |
| 0 | Minimax computer | Random computer |

Names do not determine whether a player is human or computer. A human named
`Computer` remains human.

Cells are numbered from 1, left to right and then top to bottom. Enter a cell
number to place the current player's mark. Integer inputs must be complete
whitespace-separated tokens: `3` is valid, while `3abc`, `1.5`, and overflowing
integers are rejected. An invalid or occupied cell does not change the board,
advance the turn, or increase the move count. End-of-input exits cleanly;
an unfinished game is not recorded as a win or draw.

### Board rules

- Completing an entire row or an entire column wins.
- On a square board, either full corner-to-corner diagonal also wins.
- Rectangular boards have no diagonal wins.
- A full board without a winner is a draw. A win on the final cell is counted
  only as a win.

These preserve the existing adjustable-board rules. They are not a configurable
“connect three” or “connect k” rule: a row on a 2×3 board requires three marks,
while a column requires two.

## Computer players and limits

The random player selects an empty cell using a generator seeded once.
Minimax evaluates moves for the correct player, alternates turns, stops at
terminal states, and passes alpha/beta bounds through recursive calls. Trial
moves are undone before the search returns, including when interrupted.

Search uses iterative deepening:

- Boards with at most nine cells search through the remaining moves when the
  time budget permits. Standard 3×3 move quality has been checked exhaustively
  against an independent oracle for every reachable ongoing position.
- Boards with more than nine cells search at most **six plies** and
  **100,000 nodes total per turn**. A ply is one player's move. Nonterminal
  positions at the depth limit receive a neutral score.
- The default time budget is **five seconds per Minimax move**. If a deeper
  iteration is interrupted, the last fully completed iteration supplies the
  move. If none completed, the computer selects a legal random move.

The deadline is checked during search, so it is a work budget rather than a
strict wall-clock guarantee for the whole turn. Larger-board strategic strength
and full-game performance have not been established. Their bounded searches
are not guaranteed to find an optimal move. Even on 3×3, a deliberately shortened
budget can trigger the fallback and removes the full-search guarantee.

Both move-selection functions return `-1` when the board is invalid, won, or
full. Board-shape guards prevent dimension/storage mismatches; they do not
validate the reachability of arbitrary imported or saved positions.

## Configuration

| Setting | Default | How to change it |
| --- | --- | --- |
| Rows and columns | Prompted, each 2–100 | Console input |
| Human players | Prompted, 0–2 | Console input |
| `MAX_GAME_COUNT` | 1 | Edit the source and recompile |
| `MAX_BOARD_SIZE` | 100 | Edit the source and recompile |
| `TIMER_CUTOFF` | 5 seconds | Edit the source and recompile |
| Algorithms assigned to players | Mode table above | Edit the dispatch logic |

There is no runtime algorithm picker, game-count menu, or replay prompt.
Increasing `MAX_GAME_COUNT` starts the configured number of games with the same
players and dimensions. Final move count describes the last game; wins, draws,
and completed-game count accumulate. Reported game duration includes time spent
waiting for human moves and is not an isolated AI benchmark.

## Automated tests

Tests require a C++17 compiler, Make, and Python 3. Python uses only its standard
library; there is no added C++ testing framework.

```sh
make test
make sanitize
```

`make test` runs nine C++ test groups and 18 console integration tests. Coverage
includes invalid selections, occupied cells, extreme integer values, malformed
boards, wins, draws, both computer players, interrupted searches, rectangular
boards, input recovery, and EOF. The C++ suite checks **5,478 reachable 3×3
positions**: 4,520 ongoing positions and 958 terminal positions (626 X wins,
316 O wins, and 16 draws). An independent flat-board oracle checks whether each
selected move preserves the best attainable win/draw/loss outcome.

The normal suite passed on Linux with GCC 13.3 using C++17: **43,323 C++ checks**
and **18 console tests**. `make sanitize` reruns the suites with AddressSanitizer
and UndefinedBehaviorSanitizer and requests leak detection. This target requires
a GCC/Clang toolchain and a host that support those runtimes. See
[TESTING.md](TESTING.md) for recorded results and remaining platform checks.

If the host cannot run LeakSanitizer (for example, a sandbox that blocks process
inspection), use `make sanitize ASAN_OPTIONS=detect_leaks=0` to run address and
undefined-behavior checks. This does not perform leak checking; the recorded
validation used this override after the default leak checker failed to start.

The old 3×3, 10×10, and 30×30 timing claims were produced by incorrect search
logic and have been removed. They are not measurements of this version.
Windows/macOS builds, the legacy executable, exhaustive larger-board play, and
large-board performance still require separate validation.

## Future browser reuse

The board, legal-move checks, win/draw rules, and AI can form a small game-logic
module behind a browser interface. The existing tests can serve as acceptance
cases for a later JavaScript port or WebAssembly wrapper. This repair does not
add either migration, a graphical interface, or an application framework.

## Contributing

See the repository-local [contribution guide](../../CONTRIBUTING.md) for reporting issues and submitting focused pull requests while preserving the educational context.

## Authors

* **David Tom** - *Initial Work* - [davidtomofficial](https://github.com/davidtomofficial)

See this repository's [commit history](https://github.com/davidtomofficial/cpp-Examples/commits/main/) for recorded changes. Additional authors and course-provided material remain credited in their respective files.

## License

See the [repository licensing notes](../../README.md#license) for the retained GNU GPL version 3 statement, existing file-specific notices, and the scope clarification still needed before adding local license documentation.

## Acknowledgements

* A big thanks to any and all contributors, no matter how big or small the contribution it all helps.

