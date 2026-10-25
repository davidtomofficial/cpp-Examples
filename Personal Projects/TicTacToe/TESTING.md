# Tic-Tac-Toe repair and validation

Validation date: October 24, 2026 (Pacific/Honolulu).

Source inspected: `davidtomofficial/cpp-Examples`, `main` at
[`3b81a431358c9566b5040d75eb6d2bf2156606c9`](https://github.com/davidtomofficial/cpp-Examples/commit/3b81a431358c9566b5040d75eb6d2bf2156606c9).
Only `Personal Projects/TicTacToe` is changed by this repair.

During delivery, the documentation update
[`621579720207f8445584dc2cf5471adc41530418`](https://github.com/davidtomofficial/cpp-Examples/commit/621579720207f8445584dc2cf5471adc41530418)
landed on `main`. It was integrated into this branch, retaining its author,
contribution, historical-context and licensing references. The only production
source change from that update is an author URL in a comment; tested executable
logic is unchanged by the integration.

## Environment and baseline

- Linux 6.18.55, x86_64.
- GCC/G++ 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04), C++17.
- GNU Make 4.3 and Python 3.12.14.
- No third-party game or test framework.

The unmodified source did not compile with the compiler and warning flags
below: `INT_MIN` was undeclared because its required header was missing.
The compiler also reported the shadowed/unused random index and signed/unsigned
loop comparisons. Runtime results in this report describe the corrected source,
not the original program or the tracked legacy Windows executable.

## Corrections

| Area | Corrected behavior |
| --- | --- |
| Random move selection | A once-seeded generator indexes only a nonempty legal-move list; terminal or invalid boards return `-1`. |
| Cell validation | Range and board-shape checks return before arithmetic or indexing; occupied cells and invalid marks cannot alter the board. |
| Game loop | Only accepted moves increment the move count and advance the turn; a winning last move is not also a draw. |
| Input | Complete integer tokens are required; bad suffixes, fractions and overflow are rejected; EOF exits without counting an unfinished game. |
| Full-board detection | Examines board cells rather than relying on the counter that speculative AI moves do not update. |
| Minimax | Correct terminal returns, alternating X/O moves, the actual root player, and propagated alpha/beta bounds. |
| Search interruption | Timeout/node exhaustion is separate from scores; every trial move is undone; the last completed depth or a legal random fallback supplies a move. |
| Search bounds | At most nine-cell boards can search to the end; larger boards have a six-ply and 100,000-node limit in addition to the time budget. |
| Mode selection | Human count chooses control; a human named `Computer` stays human. |
| Documentation | Exact rules and controls replace inaccurate algorithm assignments, bounds and unverified speed claims. |

## Commands and observed results

Run from `Personal Projects/TicTacToe`:

```sh
make all CXXFLAGS='-O2 -Werror'
make test CXXFLAGS='-O2 -Werror'
make sanitize ASAN_OPTIONS=detect_leaks=0
```

The Makefile adds `-std=c++17 -Wall -Wextra -Wpedantic -Wshadow`.
Both the implementation and C++ tests compiled without warnings with warnings
treated as errors in the normal build. Sanitizer builds use `-O1 -g`,
`-fsanitize=address,undefined`, `-fno-sanitize-recover=all`, and
`-fno-omit-frame-pointer`; UndefinedBehaviorSanitizer stops on the first error.

| Verification | Result |
| --- | --- |
| C++ unit/regression and exhaustive-oracle suite | PASS: 9 groups, 43,323 checks |
| Real executable console integration suite | PASS: 18 tests, including additional parameterized cases |
| Both suites under AddressSanitizer and UndefinedBehaviorSanitizer | PASS: same 43,323 checks and 18 tests, no sanitizer diagnostics |
| Source-configured three-game AI batch | PASS: 3 completed games, 3 X wins, 0 O wins, 0 draws; last game had 5 accepted moves in this run |
| LeakSanitizer | NOT VERIFIED: host process-inspection restriction; see below |
| Windows, macOS and non-GCC toolchains | NOT RUN |

The three-game check compiled a temporary copy with `MAX_GAME_COUNT = 3`,
ran `3`, `3`, `0` as setup input, and asserted that the result totals equaled
three and Minimax X had no losses. The checked-in default remains one game.
Random-opponent results can differ between runs; the invariant is that completed
games equal the sum of wins and draws and that full-search Minimax does not lose
from the empty standard board.

### Recorded normal-suite output

```text
PASS: invalid/occupied moves and 1-based cell mapping
PASS: malformed dimensions and board storage
PASS: square/rectangular wins and invalid marks
PASS: board-derived fullness and terminal move rejection
PASS: reset preserves statistics and resizes storage
PASS: random AI only selects available cells
PASS: X/O wins, blocks, draw and fork defense
PASS: timeout fallback and bounded larger-board search
PASS: all 5,478 reachable 3x3 states against an independent oracle
All 9 suites passed (43323 checks).
Exhaustive coverage: 4,520 optimal AI choices; 626 X wins; 316 O wins; 16 draws.
Ran 18 tests in 0.077s
OK
```

The sanitizer run repeated all nine C++ groups successfully and the Python
suite reported `Ran 18 tests in 0.209s` and `OK`. These are observations from one
validation run, not performance promises; they exclude compilation and the
Python timings exclude the C++ suite.

### Leak-detection limitation

The initial `make sanitize` run requested leak detection and exited with:

```text
Can't open /proc/18/task for reading.
LeakSanitizer has encountered a fatal error.
HINT: LeakSanitizer does not work under ptrace (strace, gdb, etc)
```

The sandbox did not permit the process inspection required by LeakSanitizer.
Rerunning with `ASAN_OPTIONS=detect_leaks=0` kept address and undefined-behavior
instrumentation enabled and passed. No claim of verified leak freedom is made.
On a supported development host, run the default `make sanitize` to complete
leak checking.

## What the tests establish

### Independent exhaustive standard-board verification

`tests/game-tests.cpp` enumerates positions from an empty 3×3 board, alternates
X and O, and stops expanding a position once it is won or full. Its reference
solver uses a flat string, fixed winning bit masks, and memoized negamax.
It does not call the production winner checks, move helpers, evaluation,
Minimax, or pruning to calculate reference outcomes.

| Reachable-position category | Positions checked |
| --- | ---: |
| Ongoing, X to move | 2,423 |
| Ongoing, O to move | 2,097 |
| Terminal X wins | 626 |
| Terminal O wins | 316 |
| Terminal draws | 16 |
| Total | 5,478 |

Every ongoing position checks that the chosen cell is legal and preserves the
best achievable win/draw/loss result. All terminal positions reject further AI
moves. Both selection functions preserve the board, configuration and game
statistics; the search's own start timestamp may change.

The exhaustive calls give each search a 60-second budget to avoid conflating
correctness with a deliberately shortened deadline. Actual console mode tests
use the default five-second budget. This verifies outcome-optimal choices,
not a preference for the fastest available win or a guarantee under arbitrary
CPU starvation or shortened budgets.

### Representative regressions and console games

The C++ suite covers `INT_MIN`, `INT_MAX`, zero, negative/out-of-range cells,
occupied cells, invalid marks, malformed board dimensions/storage, stale move
counters, all eight square winning lines, both rectangular orientations,
full-board wins, genuine draws, reset behavior, repeated random selections,
immediate X/O wins and blocks, and a fork defense.

Zero and negative time budgets deterministically exercise legal fallback.
Positive-budget searches on 2×4, 4×2, 4×4 and 100×100 boards check legal selection,
unchanged state and termination with a generous allowance. They do not certify
whole-game performance or perfect play on those boards.

The 18 console tests exercise the actual executable: both human winners, a
draw, a ninth-cell win, rejected selections retaining the turn/count, strict
token and setup validation, a human named `Computer`, EOF at setup and during
play, 2×2/2×3/3×2 games, three human-versus-AI opening choices, and AI versus AI.

## Documented features: disposition

| Feature | Verified behavior or remaining distinction |
| --- | --- |
| Human versus human | Verified with X wins, O wins, draw and invalid-input retries. |
| Human versus computer | Human X versus Minimax O; console completion and independent 3×3 AI correctness verified. |
| Computer versus computer | Minimax X versus random O; completion, legal counts and cumulative batch statistics verified. |
| Adjustable board size | Each dimension 2–100; representative sizes and maximum-sized state tested. Every possible size/game was not tested exhaustively. |
| Rectangular wins | Full row or column; diagonals only on square boards. Preserves the original rule, not a connect-k variant. |
| Adjustable algorithms | Algorithms exist and are assigned by mode. No runtime algorithm-selection menu. |
| Custom game counts | Source constant plus rebuild; the three-game configuration was verified. No replay/game-count menu. |
| Old speed estimates | Removed; incorrect original search made them unsuitable as evidence. |
| Existing `.exe` | Legacy artifact, neither regenerated nor validated. Build the corrected source. |

## Remaining testing before broader release

1. Build and run on the intended Windows/MSVC and/or macOS/Clang platforms;
   rebuild any executable intended for distribution from the corrected source.
2. Run leak detection on a host that permits it.
3. If larger boards become a product feature, test full sessions, strategic
   quality, responsiveness and supported dimensions on target hardware. The
   cooperative search budget is not a hard whole-turn deadline.
4. If loading saved games or accepting browser/network state, validate mark
   contents, counts, whose turn it is, and legal history at that new boundary.
   The current console creates reachable states; shape checks alone are not a
   general saved-game validator.

The existing rule and AI helpers can later be separated from console I/O and
driven by a browser UI. The exhaustive oracle and recorded input/result cases
provide acceptance tests for that adapter or port. No browser migration is
included in this repair.
