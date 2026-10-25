/// @file tic-tac-toe.cpp
/// @author David Tom <https://www.github.com/davidtom1194>
/// @date Nov. 12, 2023
///
/// @brief The program will allow for the playing of the game tic-tac-toe
/// between 2 entities, human and/or computer. When selecting 0 users, the
/// computer will play against itself by using two different algorithms:
/// one to select a move based on the available moves, the other (Minimax) 
/// will select a move that potentially is the best based on the algorithm's
/// specifications. If a user is selected, the AI will use Minimax. 
/// 
/// Standard 3x3 boards use full-depth Minimax with alpha-beta pruning when
/// the time budget permits. Larger boards use bounded search and may fall
/// back to a legal random move; they do not have a perfect-play guarantee.
/// The original performance estimates were based on an incorrect search
/// and must not be used as benchmarks for this version.

/* ========== Preprocessor Directive(s) ========== */
#include <iostream>     // input/output usage
#include <iomanip>      // output manipulation
#include <vector>       // std::vector usage
#include <string>       // std::string usage
#include <limits>       // numeric data usage
#include <cmath>        // log10
#include <cstdlib>      // EXIT_SUCCESS / EXIT_FAILURE
#include <algorithm>    // fill, find, min, max
#include <random>       // seeded-once random move selection
#include <sstream>      // strict integer-token parsing
#include <cstddef>      // std::size_t
#include <chrono>       // For benchmarking/timing
#include <cstdint>      // Convert to integer


/* ========== Global Constant(s) ========== */
// Defaults 

const int MAX_GAME_COUNT = 1;       // Source-configurable game count
const int MAX_BOARD_SIZE = 100;     // Max board size, *keep low for testing
const int MAX_PLAYER_COUNT = 2;     // Max player count
const int TIMER_CUTOFF = 5;         // Max time per turn in seconds

// Players
const char PLAYER_1 = 'X';      // Player 1's X
const char PLAYER_2 = 'O';      // Player 2's O
const char EMPTY = ' ';         // Empty Spaces

// Size
const std::string WELCOME_PROMPT = "Welcome to Tic-Tac-Toe.\n";
const std::string BOARD_SIZE_PROMPT = "Enter the desired board size: \n";
const std::string BOARD_SIZE_ERROR = "Invalid input! Enter a number between 2 and "
    + std::to_string(MAX_BOARD_SIZE) + "\n";
const std::string ROW_PROMPT = "Enter the number of rows: ";
const std::string COL_PROMPT = "Enter the number of columns: ";

// Number of Players
const std::string PLAYER_COUNT_PROMPT = "How many players are there? (0 for AI vs. AI): ";
const std::string PLAYER_COUNT_ERROR = "Invalid input! Enter a number between 0 and 2\n";

// Names
const std::string PLAYER_NAME_PROMPT = "Enter the name for Player ";

// Board
const std::string MARKER_PROMPT = "\nSelect a number to place your marks!";


/* ========== Struct Definition(s) ========== */
struct gameDefaults {
    int rows = 3;                                                   // board's row count
    int cols = 3;                                                   // board's col count
    int playerCount = MAX_PLAYER_COUNT;                             // game's player count
    std::vector<std::string> names = { "Computer", "Computer" };    // player names
    std::vector<std::vector<char>> board;                           // total board size from rows and cols
    int gameCount = 0;                                              // number of games played
    std::vector<int> playerWins;                                    // number of wins per player
    int moveCount = 0;                                              // number of moves by entities
    int drawCount = 0;                                              // number of draws
    std::chrono::steady_clock::time_point turnStartTime;            // tracks player's turn time
};  // end struct gameDefaults


/* ========== Function Prototypes ========== */
// Input Phase
enum class InputResult { Valid, Invalid, End };
InputResult readInteger(int& value);                               // validate the entire input token
bool boardSize(gameDefaults& game);                                // false on end of input
bool playerNumbers(gameDefaults& game);                            // false on end of input
bool playerNames(gameDefaults& game);                              // false on end of input

