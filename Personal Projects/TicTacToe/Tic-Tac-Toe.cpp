/// @file tic-tac-toe.cpp
/// @author David Tom <https://github.com/davidtomofficial>
/// @date Nov. 12, 2023
///
/// @brief The program will allow for the playing of the game tic-tac-toe
/// between 2 entities, human and/or computer. When selecting 0 users, the
/// computer will play against itself by using two different algorithms:
/// one to select a move based on the available moves, the other (Minimax) 
/// will select a move that potentially is the best based on the algorithm's
/// specifications. If a user is selected, the AI will use Minimax. 
/// 
/// Program is optimized to run effectively with minimal intervention and may 
/// be used on larger game boards (ie. 30x30 ~ 5 seconds) quickly and 
/// efficiently by using Alpha-Beta Cut Offs. Additional modifications, such as
/// turn-based timers, allow the program to handle even larger game boards by
/// selecting a random move, albeit a much less effective approach to a
/// calculated approach.
/// 
/// Examples and estimated times:
/// 3x3     = 5.08e-05  seconds
/// 10x10   = 0.0153459 seconds 
/// 30x30   = 4.8283    seconds

/* ========== Preprocessor Directive(s) ========== */
#include <iostream>     // input/output usage
#include <iomanip>      // output manipulation
#include <vector>       // std::vector usage
#include <string>       // std::string usage
#include <limits>       // numeric data usage
#include <cmath>        // log10
#include <cstdlib>      // For rand() and srand()
#include <ctime>        // For time()
#include <chrono>       // For benchmarking/timing
#include <cstdint>      // Convert to integer


/* ========== Global Constant(s) ========== */
// Defaults 

const int MAX_GAME_COUNT = 1;      // Max game count, *keep low for testing
const int MAX_BOARD_SIZE = 100;     // Max board size, *keep low for testing
const int MAX_PLAYER_COUNT = 2;     // Max player count
const int TIMER_CUTOFF = 5;         // Max time per turn in seconds
const int TIME_EXCEEDED = INT_MIN;  // Max time exceeded

// Players
const char PLAYER_1 = 'X';      // Player 1's X
const char PLAYER_2 = 'O';      // Player 2's O
const char EMPTY = ' ';         // Empty Spaces

// Size
const std::string WELCOME_PROMPT = "Welcome to Tic-Tac-Toe.\n";
const std::string BOARD_SIZE_PROMPT = "Enter the desired board size: \n";
const std::string BOARD_SIZE_ERROR = "Invalid input! Enter a number between 1 and 10\n";
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
void boardSize(gameDefaults& game);                                 // obtain board size
void playerNumbers(gameDefaults& game);                             // obtain number of players
void playerNames(gameDefaults& game);                               // obtain player names

// Processing Phase 
void resetBoard(gameDefaults& game);                                // reset the game
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
int minimax(gameDefaults& game, int depth, bool isMaximizer);       // minimax algorithm
int findBestMove(gameDefaults& game);                               // selects best position from available moves and possible user selections


/* ========== Main Function ========== */
int main() {
    // Variable Declaration(s)
    int exitState = EXIT_SUCCESS;                   // tracks the status of the program
    gameDefaults game;                              // struct for game information
    bool playAgain = true;                          // tracks for additional games
    bool endOfGame = false;                         // tracks game status

    /* ========== Input Phase ========== */
    boardSize(game);                                // obtain desired board size
    playerNumbers(game);                            // obtain a player count
    playerNames(game);                              // obtain player name(s)

    /* ========== Processing Phase ========== */
    // Resize game board based on inputs
    game.board.resize(game.rows, std::vector<char>(game.cols, EMPTY));
    game.playerWins.resize(MAX_PLAYER_COUNT, 0);    // initialize wins to zero

    // Time how long the game runs
    int64_t time = benchmark([&]() {
        while (playAgain && game.gameCount < MAX_GAME_COUNT) {
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

                // Begin timer for the start of the turn
                game.turnStartTime = std::chrono::steady_clock::now();

                // Automate the game with AI moves, or allow the human to play
                if (game.playerCount == 0) {
                    // Use Minimax algorithm to select a position
                    int cellChoice = findBestMove(game);
                    if (cellChoice == TIME_EXCEEDED) {
                        cellChoice = aiMove(game);
                    }
                    markBoard(cellChoice, currentPlayerMark, game);
                }
                else if (currentPlayerMark == PLAYER_2 && game.names[1] == "Computer") {
                    // Randomly select an available position
                    int cellChoice = aiMove(game);
                    markBoard(cellChoice, currentPlayerMark, game);
                }
                else {
                    // Allow a human player to select an available position
                    int cellChoice;                 // stores selection for marker

                    // Prompt the player to select an available position
                    std::cout << "Player "
                        << (currentPlayerMark == PLAYER_1 ? "1" : "2")
                        << " (" << currentPlayerMark << "), choose a cell number: ";
                    std::cin >> cellChoice;
                    markBoard(cellChoice, currentPlayerMark, game);
                }

                game.moveCount++;       // increment move count

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
                if (isBoardFull(game)) {
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
                currentPlayerMark = (currentPlayerMark == PLAYER_1)
                    ? PLAYER_2 : PLAYER_1;
            }
        }
        });

    /* ========== Output Phase ========== */
    // Output total game statistics
    printBoard(game);
    endOfGameStatistics(time, game);

    return exitState;
}


