#!/usr/bin/env bash

set -euo pipefail

project_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
temp_dir=$(mktemp -d)
trap 'rm -rf "$temp_dir"' EXIT

binary="$temp_dir/hangman"
g++ -std=c++17 -Wall -Wextra -Werror \
  "$project_root/main.cpp" "$project_root/hangman_game.cpp" -o "$binary"

require_in_order() {
  local remaining=$1
  shift

  for expected in "$@"; do
    if [[ "$remaining" != *"$expected"* ]]; then
      echo "Expected output to contain: $expected" >&2
      exit 1
    fi
    remaining=${remaining#*"$expected"}
  done
}

win_output=$(printf 'x\nx\n!\nB\na\nn\nno\n' | HANGMAN_WORD=banana "$binary")
require_in_order "$win_output" \
  'Word: _ _ _ _ _ _' \
  'Remaining guesses: 6' \
  'Incorrect.' \
  'Guessed letters: x' \
  'Remaining guesses: 5' \
  'You already guessed that letter.' \
  'Enter exactly one letter.' \
  'Word: b _ _ _ _ _' \
  'Word: b a _ a _ a' \
  'You won! The word was banana.' \
  'Play again? (yes/no): ' \
  'Thanks for playing!'

loss_output=$(printf 'b\nd\ne\nf\ng\nh\nn\n' | HANGMAN_WORD=cat "$binary")
require_in_order "$loss_output" \
  'Remaining guesses: 6' \
  'Remaining guesses: 0' \
  'You lost! The word was cat.' \
  'Thanks for playing!'

guess_eof_output=$(HANGMAN_WORD=cat "$binary" </dev/null)
require_in_order "$guess_eof_output" 'Guess a letter: ' 'Input ended—goodbye.'

replay_eof_output=$(printf 'c\na\nt\n' | HANGMAN_WORD=cat "$binary")
require_in_order "$replay_eof_output" \
  'You won! The word was cat.' \
  'Play again? (yes/no): ' \
  'Input ended—goodbye.'

if HANGMAN_WORD=Cat "$binary" >/dev/null 2>&1; then
  echo 'Expected an invalid HANGMAN_WORD value to fail.' >&2
  exit 1
fi