// Processing Phase 
void resetBoard(gameDefaults& game);                                // reset the game
bool hasValidBoard(const gameDefaults& game);                       // dimensions and storage agree
bool markBoard(int cellNum, char playerMark, gameDefaults& game);   // allow for selections
bool isBoardFull(const gameDefaults& game);                         // determine if the game is a draw
bool checkWinRows(const gameDefaults& game, char playerMark);       // check for win (rows)
bool checkWinCols(const gameDefaults& game, char playerMark);       // check for win (cols)
bool checkWinDiag(const gameDefaults& game, char playerMark);       // check for win (diag)
bool checkWin(const gameDefaults& game, char playerMark);           // check for win (total)

// Output Phase
int getSetWidth(int rows, int cols);                                // tracks setw
void printNames(const gameDefaults& game);                          // prints names of players
void printBoard(const gameDefaults& game);                          // prints board
void endOfGameStatistics(int64_t time, gameDefaults& game);         // prints game statistics

template <class Func, class... Args>
int64_t benchmark(const Func& function, Args... args);              // time for game

// Random AI
std::vector<int> getAvailableMoves(const gameDefaults& game);       // analyzes board for available moves
int aiMove(const gameDefaults& game);                               // AI selects position

// Minimax (AI) Algorithm
int evaluate(const gameDefaults& game);                             // analyzes board
int findBestMove(gameDefaults& game, char playerMark = PLAYER_2,
    std::chrono::milliseconds timeBudget = std::chrono::seconds(TIMER_CUTOFF));


/* ========== Main Function ========== */
#ifndef TICTACTOE_TEST
int main() {
    // Variable Declaration(s)
    int exitState = EXIT_SUCCESS;                   // tracks the status of the program
    gameDefaults game;                              // struct for game information
    bool endOfGame = false;                         // tracks game status

    /* ========== Input Phase ========== */
    if (!boardSize(game) || !playerNumbers(game) || !playerNames(game)) {
        std::cout << "\nInput ended. Exiting.\n";
        return EXIT_SUCCESS;
    }

    /* ========== Processing Phase ========== */
    // Resize game board based on inputs
    game.board.resize(game.rows, std::vector<char>(game.cols, EMPTY));
    game.playerWins.resize(MAX_PLAYER_COUNT, 0);    // initialize wins to zero

    // Time how long the game runs
    int64_t time = benchmark([&]() {
        while (game.gameCount < MAX_GAME_COUNT) {
            // Reset the game
            resetBoard(game);
            endOfGame = false;

            // Assign Player 1 as the starting player
            char currentPlayerMark = PLAYER_1;

            // If there are any human players available, then output game statistics
            if (game.playerCount > 0 && game.gameCount > 0) {
                std::cout << "Game Count: " << game.gameCount << std::endl;
                for (int i = 0; i < game.playerCount; ++i) {
                    std::cout << "Player " << i + 1
                        << " Win Count: " << game.playerWins[i]
                        << std::endl;
                }
            }

            while (!endOfGame) {
                // If there are human players, print the board
                if (game.playerCount > 0) {
                    printBoard(game);

                }

                // The selected mode determines control, never a player's name.
                const bool computerTurn = game.playerCount == 0
                    || (game.playerCount == 1 && currentPlayerMark == PLAYER_2);
                int cellChoice = -1;
                if (computerTurn) {
                    // AI-vs-AI: Minimax X versus random O. One human: Minimax O.
                    cellChoice = game.playerCount == 0 && currentPlayerMark == PLAYER_2
                        ? aiMove(game) : findBestMove(game, currentPlayerMark);
                } else {
                    // Prompt the player to select an available position
                    std::cout << "Player "
                        << (currentPlayerMark == PLAYER_1 ? "1" : "2")
                        << " (" << currentPlayerMark << "), choose a cell number: ";
                    const InputResult input = readInteger(cellChoice);
                    if (input == InputResult::End) {
                        std::cout << "\nInput ended. Game not completed.\n";
                        return;
                    }
                    if (input == InputResult::Invalid) {
                        std::cout << "Invalid input! Enter a whole cell number.\n";
                        continue;
                    }
                }

                if (!markBoard(cellChoice, currentPlayerMark, game)) {
                    if (computerTurn) {
                        std::cerr << "Computer failed to select a legal move.\n";
                        exitState = EXIT_FAILURE;
                        return;
                    }
                    continue;  // A rejected selection never consumes a turn.
                }
                ++game.moveCount;

                // Verify if the game is over
                if (checkWin(game, currentPlayerMark)) {
                    // Determine the index of the current player
                    int playerIndex = (currentPlayerMark == PLAYER_1) ? 0 : 1;

                    // Increment the win and game counts; end of game
                    game.playerWins[playerIndex]++;
                    game.gameCount++;
                    endOfGame = true;

                    // Show the final game board and a winner screen for human players
                    if (game.playerCount > 0) {
                        printBoard(game);
                        std::cout << std::endl << game.names[playerIndex] << " ("
                            << currentPlayerMark << ") is the Winner!\n" << std::endl;
                    }
                }

                // Determine if the game is a draw
                else if (isBoardFull(game)) {
                    // Increment the game counts and end of game
                    game.drawCount++;
                    game.gameCount++;
                    endOfGame = true;

                    // Output the final game board and a message for human players
                    if (game.playerCount > 0) {
                        printBoard(game);
                        std::cout << "\nThe game is a draw!\n";
                    }
                }

                // If game is not over, update current player for next turn
                if (!endOfGame) {
                    currentPlayerMark = (currentPlayerMark == PLAYER_1)
                        ? PLAYER_2 : PLAYER_1;
                }
            }
        }
        });

    /* ========== Output Phase ========== */
    // Output total game statistics
    printBoard(game);
    endOfGameStatistics(time, game);

    return exitState;
}
#endif