/* ========== Function Declarations ========== */

/* ========== Input Phase ========== */
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
void boardSize(gameDefaults& game) {
    // Prompt the user and obtain a board size
    std::cout << WELCOME_PROMPT << BOARD_SIZE_PROMPT;

    // Rows
    std::cout << ROW_PROMPT;
    // Verify input is an integer and within an acceptable range
    while (!(std::cin >> game.rows) || (game.rows < 2 || game.rows > MAX_BOARD_SIZE)) {
        std::cin.clear();                   // Clear the error flag
        // Ignore the bad input
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << BOARD_SIZE_ERROR;      // Prompt the user for a valid input
    }

    // Columns
    std::cout << COL_PROMPT;
    // Verify input is an integer and within an acceptable range
    while (!(std::cin >> game.cols) || (game.cols < 2 || game.cols > MAX_BOARD_SIZE)) {
        std::cin.clear();                   // Clear the error flag
        // Ignore the bad input
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << BOARD_SIZE_ERROR;      // Prompt the user for a valid input
    }
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
void playerNumbers(gameDefaults& game) {
    // Prompt the user and obtain a board size
    std::cout << PLAYER_COUNT_PROMPT;

    // Verify input is an integer and within an acceptable range
    while (!(std::cin >> game.playerCount) || (game.playerCount < 0 || game.playerCount > 2)) {
        std::cin.clear();           // Clear the error flag
        // Ignore the bad input
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << PLAYER_COUNT_ERROR;       // Prompt the user for a valid input
    }
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
void playerNames(gameDefaults& game) {
    // Initiate loop to obtain player names
    for (int i = 0; i < game.playerCount; ++i) {
        std::cout << PLAYER_NAME_PROMPT << (i + 1) << ": ";
        std::cin >> game.names[i];
    }
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
    for (int i = 0; i < game.names.size(); i++) {
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
    // Variable Declaration(s)
    const int INDENT = 10;  // Indents the game board
    int cellNumber = 1;     // Start numbering from 0
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
    game.moveCount = 0;                 // reset moveCount to 0

    // loop through the indexes 
    for (int i = 0; i < game.rows; ++i) {
        std::fill(game.board[i].begin(), game.board[i].end(), EMPTY);
    }
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
    bool validCell = false;     // tracks the validity of the selection

    // Verify the input is within an acceptable range
    if (cellNum < 1 || cellNum > game.rows * game.cols) {
        std::cout << "Invalid cell number. Please try again.";
    }

    // Calculate the rows and columns from the cell number
    int row = (cellNum - 1) / game.cols;
    int col = (cellNum - 1) % game.cols;

    // Verify if the input is acceptable based on the game board
    if (game.board[row][col] == EMPTY) {
        game.board[row][col] = playerMark;
        validCell = true;
    }
    else {
        std::cout << "Cell is alread occupied. Please try again.";
    }

    return validCell;
}

/// @brief Function checks if the board is full.
///
/// @details The function acts as a boolean reference to determine whether
/// the game board is full or not. If the game board is full, then the game
/// ends in a draw.
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
    return game.moveCount == (game.rows * game.cols) ? true : false;
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

/// @brief Function will identify which values are `EMPTY`
///
/// @details The function's main purpose is to correlate data between the
/// AI algorithms in order to generate an outcome based on a given set of 
/// requirements and data. The program is a basic example of iteratively
/// identifying any `EMPTY` elements in the game board.
/// 
/// Example usage:
/// @code
/// variable = getAvailableMoves(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
/// 
/// @return availableMoves A vector containing the available coordinates.
std::vector<int> getAvailableMoves(const gameDefaults& game) {
    std::vector<int> availableMoves;    // vector of available moves

    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            if (game.board[i][j] == EMPTY) {
                availableMoves.push_back(i * game.cols + j + 1); // Convert to 1-based cell number
            }
        }
    }
    return availableMoves;
}

/// @brief Function will randomly select a value from a known list of moves.
///
/// @details The function utilizes information obtained from the 
/// `availableMoves()` function, alongside a random number generator, to 
/// select a random move within the index of available moves.
/// 
/// Example usage:
/// @code
/// aiMove(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
/// 
/// @return availableMoves
int aiMove(const gameDefaults& game) {
    // Obtain available moves
    std::vector<int> availableMoves = getAvailableMoves(game);
    int randomIndex = -1;           // -1 if no moves are available

    if (!availableMoves.empty()) {
        // Seed for random number generation
        srand(static_cast<long>(time(nullptr)));
        int randomIndex = rand() % availableMoves.size();
    }
    return availableMoves[randomIndex];
}

/// @brief Function evaluates the current state of the game to estimate win.
///
/// @details The function checks the game's current state to see if either
/// entity has won. It's typically used in game AI to evaluate the desirability
/// of a game state. A positive value is returned if the AI wins, a negative
/// value is returned if the other entity wins, and zero if there is no winner.
/// 
/// Example usage:
/// @code
/// int score = evaluate(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
/// 
/// @return returnValue An integer score representing the game state.
int evaluate(const gameDefaults& game) {
    int returnValue = 0;        // default; no win detected

    // Check for AI win
    if (checkWin(game, PLAYER_2)) {
        returnValue = 10;
    }
    // Check for Player 1 win
    else if (checkWin(game, PLAYER_1)) {
        returnValue = -10;
    }

    return returnValue;
}

/// @brief Function will evaluate whether to maximize or minimize its moves.
///
/// @details The function will evaluate a given set of conditions and decide
/// to maximize its possibility of winning the game, or minimize the opponent's
/// chance of winning. The function requires a considerable amount of memory
/// compared to the random selection used in the previous functions.
/// 
/// Example usage:
/// @code
/// printNames(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
/// @param depth A depiction of how far the program will predict future values.
/// @param isMaximizer A variable to determine whether to maximize or minimize.
/// 
/// @return scoreValue The value to be used for prediction by Minimax.
int minimax(gameDefaults& game, int depth, bool isMaximizer) {
    int score = evaluate(game);         // tracks the evaluation score
    int scoreValue = 0;                 // tracks the return value
    int alpha = 0;                      // alpha-beta cut off variable
    int beta = 0;                       // alpha-beta cut off variable

    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - game.turnStartTime).count();

    if (duration >= TIMER_CUTOFF) {
        return TIME_EXCEEDED;                 // time exceeded value
    }

    // If Maximizer or Minimizer has won the game, return the evaluated score
    if (score == 10 || score == -10) {
        scoreValue = score;
    }

    // If there are no more moves and no winner, return 0 (draw)
    if (isBoardFull(game)) {
        scoreValue = 0;
    }

    if (scoreValue < 0) {
        // Maximizer's move
        if (isMaximizer) {
            int best = -1000;
            for (int i = 0; i < game.rows; ++i) {
                for (int j = 0; j < game.cols; ++j) {
                    if (game.board[i][j] == EMPTY) {
                        game.board[i][j] = PLAYER_2;    // AI's move
                        best = std::max(best, minimax(game, depth + 1, !isMaximizer));
                        game.board[i][j] = EMPTY;       // Undo the move

                        // Cut off
                        alpha = std::max(alpha, best);
                        if (beta <= alpha) {            // Beta cut-off
                            break;
                        }
                    }
                }

                // Cut off
                if (beta <= alpha) {                    // Beta cut-off
                    break;
                }
            }
            
            scoreValue = best;
        }
    }
    // Minimizer's move
    else if (scoreValue > 0) {
        int best = 1000;
        for (int i = 0; i < game.rows; ++i) {
            for (int j = 0; j < game.cols; ++j) {
                if (game.board[i][j] == EMPTY) {
                    game.board[i][j] = PLAYER_1;    // Player's move
                    best = std::min(best, minimax(game, depth + 1, !isMaximizer));
                    game.board[i][j] = EMPTY;       // Undo the move

                    // Cut off
                    beta = std::min(beta, best);
                    if (beta <= alpha) {            // Alpha cut-off
                        break;
                    }
                }
            }

            // Cut off
            if (beta <= alpha) {                    // Alpha cut-off
                break;
            }
        }
        scoreValue = best;
    }
    else {
        scoreValue = 0;
    }

    return scoreValue;
}

