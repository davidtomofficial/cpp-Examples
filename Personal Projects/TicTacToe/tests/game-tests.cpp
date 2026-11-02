#define TICTACTOE_TEST
#include "../Tic-Tac-Toe.cpp"

#include <array>
#include <chrono>
#include <climits>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

std::size_t checks = 0;
std::size_t suites = 0;

void require(bool condition, const std::string& message) {
    ++checks;
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// Invalid-move diagnostics are expected in these tests; retain concise output.
class CaptureOutput {
public:
    CaptureOutput() : previous(std::cout.rdbuf(output.rdbuf())) {}
    ~CaptureOutput() { std::cout.rdbuf(previous); }
    CaptureOutput(const CaptureOutput&) = delete;
    CaptureOutput& operator=(const CaptureOutput&) = delete;
private:
    std::ostringstream output;
    std::streambuf* previous;
};

void runTest(const std::string& name, const std::function<void()>& test) {
    {
        CaptureOutput capture;
        test();
    }
    ++suites;
    std::cout << "PASS: " << name << '\n';
}

gameDefaults makeGame(std::initializer_list<std::string> rows) {
    gameDefaults game;
    game.rows = static_cast<int>(rows.size());
    game.cols = static_cast<int>(rows.begin()->size());
    game.board.clear();
    game.moveCount = 0;
    game.playerWins = {2, 3};
    game.gameCount = 6;
    game.drawCount = 1;
    game.names = {"Test X", "Test O"};
    for (const std::string& row : rows) {
        game.board.emplace_back(row.begin(), row.end());
        for (char cell : row) {
            if (cell != EMPTY) {
                ++game.moveCount;
            }
        }
    }
    return game;
}

bool samePositionAndStatistics(const gameDefaults& left,
                               const gameDefaults& right) {
    // Search is allowed to reset its own turnStartTime, but nothing else.
    return left.rows == right.rows && left.cols == right.cols
        && left.playerCount == right.playerCount && left.names == right.names
        && left.board == right.board && left.gameCount == right.gameCount
        && left.playerWins == right.playerWins && left.moveCount == right.moveCount
        && left.drawCount == right.drawCount;
}

bool emptyCell(const gameDefaults& game, int move) {
    return move >= 1 && move <= game.rows * game.cols
        && game.board[static_cast<std::size_t>((move - 1) / game.cols)]
                     [static_cast<std::size_t>((move - 1) % game.cols)] == EMPTY;
}

void requireUnchanged(const gameDefaults& game, const gameDefaults& before,
                      const std::string& operation) {
    require(samePositionAndStatistics(game, before),
            operation + " changed the board, configuration, or statistics");
}

void testMoveValidation() {
    gameDefaults game = makeGame({"X  ", " O ", "   "});
    const gameDefaults before = game;
    for (int invalid : {INT_MIN, -100, -1, 0, 10, 100, INT_MAX}) {
        require(!markBoard(invalid, PLAYER_1, game),
                "out-of-range cell accepted: " + std::to_string(invalid));
        requireUnchanged(game, before, "invalid cell");
    }
    for (int occupied : {1, 5}) {
        require(!markBoard(occupied, PLAYER_2, game), "occupied cell accepted");
        requireUnchanged(game, before, "occupied cell");
    }
    for (char invalidMark : {EMPTY, '?', 'x', '\0'}) {
        require(!markBoard(2, invalidMark, game), "invalid player mark accepted");
        requireUnchanged(game, before, "invalid player mark");
    }
    require(markBoard(9, PLAYER_1, game), "last valid cell was rejected");
    require(game.board[2][2] == PLAYER_1, "last cell mapped incorrectly");
    require(game.moveCount == before.moveCount,
            "markBoard must leave accepted-move counting to the game loop");
    gameDefaults expected = before;
    expected.board[2][2] = PLAYER_1;
    requireUnchanged(game, expected, "successful placement outside selected cell");

    gameDefaults rectangle = makeGame({"   ", "   "});
    require(markBoard(4, PLAYER_2, rectangle), "rectangular-board move rejected");
    require(rectangle.board[1][0] == PLAYER_2,
            "rectangular-board 1-based mapping is incorrect");
}

void testMalformedBoards() {
    const gameDefaults valid = makeGame({"   ", "   ", "   "});
    std::vector<gameDefaults> invalidBoards;
    for (int dimension : {INT_MIN, -1, 0, 1, MAX_BOARD_SIZE + 1, INT_MAX}) {
        gameDefaults badRows = valid;
        badRows.rows = dimension;
        invalidBoards.push_back(badRows);
        gameDefaults badCols = valid;
        badCols.cols = dimension;
        invalidBoards.push_back(badCols);
    }
    gameDefaults missingRows = valid;
    missingRows.board.clear();
    invalidBoards.push_back(missingRows);
    gameDefaults shortRows = valid;
    shortRows.board.pop_back();
    invalidBoards.push_back(shortRows);
    gameDefaults extraRows = valid;
    extraRows.board.push_back(std::vector<char>(3, EMPTY));
    invalidBoards.push_back(extraRows);
    gameDefaults shortColumns = valid;
    shortColumns.board[1].pop_back();
    invalidBoards.push_back(shortColumns);
    gameDefaults extraColumns = valid;
    extraColumns.board[1].push_back(EMPTY);
    invalidBoards.push_back(extraColumns);

    for (gameDefaults& game : invalidBoards) {
        const gameDefaults before = game;
        require(!markBoard(1, PLAYER_1, game), "malformed board accepted a move");
        require(aiMove(game) == -1, "random AI accepted a malformed board");
        require(findBestMove(game, PLAYER_1, std::chrono::milliseconds(0)) == -1,
                "Minimax accepted a malformed board");
        require(!checkWin(game, PLAYER_1), "malformed board reported a winner");
        requireUnchanged(game, before, "malformed-board rejection");
    }
}

void testWinRules() {
    const std::array<std::array<int, 3>, 8> winningLines = {{
        {{0, 1, 2}}, {{3, 4, 5}}, {{6, 7, 8}},
        {{0, 3, 6}}, {{1, 4, 7}}, {{2, 5, 8}},
        {{0, 4, 8}}, {{2, 4, 6}}
    }};
    for (char mark : {PLAYER_1, PLAYER_2}) {
        for (const auto& line : winningLines) {
            gameDefaults game = makeGame({"   ", "   ", "   "});
            for (int cell : line) {
                game.board[static_cast<std::size_t>(cell / 3)]
                          [static_cast<std::size_t>(cell % 3)] = mark;
            }
            require(checkWin(game, mark), "square winning line was missed");
            require(!checkWin(game, mark == PLAYER_1 ? PLAYER_2 : PLAYER_1),
                    "opponent incorrectly reported as winner");
            require(evaluate(game) == (mark == PLAYER_2 ? 10 : -10),
                    "terminal evaluation has the wrong sign");
        }
    }

    gameDefaults empty = makeGame({"   ", "   ", "   "});
    for (char invalid : {EMPTY, '?', 'x', '\0'}) {
        require(!checkWinRows(empty, invalid), "invalid mark won an empty row");
        require(!checkWinCols(empty, invalid), "invalid mark won an empty column");
        require(!checkWinDiag(empty, invalid), "invalid mark won an empty diagonal");
        require(!checkWin(empty, invalid), "invalid mark reported as winner");
    }
    require(evaluate(empty) == 0, "empty board has a terminal evaluation");

    const gameDefaults wideRow = makeGame({"XXXX", "O O "});
    const gameDefaults wideColumn = makeGame({"O X ", "O X "});
    const gameDefaults tallRow = makeGame({"XX", " O", " O", "  "});
    const gameDefaults tallColumn = makeGame({" O", "XO", " O", "XO"});
    require(checkWinRows(wideRow, PLAYER_1), "wide full row was missed");
    require(checkWinCols(wideColumn, PLAYER_2), "wide full column was missed");
    require(checkWinRows(tallRow, PLAYER_1), "tall full row was missed");
    require(checkWinCols(tallColumn, PLAYER_2), "tall full column was missed");
    require(!checkWinRows(makeGame({"XXX ", " O O"}), PLAYER_1),
            "three marks incorrectly win an incomplete four-cell row");
    require(!checkWinCols(makeGame({"X ", "XO", "X ", " O"}), PLAYER_1),
            "three marks incorrectly win an incomplete four-cell column");
    require(!checkWinDiag(makeGame({"X  ", " X "}), PLAYER_1),
            "wide rectangular board incorrectly has diagonal wins");
    require(!checkWinDiag(makeGame({"X ", " X", "  "}), PLAYER_1),
            "tall rectangular board incorrectly has diagonal wins");
}

void testFullnessAndTerminalMoves() {
    gameDefaults incomplete = makeGame({"XO ", "   ", "   "});
    incomplete.moveCount = 9;
    require(!isBoardFull(incomplete), "fullness incorrectly trusts moveCount");

    gameDefaults draw = makeGame({"XOX", "XOO", "OXX"});
    draw.moveCount = 0;
    require(isBoardFull(draw), "full board missed with stale moveCount");
    require(!checkWin(draw, PLAYER_1) && !checkWin(draw, PLAYER_2),
            "draw board incorrectly has a winner");
    require(evaluate(draw) == 0, "draw evaluation is not neutral");

    // X won on the ninth move; being full must not erase the winning score.
    gameDefaults lastCellWin = makeGame({"XOX", "OXO", "XOX"});
    require(isBoardFull(lastCellWin), "last-cell win board is not full");
    require(checkWin(lastCellWin, PLAYER_1), "last-cell win was missed");
    require(evaluate(lastCellWin) == -10, "full-board win was evaluated as a draw");

    gameDefaults earlyWin = makeGame({"XXX", "OO ", "   "});
    require(!isBoardFull(earlyWin), "early-win board was incorrectly full");
    for (gameDefaults game : {draw, lastCellWin, earlyWin}) {
        const gameDefaults before = game;
        for (char mark : {PLAYER_1, PLAYER_2}) {
            require(findBestMove(game, mark, std::chrono::seconds(60)) == -1,
                    "Minimax moved after game over");
        }
        require(aiMove(game) == -1, "random AI moved after game over");
        require(!markBoard(9, PLAYER_2, game), "move accepted after game over");
        requireUnchanged(game, before, "terminal-board operation");
    }

    const gameDefaults before = incomplete;
    require(findBestMove(incomplete, '?', std::chrono::seconds(60)) == -1,
            "Minimax accepted an invalid player mark");
    requireUnchanged(incomplete, before, "invalid AI mark");
}

void testReset() {
    gameDefaults game = makeGame({"XOX", " OO", "  X"});
    game.rows = 4;
    game.cols = 2;
    gameDefaults expected = game;
    expected.board.assign(4, std::vector<char>(2, EMPTY));
    expected.moveCount = 0;
    resetBoard(game);
    requireUnchanged(game, expected, "resetBoard");
    require(getAvailableMoves(game) == std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8}),
            "reset did not initialize the new rectangular dimensions");
}