/* ========== Function Declarations ========== */

/* ========== Input Phase ========== */
// Parse one complete whitespace-delimited token. Reject fractions, suffixes,
// and integer overflow without poisoning std::cin for the next attempt.
InputResult readInteger(int& value) {
    std::string token;
    if (!(std::cin >> token)) {
        return InputResult::End;
    }
    std::istringstream input(token);
    int candidate = 0;
    char extra = '\0';
    if (!(input >> candidate) || (input >> extra)) {
        return InputResult::Invalid;
    }
    value = candidate;
    return InputResult::Valid;
}

bool readBoundedInteger(int& value, int minimum, int maximum,
    const std::string& prompt, const std::string& error) {
    while (true) {
        std::cout << prompt;
        const InputResult input = readInteger(value);
        if (input == InputResult::End) {
            return false;
        }
        if (input == InputResult::Valid && value >= minimum && value <= maximum) {
            return true;
        }
        std::cout << error;
    }
}

/// @brief Function will prompt the user to enter a board size for the game.
/// 
/// @details The function will prompt the user to set up the game's board by
/// asking for row and column sizes. Bounds and integrity checks are made to
/// verify that the inputs are valid within an acceptable range and are an
/// integer. Data will be stored in the `gameDefaults game` struct within the
///  variables `game.rows` and `game.cols`.
/// 
/// Example usage:
/// @code
/// boardSize(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
bool boardSize(gameDefaults& game) {
    // Prompt the user and obtain a board size
    std::cout << WELCOME_PROMPT << BOARD_SIZE_PROMPT;

    return readBoundedInteger(game.rows, 2, MAX_BOARD_SIZE, ROW_PROMPT, BOARD_SIZE_ERROR)
        && readBoundedInteger(game.cols, 2, MAX_BOARD_SIZE, COL_PROMPT, BOARD_SIZE_ERROR);
}

