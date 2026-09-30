# Feature: Console Hangman Game

> Implementation tasks for the approved specification in [console-hangman-game-spec-20260930.md](console-hangman-game-spec-20260930.md).

## Feature Description

Replace the placeholder C++ program with a dependency-free, single-player console Hangman game. A player guesses letters in a hidden word, sees their progress and remaining incorrect guesses after every turn, and can begin another game after a win or loss.

## User Story

As a console-game player  
I want to guess letters in a hidden word and receive immediate, clear feedback  
So that I can play a complete, repeatable game of Hangman from the terminal.

## Problem Statement

The repository currently builds and runs a no-op C++ executable. It offers no gameplay, domain model, input validation, or automated regression coverage.

## Solution Statement

Implement the game rules in a small, testable `HangmanGame` class, and keep terminal input/output and replay control in `main.cpp`. Use a small built-in word list so the game has no runtime files or third-party dependencies. Normal play chooses a word randomly; `HANGMAN_WORD` may supply a valid lowercase test word for deterministic E2E tests. Normalize guesses case-insensitively, reject invalid and duplicate guesses without consuming an attempt, and use a fixed maximum of six incorrect guesses. Show remaining guesses only—do not render ASCII gallows art.

## Relevant Files

- `main.cpp` — replace the placeholder program with the interactive terminal loop and replay prompt.
- `test_runner.sh` — replace its current interactive one-command runner with a fail-fast, non-interactive build-and-test script using explicit source lists.
- `README.md` — document how to build, run, and test the game.
- `tests/README.md` — retain as the location guidance for new automated tests.

### New Files

- `hangman_game.h` — public game-state interface and constants.
- `hangman_game.cpp` — word validation, guess processing, win/loss evaluation, and display-word construction.
- `tests/hangman_game_test.cpp` — deterministic unit tests for game rules.
- `tests/e2e_console_hangman.sh` — scripted stdin/stdout smoke test for the interactive console experience.

## Implementation Plan

### Phase 1: Foundation

Define the game-state contract before connecting it to terminal I/O. The class receives a known word at construction, which makes its rules deterministic and independently testable. It owns the target word, guessed-letter set, and incorrect-guess count; callers can query the masked word, guessed letters, attempts remaining, and terminal state.

### Phase 2: Core Implementation

Implement a case-insensitive single-letter guess operation. It must distinguish accepted correct, accepted incorrect, duplicate, invalid, and terminal-game results. After trimming outer whitespace, only exactly one ASCII alphabetic character is valid. Only a new incorrect alphabetical letter decrements an attempt. A game wins when every distinct target letter has been guessed and loses after the sixth accepted incorrect guess; guesses after a terminal state are rejected without mutation.

### Phase 3: Integration

Choose a word at random from a short in-code list once per round, unless the `HANGMAN_WORD` environment variable supplies a valid test word. Render a consistently formatted masked word, sorted previous guesses, and attempts remaining before prompting. Continue until a terminal state, announce the outcome and answer, then accept a trimmed, case-insensitive `y`/`yes` or `n`/`no` replay response. On EOF during either prompt, print a brief goodbye and exit successfully. Add deterministic unit tests plus shell-driven E2E cases that assert ordered output.

## Step by Step Tasks

### 1. Define the game rules and interface

- Create `hangman_game.h` with a `HangmanGame` class, a six-strike constant, terminal-state queries, and a result type for correct, incorrect, duplicate, invalid, and post-terminal guesses.
- Decide and document that constructor words and `HANGMAN_WORD` values must be nonempty lowercase ASCII alphabetic strings. The built-in list meets this contract; invalid override values fail clearly rather than silently changing gameplay.
- Trim outer whitespace before validating guesses and replay answers. A guess must then be exactly one ASCII A–Z letter; internal whitespace and non-ASCII values are invalid.
- Keep all rule decisions in the domain class; do not put game rules in `main.cpp`.

### 2. Add the console end-to-end test first

- Create `tests/e2e_console_hangman.sh` to build into a temporary directory, clean it with `trap`, and run deterministic transcript cases with `HANGMAN_WORD`.
- Require two ordered-output cases: a `banana` win containing an incorrect guess, a duplicate, invalid input, repeated-letter reveal, final answer, and no-replay exit; and a `cat` loss after six distinct misses, final answer, and no-replay exit.
- Assert expected output in sequence, rather than unrelated loose matches, and make the script exit nonzero on any compilation, command, or assertion failure.
- Add an EOF case proving that closed guess input and closed replay input print the brief exit message and terminate without a prompt loop.

