#!/usr/bin/env bash

set -euo pipefail

project_root=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
build_dir="$project_root/build"
mkdir -p "$build_dir"

g++ -std=c++17 -Wall -Wextra -Werror \
  "$project_root/main.cpp" "$project_root/hangman_game.cpp" -o "$build_dir/hangman"
g++ -std=c++17 -Wall -Wextra -Werror -I"$project_root" \
  "$project_root/tests/hangman_game_test.cpp" "$project_root/hangman_game.cpp" \
  -o "$build_dir/hangman_game_tests"

"$build_dir/hangman_game_tests"
bash "$project_root/tests/e2e_console_hangman.sh"