/// @brief Function will obtain the number of players in the game.
///
/// @details The function will prompt the user for an input to determine the
/// number of players in the game. Bounds and integry checks are made to verify
/// that the inputs are valid within an acceptable range and are an integer.
/// Data will be stored in the `gameDefaults game` struct within the variable
/// `game.playerCount`.
/// 
/// Example usage:
/// @code
/// playerNumbers(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
bool playerNumbers(gameDefaults& game) {
    return readBoundedInteger(game.playerCount, 0, MAX_PLAYER_COUNT,
        PLAYER_COUNT_PROMPT, PLAYER_COUNT_ERROR);
}

/// @brief Function will obtain the player names.
///
/// @details The function will prompt the user for an input to determine the
/// names of the players in the game. Names are stored as a string and do not
/// require the same bounds and integry checks used in previous functions.
/// Data will be stored in a vector within the `gameDefaults game` struct 
/// and are assigned to the variable `game.names[i]`.
/// 
/// Example usage:
/// @code
/// playerNames(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
bool playerNames(gameDefaults& game) {
    // Initiate loop to obtain player names
    for (int i = 0; i < game.playerCount; ++i) {
        std::cout << PLAYER_NAME_PROMPT << (i + 1) << ": ";
        if (!(std::cin >> game.names[i])) {
            return false;
        }
    }
    return true;
}

/* ========== Processing Phase ========== */
/// @note The benchmark function was obtained from the College of Southern
/// Nevada's Computer Science department during the attendance of Computer
/// Science 202 (CS 202) under Professor K. Mess.
/// 
/// @brief Measures the execution time of a function in nanoseconds.
///
/// @details This function template takes a callable and its arguments, executes
/// the callable with the provided arguments, and measures the execution time 
/// using high-resolution clock. It returns the duration taken for the function
/// call to execute in nanoseconds.
///
/// This can be used to benchmark algorithms, function calls, or any callable 
/// constructs. The function being benchmarked is called exactly once. The 
/// precision and accuracy of the benchmark depend on the resolution of the 
/// clock on the system where the code is run.
///
/// Example usage:
/// @code
/// auto result = benchmark([](int x){ return x * x; }, 10);
/// std::cout << "Time taken: " << result << " nanoseconds" << std::endl;
/// @endcode
///
/// @tparam Func The type of the callable to be benchmarked.
/// @tparam Args The types of the arguments to pass to the callable.
/// @param function A reference to the callable to be benchmarked.
/// @param args The arguments to pass to the callable.
/// @return The execution time of the callable in nanoseconds as an int64_t.
template <class Func, class... Args>
int64_t benchmark(const Func& function, Args... args) {
    using namespace std::chrono;

    const auto start = high_resolution_clock::now();
    function(args...);
    const auto stop = high_resolution_clock::now();

    return duration_cast<nanoseconds>(stop - start).count();
}

/// @brief Function will obtain a width to be used for output.
///
/// @details The function will first calculate the total number of cells across
/// the game board. Next it will determine the maximum number of digits that
/// will be used in the game board's formatting and output. 
/// 
/// The value for the variable `maxDigits`is obtained by first calculating
/// the base-10 logarithm of `totalCells + 1` (ex. log10(100) == 2), then 
/// uses std::ceil to round the value to the nearest whole number 
/// (ex. ceil(log10(26)) == 2) therefore allowing the program to know how many
/// digits will be needed.
/// 
/// Example usage:
/// @code
/// getSetWidth(game.rows, game.cols);
/// @endcode
/// 
/// @param rows The game board's total number of rows.
/// @param cols The game board's total number of columns.
/// 
/// @return width += 1 The std::setw() value used for formatting the board's
/// output within the terminal window.
int getSetWidth(int rows, int cols) {
    int totalCells = rows * cols;   // total number of cells on the board

    // maximum number of digits
    int maxDigits = static_cast<int>(
        std::ceil(
            std::log10(totalCells + 1)));  // start count at 1

    int width = maxDigits;          // sets the width for the setw variable

    return width += 1;
}

