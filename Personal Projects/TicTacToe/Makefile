CXX ?= g++
PYTHON ?= python3
CXXFLAGS ?= -O2
WARNINGS = -Wall -Wextra -Wpedantic -Wshadow
COMMON = -std=c++17 $(WARNINGS)
SANITIZERS = -O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer
ASAN_OPTIONS ?= detect_leaks=1

.PHONY: all test sanitize clean

all: build/tictactoe

build/tictactoe: Tic-Tac-Toe.cpp
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(COMMON) $(CXXFLAGS) $< $(LDFLAGS) -o $@

build/game-tests: tests/game-tests.cpp Tic-Tac-Toe.cpp
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(COMMON) $(CXXFLAGS) $< $(LDFLAGS) -o $@

test: build/tictactoe build/game-tests
	./build/game-tests
	$(PYTHON) tests/cli-tests.py ./build/tictactoe

build/sanitize/tictactoe: Tic-Tac-Toe.cpp
	mkdir -p build/sanitize
	$(CXX) $(CPPFLAGS) $(COMMON) $(SANITIZERS) $< $(LDFLAGS) -o $@

build/sanitize/game-tests: tests/game-tests.cpp Tic-Tac-Toe.cpp
	mkdir -p build/sanitize
	$(CXX) $(CPPFLAGS) $(COMMON) $(SANITIZERS) $< $(LDFLAGS) -o $@

sanitize: build/sanitize/tictactoe build/sanitize/game-tests
	ASAN_OPTIONS="$(ASAN_OPTIONS)" UBSAN_OPTIONS=halt_on_error=1 ./build/sanitize/game-tests
	ASAN_OPTIONS="$(ASAN_OPTIONS)" UBSAN_OPTIONS=halt_on_error=1 $(PYTHON) tests/cli-tests.py ./build/sanitize/tictactoe

clean:
	$(RM) -r build