/// @brief Function will determine the best move to make based on Minimax.
///
/// @details The best move is predicted based on the values obtained from the
/// AI algorithm `Minimax`. Assuming the best move is predicted then the move
/// will be decided based on the series of available input values.
/// 
/// Example usage:
/// @code
/// printNames(game);
/// @endcode
/// 
/// @param game The struct containing values regarding the game's values.
/// 
/// @return bestMove Is the calculated best move for the AI to make.
int findBestMove(gameDefaults& game) {
    // Variable Declaration(s)
    int bestVal = -1000;
    int bestMove = -1;

    // Loop through the game board and make predictions
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            if (game.board[i][j] == EMPTY) {
                game.board[i][j] = PLAYER_2;            // Make the move
                int moveVal = minimax(game, 0, false);  // Calculate the move's value
                game.board[i][j] = EMPTY;               // Undo the move

                if (moveVal > bestVal) {
                    bestVal = moveVal;
                    bestMove = i * game.cols + j + 1;   // Convert to 1-based cell number
                }
            }
        }
    }

    return bestMove;
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

    // Outputs player win counts
    for (int i = 0; i < game.playerWins.size(); ++i) {
        std::cout << "Player " << i + 1 << " Win Count : " << game.playerWins[i] << std::endl;
    }
}