/// @brief Function ouputs the names of the players
///
/// @details The function applies appropriate formatting and outputs the 
/// names of the players within the game. The function accesses the struct
/// `game` to then output the vector values stored in `game.names[i]`.
/// 
/// Example usage:
/// @code
/// printNames(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
void printNames(const gameDefaults& game) {

    // Output empty line as spacer
    std::cout << std::endl;

    // Initiate loop through the number of names and output names
    for (std::size_t i = 0; i < game.names.size(); ++i) {
        std::cout << "Player "
            << (i + 1)
            << ": "
            << game.names[i]
            << std::endl;
    }
}

/// @brief Function will output the board to the screen based on the inputs
/// recieved by the program.
///
/// @details The function will output to the screen the current status of the
/// game board while also adhering to the appropriate formatting guidelines 
/// obtained by previous functions. The main purpose for the `printBoard`
/// function is to allow the user to visualize the board at three main points
/// in the program. The initial and empty game board, any player made updates 
/// to the game board, and the final game board at the end of the game.
/// 
/// Example usage:
/// @code
/// printBoard(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
void printBoard(const gameDefaults& game) {
    if (!hasValidBoard(game)) {
        return;
    }
    // Variable Declaration(s)
    const int INDENT = 10;  // Indents the game board
    int cellNumber = 1;     // Cell numbers are 1-based
    int setwValue = getSetWidth(game.rows, game.cols);     // Sets the column width

    // Output game header
    printNames(game);
    std::cout << MARKER_PROMPT << std::endl;

    // Loop to make game board
    std::cout << std::endl;
    for (int i = 0; i < game.rows; i++) {
        std::cout << std::setw(INDENT) << "";   // Indent the row
        for (int j = 0; j < game.cols; j++) {
            // Print the cell number or the player's mark
            if (game.board[i][j] == EMPTY) {
                std::cout << std::setw(setwValue) << cellNumber;
            }
            else {
                std::cout << std::setw(setwValue) << game.board[i][j];
            }

            cellNumber++;   // Increment cell number

            // Output the column barriers
            if (j < game.cols - 1) {
                std::cout << " | ";
            }
        }

        // Output the row barriers
        std::cout << std::endl;
        if (i < game.rows - 1) {
            std::cout << std::setw(INDENT) << "";   // Indent the row
            std::cout << std::string(game.cols * (setwValue + 3) - 1, '-')
                << std::endl;
        }
    }
    std::cout << std::endl;
}

/// @brief Function resets the board and any pertinent values.
///
/// @details The function will assign all `EMPTY` values to all the cells in
/// the game board in order to set or reset the game board.
/// 
/// Example usage:
/// @code
/// printNames(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
void resetBoard(gameDefaults& game) {
    game.moveCount = 0;
    if (game.rows < 2 || game.rows > MAX_BOARD_SIZE
        || game.cols < 2 || game.cols > MAX_BOARD_SIZE) {
        game.board.clear();
        return;
    }
    game.board.assign(game.rows, std::vector<char>(game.cols, EMPTY));
}

// All indexing helpers reject inconsistent dimensions instead of relying on
// the console setup to have allocated a board correctly.
bool hasValidBoard(const gameDefaults& game) {
    if (game.rows < 2 || game.rows > MAX_BOARD_SIZE
        || game.cols < 2 || game.cols > MAX_BOARD_SIZE
        || game.board.size() != static_cast<std::size_t>(game.rows)) {
        return false;
    }
    return std::all_of(game.board.begin(), game.board.end(), [&](const auto& row) {
        return row.size() == static_cast<std::size_t>(game.cols);
    });
}

