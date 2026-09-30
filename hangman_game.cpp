#include "hangman_game.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace hangman {
namespace {

bool isLowercaseLetter(char value) {
    return value >= 'a' && value <= 'z';
}

bool isLetter(char value) {
    return (value >= 'a' && value <= 'z') || (value >= 'A' && value <= 'Z');
}

char toLowercase(char value) {
    return value >= 'A' && value <= 'Z' ? static_cast<char>(value - 'A' + 'a') : value;
}

std::string_view trim(std::string_view input) {
    const std::size_t first = input.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string_view::npos) {
        return {};
    }

    const std::size_t last = input.find_last_not_of(" \t\n\r\f\v");
    return input.substr(first, last - first + 1);
}

bool isValidWord(const std::string& word) {
    return !word.empty() && std::all_of(word.begin(), word.end(), isLowercaseLetter);
}

}  // namespace

HangmanGame::HangmanGame(std::string word) : word_(std::move(word)) {
    if (!isValidWord(word_)) {
        throw std::invalid_argument("words must contain lowercase ASCII letters only");
    }
}

GuessResult HangmanGame::guess(std::string_view input) {
    if (isFinished()) {
        return GuessResult::game_over;
    }

    input = trim(input);
    if (input.size() != 1 || !isLetter(input.front())) {
        return GuessResult::invalid;
    }

    const char letter = toLowercase(input.front());
    if (guessed_letters_.find(letter) != std::string::npos) {
        return GuessResult::duplicate;
    }

    guessed_letters_.push_back(letter);
    std::sort(guessed_letters_.begin(), guessed_letters_.end());

    if (word_.find(letter) != std::string::npos) {
        return GuessResult::correct;
    }

    ++incorrect_guesses_;
    return GuessResult::incorrect;
}

const std::string& HangmanGame::answer() const {
    return word_;
}

std::string HangmanGame::guessedLetters() const {
    std::string result;
    for (char letter : guessed_letters_) {
        if (!result.empty()) {
            result += ' ';
        }
        result += letter;
    }
    return result;
}

std::string HangmanGame::maskedWord() const {
    std::string result;
    for (char letter : word_) {
        if (!result.empty()) {
            result += ' ';
        }
        result += guessed_letters_.find(letter) != std::string::npos ? letter : '_';
    }
    return result;
}

int HangmanGame::remainingGuesses() const {
    return kMaxIncorrectGuesses - incorrect_guesses_;
}

bool HangmanGame::isWon() const {
    return std::all_of(word_.begin(), word_.end(), [this](char letter) {
        return guessed_letters_.find(letter) != std::string::npos;
    });
}

bool HangmanGame::isLost() const {
    return incorrect_guesses_ >= kMaxIncorrectGuesses;
}

bool HangmanGame::isFinished() const {
    return isWon() || isLost();
}

}  // namespace hangman
