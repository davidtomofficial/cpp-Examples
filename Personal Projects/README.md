# Personal Projects

> Historical educational example. See the [repository overview](../README.md) for context and current documentation status.

Historical C++ practice projects and personal experiments preserved alongside the coursework in this repository. See the [Tic-Tac-Toe README](TicTacToe/README.md) for the original project description.

# File descriptions
- Tic Tac Toe: Tic-Tac-Toe.cpp
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

- Historical example timings (not revalidated):
  - 3x3     = 5.08e-05  seconds
  - 10x10   = 0.0153459 seconds 
  - 30x30   = 4.8283    seconds

## Getting Started

Clone/download the files and any dependencies for your project then compile and run the programs using the linux command:
  - "$ c++ $CXXFLAGS [programName.cpp] -o [programName]"
  - "./[programName]

### Prerequisites

In order to run any C++ based programs you must first have installed C++ and its dependencies on a device that can compile
and run the program.  You can install the appropriate software many ways:
1. Visual Studio
2. Geany
3. Repository: Most if not all linux distros have a repository containing the necessary software that can be accessed using the
   below example commands (more information can be found by accessing the manual/help page for your operating system):
   - For Debian/GNU Linux:
         - "$ sudo apt install build-essential"
   - RHEL
         - "$ sudo dnf install build-essential"

Occasionally programs may require dependencies; the easiest way to properly install them is by searching GitHub repositories
and cloning/downloading the appropriate files for your project.  This can be done in:
    - a web browser
    - **or**
    - through the "git clone ssh://git@github.com/[username]/[repository-name].git" command
Once downloaded, import the library and restart your IDE.

### Example installation
1. Download the appropriate dependencies and software
2. Install as needed
3. Import any libraries/dependencies for your project (most projects will state this in the first few lines)
4. Clone the files from the repository
5. Upload the code to your device (laptop, Desktop, etc.).
6. Contribute to further developments.
7. Repeat as needed

## Contributing

See the repository-local [contribution guide](../CONTRIBUTING.md) for reporting issues and submitting focused pull requests while preserving the educational context.

## Authors

* **David Tom** - *Initial Work* - [davidtomofficial](https://github.com/davidtomofficial)

See this repository's [commit history](https://github.com/davidtomofficial/cpp-Examples/commits/main/) for recorded changes. Additional authors and course-provided material remain credited in their respective files.

## License

See the [repository licensing notes](../README.md#license) for the retained GNU GPL version 3 statement, existing file-specific notices, and the scope clarification still needed before adding local license documentation.

## Acknowledgements

* A big thanks to any and all contributors, no matter how big or small the contribution it all helps.