void testRandomAI() {
    gameDefaults game = makeGame({"XO ", " OX", "X  "});
    const gameDefaults before = game;
    require(getAvailableMoves(game) == std::vector<int>({3, 4, 8, 9}),
            "available-move list has incorrect 1-based cells");
    for (int attempt = 0; attempt < 512; ++attempt) {
        require(emptyCell(game, aiMove(game)), "random AI returned a nonempty or invalid cell");
        requireUnchanged(game, before, "random AI selection");
    }

    gameDefaults oneLeft = makeGame({"XOX", "XOO", "OX "});
    require(aiMove(oneLeft) == 9, "random AI missed the only available cell");
    requireUnchanged(oneLeft, makeGame({"XOX", "XOO", "OX "}),
                     "single-choice random AI");
}

void requireBestMove(gameDefaults game, char mark, int expected,
                     const std::string& label) {
    const gameDefaults before = game;
    require(findBestMove(game, mark, std::chrono::seconds(60)) == expected, label);
    requireUnchanged(game, before, label);
}

void testTacticalAI() {
    requireBestMove(makeGame({"XX ", "OO ", "   "}), PLAYER_1, 3,
                    "X failed to take an immediate win");
    requireBestMove(makeGame({"XX ", "OO ", "X  "}), PLAYER_2, 6,
                    "O failed to take an immediate win");
    requireBestMove(makeGame({"OO ", "X  ", " X "}), PLAYER_1, 3,
                    "X failed to block O's immediate row win");
    requireBestMove(makeGame({"XX ", " O ", "   "}), PLAYER_2, 3,
                    "O failed to block X's immediate row win");
    requireBestMove(makeGame({"XOX", "XOO", "OX "}), PLAYER_1, 9,
                    "Minimax missed the only move to a draw");

    gameDefaults game = makeGame({"X  ", " O ", "  X"});
    const gameDefaults before = game;
    const int move = findBestMove(game, PLAYER_2, std::chrono::seconds(60));
    require(move == 2 || move == 4 || move == 6 || move == 8,
            "O did not avoid the opposite-corners fork");
    requireUnchanged(game, before, "fork defense");
}

