# Console Hangman

A dependency-free C++17 terminal version of Hangman. Guess the letters in a hidden word before making six distinct incorrect guesses.

## Build and Play

```bash
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Werror main.cpp hangman_game.cpp -o build/hangman
./build/hangman
```

Enter one letter per turn. Guesses are case-insensitive; invalid and repeated guesses do not reduce the remaining-guess count. After each round, answer `yes` or `no` to play again or exit.

If input ends unexpectedly, the game exits cleanly with a short message.

## Test

Run the complete non-interactive test suite:

```bash
bash test_runner.sh
```

It builds the application and unit tests in `build/`, then runs deterministic console end-to-end tests. Generated files remain in ignored build or temporary directories.

## Container

This repository is compatible with [cpp-container](https://github.com/ChicoState/cpp-container). To open a shell in the container:

```bash
docker run -v "$(pwd)":/usr/src -it cpp-container sh
```

Run the build, play, and test commands above from that shell.
