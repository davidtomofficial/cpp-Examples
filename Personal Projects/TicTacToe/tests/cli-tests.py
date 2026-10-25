#!/usr/bin/env python3
"""Black-box console regressions. Run: python3 tests/cli-tests.py ./tic-tac-toe.

Uses only Python's standard library. Each subprocess has a timeout so an input
failure or a stuck game fails the suite instead of hanging the test runner.
"""

import pathlib
import re
import subprocess
import sys
import unittest


EXECUTABLE = None


def token_input(*tokens):
    return "\n".join(str(token) for token in tokens) + "\n"


class ConsoleTests(unittest.TestCase):
    def run_game(self, data):
        result = subprocess.run(
            [str(EXECUTABLE)],
            input=data,
            text=True,
            capture_output=True,
            timeout=20,
            check=False,
        )
        self.assertEqual(
            result.returncode,
            0,
            f"Exit status {result.returncode}\n{result.stderr}\n{result.stdout}",
        )
        self.assertEqual(result.stderr, "", result.stderr)
        return result.stdout

    def counter(self, output, label):
        matches = re.findall(re.escape(label) + r"\s*:\s*(\d+)", output)
        self.assertTrue(matches, f"Missing {label!r} counter\n{output}")
        return int(matches[-1])

    def final_board(self, output, rows=3, cols=3):
        board_rows = []
        for line in output.splitlines():
            cells = [part.strip() for part in line.split("|")]
            if len(cells) == cols and all(
                re.fullmatch(r"[XO]|\d+", cell) for cell in cells
            ):
                board_rows.append(cells)
        self.assertGreaterEqual(len(board_rows), rows, output)
        cells = [cell for row in board_rows[-rows:] for cell in row]
        for index, cell in enumerate(cells, start=1):
            if cell not in ("X", "O"):
                self.assertEqual(cell, str(index), "Empty cell numbering changed")
        return "".join(cell if cell in ("X", "O") else "." for cell in cells)

    def assert_result(self, output, moves, x_wins, o_wins, draws, board=None,
                      rows=3, cols=3):
        self.assertEqual(self.counter(output, "Game Count"), 1)
        self.assertEqual(self.counter(output, "Move Count"), moves)
        self.assertEqual(self.counter(output, "Player 1 Win Count"), x_wins)
        self.assertEqual(self.counter(output, "Player 2 Win Count"), o_wins)
        self.assertEqual(self.counter(output, "Draw Count"), draws)
        self.assertEqual(x_wins + o_wins + draws, 1)
        actual_board = self.final_board(output, rows, cols)
        self.assertEqual(actual_board.count("X") + actual_board.count("O"), moves)
        self.assertEqual(actual_board.count("X"), (moves + 1) // 2)
        self.assertEqual(actual_board.count("O"), moves // 2)
        if board is not None:
            self.assertEqual(actual_board, board)

    def human_game(self, moves, rows=3, cols=3, names=("Alice", "Bob")):
        return self.run_game(token_input(rows, cols, 2, *names, *moves))

    def test_two_humans_x_wins(self):
        output = self.human_game([1, 4, 2, 5, 3])
        self.assert_result(output, 5, 1, 0, 0, "XXXOO....")
        self.assertIn("Alice (X) is the Winner!", output)

    def test_two_humans_o_wins(self):
        output = self.human_game([1, 4, 2, 5, 9, 6])
        self.assert_result(output, 6, 0, 1, 0, "XX.OOO..X")
        self.assertIn("Bob (O) is the Winner!", output)

    def test_draw(self):
        output = self.human_game([1, 2, 3, 5, 4, 6, 8, 7, 9])
        self.assert_result(output, 9, 0, 0, 1, "XOXXOOOXX")
        self.assertIn("The game is a draw!", output)

    def test_last_cell_win_is_not_also_a_draw(self):
        output = self.human_game([1, 2, 3, 4, 5, 6, 8, 7, 9])
        self.assert_result(output, 9, 1, 0, 0, "XOXOXOOXX")
        self.assertNotIn("The game is a draw!", output)

    def test_invalid_cells_do_not_advance_turn_or_move_count(self):
        output = self.human_game([
            0, -1, -2147483648, 10, 2147483647, 1,
            1, 0, 4,
            4, 2,
            2, 5,
            5, 3,
        ])
        self.assert_result(output, 5, 1, 0, 0, "XXXOO....")

    def test_bad_numeric_tokens_do_not_advance_turn(self):
        output = self.human_game([
            "garbage", "1.5", "1abc", "999999999999999999999999999", 1,
            "oops", "-999999999999999999999999999", 4,
            2, 5, 3,
        ])
        self.assert_result(output, 5, 1, 0, 0, "XXXOO....")

    def test_setup_rejects_bad_dimensions_and_player_counts(self):
        output = self.run_game(token_input(
            "abc", "3abc", "1.5", -1, 0, 1, 101, 2147483647, 3,
            "999999999999999999999999999", 0, 101, 3,
            "xyz", "2abc", -1, 3, 2,
            "Alice", "Bob", 1, 4, 2, 5, 3,
        ))
        self.assert_result(output, 5, 1, 0, 0, "XXXOO....")

    def test_human_named_computer_stays_human(self):
        output = self.human_game([1, 4, 2, 5, 3], names=("Computer", "Computer"))
        self.assert_result(output, 5, 1, 0, 0, "XXXOO....")
        self.assertIn("Player 2 (O), choose a cell number:", output)

    def test_two_by_two_board(self):
        output = self.human_game([1, 2, 4], rows=2, cols=2)
        self.assert_result(output, 3, 1, 0, 0, "XO.X", rows=2, cols=2)

    def test_wide_rectangle_full_column_wins(self):
        output = self.human_game([1, 2, 4], rows=2, cols=3)
        self.assert_result(output, 3, 1, 0, 0, "XO.X..", rows=2, cols=3)

    def test_tall_rectangle_full_row_wins(self):
        output = self.human_game([1, 3, 2], rows=3, cols=2)
        self.assert_result(output, 3, 1, 0, 0, "XXO...", rows=3, cols=2)

    def test_wide_rectangle_partial_diagonal_does_not_win(self):
        output = self.human_game([1, 2, 5, 3, 4], rows=2, cols=3)
        self.assert_result(output, 5, 1, 0, 0, "XOOXX.", rows=2, cols=3)

    def test_tall_rectangle_diagonal_like_marks_can_draw(self):
        output = self.human_game([1, 2, 4, 3, 6, 5], rows=3, cols=2)
        self.assert_result(output, 6, 0, 0, 1, "XOOXOX", rows=3, cols=2)

    def test_human_vs_minimax_completes_without_human_win(self):
        for opening in (1, 2, 5):
            with self.subTest(opening=opening):
                # Repeated sweeps supply another empty cell after any rejected
                # occupied choice, regardless of the computer's chosen moves.
                moves = [opening] + list(range(1, 10)) * 5
                output = self.run_game(token_input(3, 3, 1, "Alice", *moves))
                wins = self.counter(output, "Player 2 Win Count")
                draws = self.counter(output, "Draw Count")
                moves_played = self.counter(output, "Move Count")
                self.assertTrue(5 <= moves_played <= 9, output)
                self.assertEqual(wins + draws, 1, output)
                self.assert_result(output, moves_played, 0, wins, draws)
                self.assertNotIn("Player 2 (O), choose a cell number:", output)

    def test_computer_vs_computer_completes_without_minimax_loss(self):
        output = self.run_game(token_input(3, 3, 0))
        wins = self.counter(output, "Player 1 Win Count")
        draws = self.counter(output, "Draw Count")
        moves_played = self.counter(output, "Move Count")
        self.assertTrue(5 <= moves_played <= 9, output)
        self.assertEqual(wins + draws, 1, output)
        self.assert_result(output, moves_played, wins, 0, draws)
        self.assertNotIn("choose a cell number:", output)

    def test_eof_at_every_setup_stage_terminates(self):
        for tokens in ((), (3,), (3, 3), (3, 3, 2), (3, 3, 2, "Alice"),
                       ("abc",), (3, "abc"), (3, 3, "abc")):
            with self.subTest(tokens=tokens):
                output = self.run_game(token_input(*tokens) if tokens else "")
                counts = re.findall(r"Game Count:\s*(\d+)", output)
                self.assertTrue(all(int(count) == 0 for count in counts), output)

    def test_eof_during_game_does_not_complete_game(self):
        output = self.run_game(token_input(3, 3, 2, "Alice", "Bob", 1, 5))
        self.assertEqual(self.counter(output, "Game Count"), 0)
        self.assertEqual(self.counter(output, "Move Count"), 2)
        self.assertEqual(self.counter(output, "Draw Count"), 0)
        self.assertEqual(self.counter(output, "Player 1 Win Count"), 0)
        self.assertEqual(self.counter(output, "Player 2 Win Count"), 0)
        self.assertEqual(self.final_board(output), "X...O....")

    def test_eof_after_invalid_move_does_not_complete_game(self):
        output = self.run_game(token_input(3, 3, 2, "Alice", "Bob", 1, "bad"))
        self.assertEqual(self.counter(output, "Game Count"), 0)
        self.assertEqual(self.counter(output, "Move Count"), 1)
        self.assertEqual(self.final_board(output), "X........")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("usage: python3 tests/cli-tests.py PATH_TO_GAME_EXECUTABLE")
    EXECUTABLE = pathlib.Path(sys.argv[1]).resolve()
    if not EXECUTABLE.is_file():
        raise SystemExit(f"Game executable does not exist: {EXECUTABLE}")
    unittest.main(argv=[sys.argv[0]], verbosity=2)