void testTimeoutAndLargeBoards() {
    gameDefaults game = makeGame({"X  ", " O ", "   "});
    const gameDefaults before = game;
    for (char mark : {PLAYER_1, PLAYER_2}) {
        for (int attempt = 0; attempt < 32; ++attempt) {
            require(emptyCell(game, findBestMove(game, mark, std::chrono::milliseconds(0))),
                    "expired search did not return a legal fallback");
            requireUnchanged(game, before, "zero-budget fallback");
        }
    }
    require(emptyCell(game, findBestMove(game, PLAYER_1, std::chrono::milliseconds(-1))),
            "negative budget did not return a legal fallback");
    requireUnchanged(game, before, "negative-budget fallback");

    for (const auto& dimensions : {std::pair<int, int>{4, 4}, {2, 4}, {4, 2},
                                   {MAX_BOARD_SIZE, MAX_BOARD_SIZE}}) {
        gameDefaults large;
        large.rows = dimensions.first;
        large.cols = dimensions.second;
        large.playerWins = {0, 0};
        resetBoard(large);
        const gameDefaults original = large;
        require(markBoard(large.rows * large.cols, PLAYER_1, large),
                "largest 1-based cell rejected on a larger board");
        require(!markBoard(large.rows * large.cols + 1, PLAYER_2, large),
                "out-of-range move accepted on a larger board");
        resetBoard(large);
        requireUnchanged(large, original, "large-board reset");
        const auto started = std::chrono::steady_clock::now();
        const int selected = findBestMove(large, PLAYER_1, std::chrono::milliseconds(2));
        const auto elapsed = std::chrono::steady_clock::now() - started;
        require(emptyCell(large, selected), "larger-board AI returned an invalid cell");
        requireUnchanged(large, original, "larger-board timed search");
        // This is a generous termination check, not a hard real-time claim.
        require(elapsed < std::chrono::seconds(5),
                "larger-board search exceeded a generous termination allowance");
    }
}