/// @brief Function allows entity input to be used in the game
///
/// @details The function accepts input from an entity to be used as a
/// selection for the game. Bounds and integry checks are utilized throughout
/// the function.
/// 
/// Example usage:
/// @code
/// printNames(game);
/// @endcode
/// 
/// @param cellNum The cell number, derived from [i][j] used for selection.
/// @param playerMark The player's selection that is verified and used.
/// @param game The struct containing values regarding the game's values.
/// 
/// @return validCell A valid cell on the game board is input to the game.
bool markBoard(int cellNum, char playerMark, gameDefaults& game) {
    if (!hasValidBoard(game) || (playerMark != PLAYER_1 && playerMark != PLAYER_2)) {
        return false;
    }
    // Return before subtraction, division, or indexing, including for INT_MIN.
    if (cellNum < 1 || cellNum > game.rows * game.cols) {
        std::cout << "Invalid cell number. Please try again.\n";
        return false;
    }
    if (checkWin(game, PLAYER_1) || checkWin(game, PLAYER_2)) {
        return false;
    }

    // Calculate the rows and columns from the cell number
    int row = (cellNum - 1) / game.cols;
    int col = (cellNum - 1) % game.cols;

    // Verify if the input is acceptable based on the game board
    if (game.board[row][col] != EMPTY) {
        std::cout << "Cell is already occupied. Please try again.\n";
        return false;
    }
    game.board[row][col] = playerMark;
    return true;
}

/// @brief Function checks if the board is full.
///
/// @details The function acts as a boolean reference to determine whether
/// the game board is full or not. Check for a winner before declaring a draw.
/// Simulated AI moves do not update moveCount, so inspect the cells directly.
/// 
/// Example usage:
/// @code
/// if (isBoardFull(game)) {args...}
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
/// 
/// @return Tracker to determine whether the board is full or not from counts.
bool isBoardFull(const gameDefaults& game) {
    return hasValidBoard(game)
        && std::all_of(game.board.begin(), game.board.end(), [](const auto& row) {
            return std::find(row.begin(), row.end(), EMPTY) == row.end();
        });
}

/// @brief Function will check if a win is present.
///
/// @details The function will iteratively verify if the values are acceptable
/// for the given win condition. 
/// 
/// Example usage:
/// @code
/// checkWinRows(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
/// @param playerMark The player's given marker (ie. 'X' or 'O')
/// 
/// @return rowWin The determined win or lack thereof.
bool checkWinRows(const gameDefaults& game, char playerMark) {
    if (!hasValidBoard(game) || (playerMark != PLAYER_1 && playerMark != PLAYER_2)) {
        return false;
    }
    // Check rows
    for (int i = 0; i < game.rows; ++i) {
        bool rowWin = true;
        for (int j = 0; j < game.cols; ++j) {
            if (game.board[i][j] != playerMark) {
                rowWin = false;
                break;
            }
        }
        if (rowWin) return true;
    }

    return false;
}

/// @brief Function will check if a win is present.
///
/// @details The function will iteratively verify if the values are acceptable
/// for the given win condition. 
/// 
/// Example usage:
/// @code
/// checkWinCols(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
/// @param playerMark The player's given marker (ie. 'X' or 'O')
/// 
/// @return colWin The determined win or lack thereof.
bool checkWinCols(const gameDefaults& game, char playerMark) {
    if (!hasValidBoard(game) || (playerMark != PLAYER_1 && playerMark != PLAYER_2)) {
        return false;
    }
    // Check columns
    for (int j = 0; j < game.cols; ++j) {
        bool colWin = true;
        for (int i = 0; i < game.rows; ++i) {
            if (game.board[i][j] != playerMark) {
                colWin = false;
                break;
            }
        }
        if (colWin) return true;
    }

    return false;
}

/// @brief Function will check if a win is present.
///
/// @details The function will iteratively verify if the values are acceptable
/// for the given win condition. 
/// 
/// Example usage:
/// @code
/// checkWinDiag(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
/// @param playerMark The player's given marker (ie. 'X' or 'O')
/// 
/// @return diagWin The determined win or lack thereof.
bool checkWinDiag(const gameDefaults& game, char playerMark) {
    if (!hasValidBoard(game) || (playerMark != PLAYER_1 && playerMark != PLAYER_2)) {
        return false;
    }
    bool isWin = false;         // Flag to indicate a win condition
    // Check diagonals
    if (game.rows == game.cols) {
        bool diagWin1 = true;
        bool diagWin2 = true;
        for (int i = 0; i < game.rows; ++i) {
            if (game.board[i][i] != playerMark) {
                diagWin1 = false;
            }
            if (game.board[i][game.rows - i - 1] != playerMark) {
                diagWin2 = false;
            }
        }

        // Check if either diagonal is a win
        if (diagWin1 || diagWin2) {
            isWin = true;
        }
    }

    return isWin;
}

