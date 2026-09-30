# Spec: Console Hangman Game

## Objective

Build a complete, dependency-free C++17 Hangman game for a terminal user. Each round randomly selects a word from a small built-in list; the player enters letter guesses until every letter is revealed or six distinct incorrect guesses occur. The terminal shows the masked word, sorted guesses, and remaining guesses, then offers replay after a clear win or loss result.

The game succeeds when a player can complete repeated rounds interactively and automated tests can deterministically verify all rule and console-flow behavior.

### Confirmed Product Decisions

- Show remaining guesses; do not render ASCII gallows art.
- Use a random, built-in word list in normal gameplay; no external dictionary or word file.
- Treat guesses case-insensitively. After trimming outer whitespace, a guess is valid only when it is exactly one ASCII letter.
- Invalid and duplicate guesses never consume a remaining guess.
- Support a deterministic test-only environment override: `HANGMAN_WORD=<lowercase-word>`.
- When input ends during a guess or replay prompt, print `Input ended—goodbye.` and exit successfully.

## Tech Stack

- Language: ISO C++17.
- Compiler: `g++` with `-Wall -Wextra -Werror`.
- Runtime: standard terminal input/output using the C++ standard library.
- Dependencies: none; do not add packages or frameworks.
- Test approach: self-contained C++ assertion-style unit tests plus Bash E2E tests that feed scripted stdin and verify ordered output.

## Commands

Build the playable application:

```bash
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Werror main.cpp hangman_game.cpp -o build/hangman
```

Run a normal random-word game:

```bash
./build/hangman
```

Build and run unit tests:

```bash
g++ -std=c++17 -Wall -Wextra -Werror -I. tests/hangman_game_test.cpp hangman_game.cpp -o build/hangman_game_tests
./build/hangman_game_tests
```

Run deterministic console tests and the complete project suite:

```bash
bash tests/e2e_console_hangman.sh
bash test_runner.sh
```

## Project Structure

```text
main.cpp                         interactive game/replay loop and built-in word selection
hangman_game.h                   game-domain public interface and result types
hangman_game.cpp                 game rules and state transitions
tests/hangman_game_test.cpp      deterministic unit tests for domain behavior
tests/e2e_console_hangman.sh     deterministic terminal transcript tests
test_runner.sh                   fail-fast non-interactive build-and-test entry point
README.md                        player/developer run and test instructions
specs/                           feature specification and implementation plan
build/                           ignored generated executables
```

## Code Style

- Keep game rules in `HangmanGame`; keep `main.cpp` responsible for terminal I/O, word selection, and replay control only.
- Use explicit names, `const` for query operations, and small standard-library types rather than new abstractions or dependencies.
- Let the domain object normalize and validate a complete guess string so its input rules can be unit-tested; keep terminal reading and messaging in `main.cpp`.
- Avoid decorators, global mutable game state, and a separate UI framework.

Example domain boundary:

```cpp
enum class GuessResult { correct, incorrect, duplicate, invalid, game_over };

class HangmanGame {
public:
    explicit HangmanGame(std::string word);
    GuessResult guess(std::string_view input);
    bool isWon() const;
    bool isLost() const;
};
```

The exact names may vary, but the interface must preserve these responsibilities and distinguish each result.

## Functional Requirements

### Word Selection

- Normal play selects a word randomly from a small, family-friendly built-in lowercase ASCII word list once per round.
- `HANGMAN_WORD` may override normal selection only as a deterministic automated-test seam.
- A constructor word and an override word must be nonempty lowercase ASCII alphabetic strings. Invalid override values fail clearly rather than silently falling back to random selection.

### Gameplay Rules

- Start each round with six remaining incorrect guesses.
- Display unrevealed letters as a stable, human-readable mask such as `_ _ _`; reveal every matching occurrence after a correct guess.
- Display guessed letters in a stable sorted order and display the remaining-guess count before each prompt.
- Trim leading and trailing whitespace from a guess. Accept exactly one ASCII alphabetic character, normalized case-insensitively.
- A new correct guess reveals letters without reducing the remaining count.
- A new incorrect guess reduces the remaining count by one.
- Invalid guesses and duplicate guesses show feedback but do not change state.
- Win when every distinct letter in the word has been guessed. Lose on the sixth distinct incorrect guess.
- Reject guesses after win/loss with a non-mutating terminal result. The console loop should not normally submit one.

### End of Round and Replay

- State an unambiguous win or loss and reveal the target word.
- Trim replay input and accept `y`/`yes` and `n`/`no`, case-insensitively; reprompt on other values.
- `y`/`yes` starts a new independently selected round; `n`/`no` exits successfully.
- EOF during a guess or replay prompt writes `Input ended—goodbye.` and exits successfully without looping.

## Testing Strategy

### Unit Tests

`tests/hangman_game_test.cpp` must test the production `hangman_game.cpp` implementation for:

- Initial mask and six remaining guesses.
- Correct and incorrect guesses.
- Case normalization and words containing repeated letters, including `banana`.
- Duplicate correct and incorrect guesses that do not consume an attempt.
- Empty, multi-character, numeric, punctuation, whitespace-only, and non-ASCII input as invalid.
- Constructor validation for empty, uppercase, and nonalphabetic words.
- Win state, loss state, and non-mutating `game_over` behavior.

### End-to-End Tests

`tests/e2e_console_hangman.sh` must compile in a temporary directory, clean that directory with `trap`, and use `HANGMAN_WORD` to run deterministic ordered-transcript checks:

- A `banana` winning round including an incorrect guess, a duplicate, invalid input, repeated-letter reveal, answer display, and no-replay exit.
- A `cat` losing round after six distinct incorrect guesses, answer display, and no-replay exit.
- EOF at the guess prompt and at the replay prompt, each producing the exact goodbye behavior and a successful exit.

Assertions must check output in meaningful sequence, not merely independent text matches. The project script must use `set -euo pipefail`, compile explicit source lists into `build/`, run unit and E2E tests, and never start an unbounded interactive session.

## Boundaries

- Always: compile with warnings treated as errors; validate input; keep builds/tests non-interactive; run the full test script before handoff; keep generated files in ignored locations.
- Ask first: add a third-party package; change the C++ standard; change the six-miss rule; add data files, scoring, categories, difficulty modes, or CI configuration.
- Never: commit generated executables or secrets; remove tests to make a build pass; add ASCII gallows art, a graphical interface, persistent scores, or external word dictionaries within this feature.

## Success Criteria

- The program builds as C++17 with `-Wall -Wextra -Werror` and plays an interactive round.
- Normal rounds randomly choose from a built-in list, while `HANGMAN_WORD` enables deterministic test runs.
- The terminal consistently shows mask, sorted guesses, and remaining guesses; no ASCII gallows is rendered.
- Six distinct incorrect letters lose; correct, duplicate, and invalid input follow the defined state-change rules.
- Case-insensitive win/loss and replay paths, including EOF, produce the specified results.
- Unit tests and deterministic win, loss, and EOF E2E tests pass via `bash test_runner.sh` without interactive input.
- `README.md` accurately documents normal build, run, and test commands, but does not advertise the test-only override as a player feature.

## Open Questions

None. The feature direction and testability decisions were explicitly confirmed by the requester on 2026-09-30.

## Related Plan

The ordered implementation tasks and validation checkpoints are in [console-hangman-game-plan-20260930.md](console-hangman-game-plan-20260930.md).