// Independent oracle: flat positions and bit masks, not production win helpers,
// move helpers, evaluate(), alpha-beta pruning, or the production search.
char oracleWinner(const std::string& position) {
    const std::array<unsigned, 8> masks = {{
        0x007U, 0x038U, 0x1c0U, 0x049U,
        0x092U, 0x124U, 0x111U, 0x054U
    }};
    for (char mark : {'X', 'O'}) {
        unsigned bits = 0;
        for (std::size_t cell = 0; cell < position.size(); ++cell) {
            if (position[cell] == mark) {
                bits |= 1U << cell;
            }
        }
        for (unsigned mask : masks) {
            if ((bits & mask) == mask) {
                return mark;
            }
        }
    }
    return ' ';
}

char otherMark(char mark) {
    return mark == 'X' ? 'O' : 'X';
}

class Oracle {
public:
    int value(const std::string& position, char mark) {
        const std::string key = position + mark;
        const auto saved = cache.find(key);
        if (saved != cache.end()) {
            return saved->second;
        }
        const char winner = oracleWinner(position);
        if (winner != ' ') {
            return winner == mark ? 1 : -1;
        }
        int best = -2;
        for (std::size_t cell = 0; cell < position.size(); ++cell) {
            if (position[cell] == ' ') {
                std::string child = position;
                child[cell] = mark;
                const int candidate = -value(child, otherMark(mark));
                if (candidate > best) {
                    best = candidate;
                }
            }
        }
        if (best == -2) {
            best = 0;
        }
        cache.emplace(key, best);
        return best;
    }
private:
    std::unordered_map<std::string, int> cache;
};

struct Coverage {
    std::size_t ongoing = 0;
    std::size_t xTurns = 0;
    std::size_t oTurns = 0;
    std::size_t xWins = 0;
    std::size_t oWins = 0;
    std::size_t draws = 0;
    std::size_t forcedWins = 0;
    std::size_t forcedDraws = 0;
    std::size_t forcedLosses = 0;
};