### 3. Implement and unit-test the game domain

- Implement `hangman_game.cpp` and `tests/hangman_game_test.cpp` together.
- Cover initial masking, correct guesses, incorrect guesses, case normalization, repeated letters in a word, duplicate guesses, invalid guesses, constructor validation, win detection, loss detection, and non-mutating post-terminal guesses.
- Compile tests with the production game implementation, not a copied or mocked version of its logic; use `-I.` so test files can include the root-level game header consistently.

### 4. Build the interactive game loop

- Replace `main.cpp` with a round loop that selects a word, displays a defined mask format (for example, `b _ n _ n _`), sorted guessed letters, and remaining guesses; it must read a whole input line, report validation/duplicate feedback, and show the final win/loss message with the answer.
- Add replay handling that accepts trimmed `y`/`yes` and `n`/`no` case-insensitively, reprompting for anything else. Detect failed `getline` at both prompts, print `Input ended—goodbye.`, and return success.
- Read `HANGMAN_WORD` before normal selection solely as a deterministic test seam; document it in test documentation, not normal player instructions.
- Seed word selection once at startup; avoid reseeding between rounds.

### 5. Make validation repeatable and document usage

- Replace `test_runner.sh` with `set -euo pipefail`, explicit C++17 source lists, builds under the ignored `build/` directory, the unit-test executable, and the console E2E script. It must never launch an unbounded interactive `./app`.
- Update `README.md` with build/run/test commands and a brief explanation of the six-miss rule.
- Run every validation command below and fix any failures before handoff.

## Testing Strategy

### Unit Tests

- A new game exposes one underscore per unguessed target letter and six attempts.
- Correct guesses reveal every matching occurrence and do not reduce attempts.
- Incorrect guesses reduce attempts exactly once per distinct letter.
- Uppercase input is accepted as its lowercase equivalent.
- Empty, multi-character, numeric, punctuation, whitespace-only, and duplicate guesses report the appropriate non-consuming result.
- Constructor and environment-override validation reject empty, uppercase, and nonalphabetic words according to the documented contract.
- A fully revealed word is won; six distinct misses lose; a post-terminal guess receives a non-mutating terminal result.

### Edge Cases

- Target words with repeated letters, such as `banana`.
- A correct letter guessed after several misses.
- A duplicate of both a correct and an incorrect guess.
- Input lines containing leading/trailing whitespace.
- Invalid replay answers and end-of-input from a redirected terminal session.
- A final correct guess that completes the word before all strikes are used.
- `HANGMAN_WORD` set to a valid deterministic word and to malformed values.
- EOF while awaiting a guess and while awaiting replay input.

## Acceptance Criteria

- The application compiles with the repository's C++ compiler command and launches an interactive Hangman round.
- The game displays masked progress, guessed letters, and attempts remaining on each turn.
- Exactly six distinct incorrect alphabetical guesses cause a loss; invalid and duplicate guesses do not count as misses.
- Letter guesses are case-insensitive and all repeated occurrences are revealed.
- The player receives an unambiguous win/loss message with the answer and can validly replay or exit.
- Unit tests and deterministic win, loss, and EOF console E2E cases pass through the project test script.
- The project test script returns without prompting for interactive input and leaves generated files only in ignored build or temporary directories.
- The README provides accurate run and test instructions.

## Validation Commands

Execute every command to validate the feature works correctly with zero regressions.

```bash
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Werror main.cpp hangman_game.cpp -o build/hangman
g++ -std=c++17 -Wall -Wextra -Werror -I. tests/hangman_game_test.cpp hangman_game.cpp -o build/hangman_game_tests
./build/hangman_game_tests
bash tests/e2e_console_hangman.sh
bash test_runner.sh
```

Manual gameplay check after automated validation:

```bash
./build/hangman
```

## Notes

- This plan intentionally excludes a graphical interface, persistent score history, categories, difficulty levels, external dictionaries, and ASCII-art gallows. Those can be separate features after the core loop is stable.
- The exact word list is not a game-rule concern; keep it small, family-friendly, and local to the console entry point unless future requirements call for reuse.
- The `HANGMAN_WORD` override is a test seam and should not be promoted as a normal gameplay feature.