/// @brief Function will check if a win is present.
///
/// @details The function will iteratively verify if the values are acceptable
/// for the given win condition. 
/// 
/// Example usage:
/// @code
/// checkWin(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
/// @param playerMark The player's given marker (ie. 'X' or 'O')
/// 
/// @return checkWin* The determined win or lack thereof.
bool checkWin(const gameDefaults& game, char playerMark) {
    return checkWinRows(game, playerMark)
        || checkWinCols(game, playerMark)
        || checkWinDiag(game, playerMark);
}

/* ========== AI ========== */

/// @brief Return the empty cells as 1-based row-major cell numbers.
std::vector<int> getAvailableMoves(const gameDefaults& game) {
    std::vector<int> availableMoves;
    if (!hasValidBoard(game)) {
        return availableMoves;
    }

    for (int row = 0; row < game.rows; ++row) {
        for (int col = 0; col < game.cols; ++col) {
            if (game.board[row][col] == EMPTY) {
                availableMoves.push_back(row * game.cols + col + 1);
            }
        }
    }
    return availableMoves;
}

/// @brief Select an empty cell uniformly; return -1 if the game cannot continue.
int aiMove(const gameDefaults& game) {
    if (!hasValidBoard(game) || checkWin(game, PLAYER_1)
        || checkWin(game, PLAYER_2)) {
        return -1;
    }

    const std::vector<int> availableMoves = getAvailableMoves(game);
    if (availableMoves.empty()) {
        return -1;
    }

    // Seed once, rather than restarting the sequence on each move.
    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<std::size_t> distribution(
        0, availableMoves.size() - 1);
    return availableMoves[distribution(generator)];
}

/// @brief Score from O's perspective: O wins +10, X wins -10, otherwise 0.
int evaluate(const gameDefaults& game) {
    if (checkWin(game, PLAYER_2)) {
        return 10;
    }
    if (checkWin(game, PLAYER_1)) {
        return -10;
    }
    return 0;
}

namespace {

// A separate stop flag keeps a timeout/node limit out of the game scores.
struct SearchContext {
    std::chrono::steady_clock::time_point started;
    std::chrono::milliseconds timeBudget;
    std::size_t nodesRemaining;
    bool stopped = false;

    bool visitNode() {
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - started);
        if (nodesRemaining == 0 || elapsed >= timeBudget) {
            stopped = true;
            return false;
        }
        --nodesRemaining;
        return true;
    }
};

/// @brief Alpha-beta search; O maximizes and X minimizes the same score.
int minimax(gameDefaults& game, int depthRemaining, bool isMaximizer,
            int alpha, int beta, SearchContext& search) {
    if (!search.visitNode()) {
        return 0;
    }

    // A win on the final cell is a win, not a draw. Terminal positions stop.
    const int score = evaluate(game);
    if (score != 0) {
        return score;
    }
    if (isBoardFull(game) || depthRemaining == 0) {
        return 0;
    }

    int best = isMaximizer ? -11 : 11;
    const char playerMark = isMaximizer ? PLAYER_2 : PLAYER_1;
    for (int row = 0; row < game.rows; ++row) {
        for (int col = 0; col < game.cols; ++col) {
            if (game.board[row][col] != EMPTY) {
                continue;
            }
            game.board[row][col] = playerMark;
            const int value = minimax(game, depthRemaining - 1, !isMaximizer,
                                      alpha, beta, search);
            game.board[row][col] = EMPTY;

            // Always restore the trial move before stopping or pruning.
            if (search.stopped) {
                return 0;
            }
            if (isMaximizer) {
                best = std::max(best, value);
                alpha = std::max(alpha, best);
            }
            else {
                best = std::min(best, value);
                beta = std::min(beta, best);
            }
            if (alpha >= beta) {
                return best;
            }
        }
    }
    return best;
}

} // namespace