void verifyReachablePosition(const std::string& position, char mark,
                            std::unordered_set<std::string>& visited,
                            Oracle& oracle, Coverage& coverage) {
    if (!visited.insert(position).second) {
        return;
    }
    gameDefaults game = makeGame({position.substr(0, 3), position.substr(3, 3),
                                  position.substr(6, 3)});
    const gameDefaults before = game;
    const char winner = oracleWinner(position);
    const bool full = position.find(' ') == std::string::npos;
    require(checkWin(game, PLAYER_1) == (winner == 'X'),
            "X winner mismatch on reachable position: " + position);
    require(checkWin(game, PLAYER_2) == (winner == 'O'),
            "O winner mismatch on reachable position: " + position);
    require(isBoardFull(game) == full, "fullness mismatch on reachable position");

    const int selected = findBestMove(game, mark, std::chrono::seconds(60));
    requireUnchanged(game, before, "exhaustive Minimax selection");
    if (winner != ' ' || full) {
        require(selected == -1, "Minimax moved in a terminal reachable position");
        require(aiMove(game) == -1, "random AI moved in a terminal reachable position");
        if (winner == 'X') {
            ++coverage.xWins;
        } else if (winner == 'O') {
            ++coverage.oWins;
        } else {
            ++coverage.draws;
        }
        return;
    }

    ++coverage.ongoing;
    if (mark == 'X') {
        ++coverage.xTurns;
    } else {
        ++coverage.oTurns;
    }
    const int best = oracle.value(position, mark);
    if (best == 1) {
        ++coverage.forcedWins;
    } else if (best == 0) {
        ++coverage.forcedDraws;
    } else {
        ++coverage.forcedLosses;
    }
    require(emptyCell(game, selected), "Minimax returned an illegal reachable-state move");
    std::string chosen = position;
    chosen[static_cast<std::size_t>(selected - 1)] = mark;
    require(-oracle.value(chosen, otherMark(mark)) == best,
            "Minimax chose a suboptimal move for " + std::string(1, mark)
                + " on position: [" + position + "]");
    require(emptyCell(game, aiMove(game)), "random AI returned an illegal reachable-state move");
    requireUnchanged(game, before, "exhaustive random AI selection");

    for (std::size_t cell = 0; cell < position.size(); ++cell) {
        if (position[cell] == ' ') {
            std::string child = position;
            child[cell] = mark;
            verifyReachablePosition(child, otherMark(mark), visited, oracle, coverage);
        }
    }
}

void testExhaustiveThreeByThree() {
    Oracle oracle;
    Coverage coverage;
    std::unordered_set<std::string> visited;
    verifyReachablePosition(std::string(9, ' '), 'X', visited, oracle, coverage);
    require(visited.size() == 5478, "reachable-state enumeration is incomplete");
    require(coverage.ongoing == 4520 && coverage.xTurns == 2423 && coverage.oTurns == 2097,
            "nonterminal turn coverage is incomplete");
    require(coverage.xWins == 626 && coverage.oWins == 316 && coverage.draws == 16,
            "terminal-state coverage is incomplete");
    require(coverage.forcedWins == 2836 && coverage.forcedDraws == 1052
                && coverage.forcedLosses == 632,
            "oracle outcome coverage differs from independently enumerated counts");
    require(oracle.value(std::string(9, ' '), 'X') == 0,
            "independent oracle does not solve the empty board as a draw");
}

} // namespace

int main() {
    try {
        runTest("invalid/occupied moves and 1-based cell mapping", testMoveValidation);
        runTest("malformed dimensions and board storage", testMalformedBoards);
        runTest("square/rectangular wins and invalid marks", testWinRules);
        runTest("board-derived fullness and terminal move rejection", testFullnessAndTerminalMoves);
        runTest("reset preserves statistics and resizes storage", testReset);
        runTest("random AI only selects available cells", testRandomAI);
        runTest("X/O wins, blocks, draw and fork defense", testTacticalAI);
        runTest("timeout fallback and bounded larger-board search", testTimeoutAndLargeBoards);
        runTest("all 5,478 reachable 3x3 states against an independent oracle", testExhaustiveThreeByThree);
    } catch (const std::exception& error) {
        std::cerr << "FAIL after " << checks << " checks: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    std::cout << "All " << suites << " suites passed (" << checks << " checks).\n"
              << "Exhaustive coverage: 4,520 optimal AI choices; 626 X wins; "
              << "316 O wins; 16 draws.\n";
    return EXIT_SUCCESS;
}
