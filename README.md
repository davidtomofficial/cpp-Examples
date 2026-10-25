# C++

Historical C++ coursework and personal practice projects covering programming fundamentals, data structures, algorithms, and a Tic-Tac-Toe experiment. Original work by [David Tom](https://github.com/davidtomofficial).

**Status: Historical educational examples.** Project descriptions reflect the original work. The Tic-Tac-Toe source has been corrected and tested as documented in its [build and regression guide](Personal%20Projects/TicTacToe/README.md); other examples have not undergone full behavioral or current-platform validation. This collection is separate from current David Tom Solutions (DTS) and David Tom Publishing (DTP) offerings.

## File descriptions

The descriptions below record original learning objectives and intended features. Validation results for the updated game are documented in its guide; other descriptions remain historical.

- Vowel Counter: [midtermB.cpp](midtermB/midtermB.cpp)
    - file streams
    - selection statements

- Book Cipher: [pa08.cpp](pa08/pa08.cpp)
    - functions
    - arrays
    - file streams

- Grade Calculator: [pa09.cpp](pa09/pa09.cpp)
    - functions
    - structs
    - command line arguments

- Grade Report: [finalB.cpp](finalB/finalB.cpp)
    - functions
    - arrays
    - file streams
    - structs

- Pointer and DNA Nucleotides: [pa13a.cpp](pa13/pa13a.cpp)
    - Makefile
    - stream overloads
    - pointers
    - vectors
    - arrays
 
 - Pointer and Grade Calculator: [pa13b.cpp](pa13b/pa13b.cpp)
    - Makefile
    - pointers
    - vectors
    - arrays
  
- Pointer and Word Counter: [pa14.cpp](pa14/pa14.cpp)
  - Makefiles
  - stream overload functions
  - pointers
  - vectors
  - structs
  - no memory leaks
  - selection statements
  - selection sort algorithm

- Recursive Demonstration: [pa15.cpp](pa15/pa15.cpp)
  - proper code documentation
  - proper implementation of Makefiles
  - recursive functions
  - pointers
  - vectors
  - no memory leaks
  - selection statements
  - selection sort algorithm
  - and an appropriate output based on the validity of the program's operation.

- Iterators, Sorting, and Searching Algorithms: [pa16.cpp](pa16/pa16.cpp)
  - proper code documentation
  - proper implementation of Makefiles
  - iterators
  - sorting algorithms (ie. bubble sort, selection sort, insertion sort, and std::sort)
  - searching algorithms (ie. sequential sort & binary sort)
  - pointers
  - vectors
  - structs
  - no memory leaks
  - and an appropriate output based on the validity of the program's operation.

- Linked Lists and Templates: [pa17b.cpp](pa17b/pa17b.cpp)
    - proper code documentation
    - proper implementation of Makefiles
    - linked lists
    - template files
    - no memory leaks
    - and an appropriate output based on the validity of the program's operation.

- Stacks and Infix/Postfix Notation: [pa18.cpp](pa18/pa18.cpp)
    - proper code documentation
    - proper implementation of Makefiles
    - iterators
    - linked lists
    - stacks
    - infix/postfix notations
    - no memory leaks
    - selection statements
    - and an appropriate output based on the validity of the program's operation.

- Personal Projects - Tic Tac Toe: [Tic-Tac-Toe.cpp](Personal%20Projects/TicTacToe/Tic-Tac-Toe.cpp)
  - proper code documentation
  - AI vs AI, AI vs Human, and Human vs Human
  - AI Algorithms: Minimax with Alpha-Beta Cut Off & Random Selections
  - dynamic game board size
  - customizable AI game counts
  - benchmarking function
  - bounds checking and input verification
  - pointers
  - vectors
  - structs
  - no memory leaks
  - and an appropriate output based on the validity of the program's operation.

## Getting Started

Build each program separately with a C++17 compiler. For example, from the repository root:

```sh
c++ -std=c++17 -Wall -Wextra -Wpedantic midtermB/midtermB.cpp -o /tmp/vowel-counter
/tmp/vowel-counter
```

The `pa13` program also needs its implementation file when linking:

```sh
c++ -std=c++17 -Wall -Wextra -Wpedantic pa13/pa13a.cpp pa13/Vector.cpp -o /tmp/pa13a
```

For the game and its behavioral regressions, see the
[Tic-Tac-Toe build and test guide](Personal%20Projects/TicTacToe/README.md).

Several historical coursework tests include `catch.hpp`, which is not bundled
here. Their test framework must be provided separately. The legacy Makefiles
also contain course-specific tools and submission targets; inspect the desired
target before invoking it in another environment. Existing checked-in executables
are historical artifacts and do not automatically reflect source changes.

## Contributing

See the repository-local [contribution guide](CONTRIBUTING.md) for reporting issues and submitting focused pull requests while preserving the educational context.

## Authors

* **David Tom** - *Initial Work* - [davidtomofficial](https://github.com/davidtomofficial)

See this repository's [commit history](https://github.com/davidtomofficial/cpp-Examples/commits/main/) for recorded changes. Additional authors and course-provided material remain credited in their respective files.

## License

The existing licensing statement is retained:

> This project is licensed under the GNU General Public License Version 3.

Read the [GNU GPL version 3 text](https://www.gnu.org/licenses/gpl-3.0.html). This repository currently has no local `LICENSE` file. Earlier READMEs linked to a license in the separate GitHub profile repository; that file is not a project-local license.

Some bundled material has separate attribution: [linked-list tests](pa17b/LList-test.cxx) and [stack tests](pa18/Stack-test.cxx) credit Kevin Mess, and [Vector.h](pa13/Vector.h) describes material adapted from the CSN CS202 shared directory. Those attributions must be preserved; this update does not establish permission to relicense third-party material.

Existing file-specific notices and third-party attribution remain unchanged. The maintainer still needs to document the scope of the repository-level statement, confirm the version wording where file-specific notices do not already settle it, and add the appropriate local license text. This documentation update does not change existing licensing terms.

## Acknowledgements

* A big thanks to any and all contributors, no matter how big or small the contribution it all helps.