/// @brief Choose a move for the supplied mark without changing the board.
///
/// Boards with at most nine cells are searched to the end when time allows.
/// Larger boards use at most six plies and 100,000 nodes per turn; unsearched
/// nonterminal leaves score zero. This is a bounded opponent, not a guarantee
/// of optimal play on larger boards. An interrupted depth is discarded in
/// favor of the last completed depth, or a legal random move if none completed.
int findBestMove(gameDefaults& game, char playerMark,
                 std::chrono::milliseconds timeBudget) {
    if (!hasValidBoard(game)
        || (playerMark != PLAYER_1 && playerMark != PLAYER_2)
        || checkWin(game, PLAYER_1) || checkWin(game, PLAYER_2)) {
        return -1;
    }

    const std::vector<int> availableMoves = getAvailableMoves(game);
    if (availableMoves.empty()) {
        return -1;
    }

    game.turnStartTime = std::chrono::steady_clock::now();
    const bool smallBoard = game.rows * game.cols <= 9;
    const int maxDepth = smallBoard
        ? static_cast<int>(availableMoves.size())
        : std::min(6, static_cast<int>(availableMoves.size()));
    SearchContext search{game.turnStartTime, timeBudget,
        smallBoard ? std::numeric_limits<std::size_t>::max() : 100000};
    const bool isMaximizer = playerMark == PLAYER_2;
    int bestMove = -1;

    // Iterative deepening retains a result even if a later depth times out.
    for (int depth = 1; depth <= maxDepth; ++depth) {
        int iterationBest = isMaximizer ? -11 : 11;
        int iterationMove = -1;
        int alpha = -11;
        int beta = 11;
        for (const int cell : availableMoves) {
            const int row = (cell - 1) / game.cols;
            const int col = (cell - 1) % game.cols;
            game.board[row][col] = playerMark;
            const int value = minimax(game, depth - 1, !isMaximizer,
                                      alpha, beta, search);
            game.board[row][col] = EMPTY;
            if (search.stopped) {
                break;
            }
            if ((isMaximizer && value > iterationBest)
                || (!isMaximizer && value < iterationBest)) {
                iterationBest = value;
                iterationMove = cell;
            }
            if (isMaximizer) {
                alpha = std::max(alpha, iterationBest);
            }
            else {
                beta = std::min(beta, iterationBest);
            }
        }
        if (search.stopped) {
            break;
        }
        bestMove = iterationMove;
    }

    return bestMove == -1 ? aiMove(game) : bestMove;
}

/* ========== Output Phase ========== */

/// @brief Prints the game's statistics for human analysis.
/// 
/// @details Function will output the game's statistics, including the game 
/// duration (obtained from the benchmark function), the game count, and the
/// player's win counts. The function will track these values for both human 
/// and AI players and will output the appropriate messages to the screen
/// upon the end of the game.
///
/// Example usage:
/// @code
/// endOfCameStatistics(time, game);
/// @endcode
/// 
/// @param time A calculated time from the benchmark function.
/// @param game The struct containing values regarding the game's values.
void endOfGameStatistics(int64_t time, gameDefaults& game) {
    // Outputs game duration and count
    std::cout << "Game duration: " << time / 1e9 << " seconds" << std::endl;
    std::cout << "Game Count: " << game.gameCount << std::endl;
    std::cout << "Draw Count: " << game.drawCount << std::endl;
    std::cout << "Move Count: " << game.moveCount << std::endl;

    // Outputs player win counts
    for (std::size_t i = 0; i < game.playerWins.size(); ++i) {
        std::cout << "Player " << i + 1 << " Win Count : " << game.playerWins[i] << std::endl;
    }
}

