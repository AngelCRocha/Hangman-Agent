#include "hangman_game.h"

#include <array>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <random>
#include <string>
#include <string_view>

namespace {

std::string_view trim(std::string_view input) {
    const std::size_t first = input.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string_view::npos) {
        return {};
    }

    const std::size_t last = input.find_last_not_of(" \t\n\r\f\v");
    return input.substr(first, last - first + 1);
}

std::string lowercase(std::string_view input) {
    std::string result;
    result.reserve(input.size());
    for (char character : input) {
        result += character >= 'A' && character <= 'Z'
                      ? static_cast<char>(character - 'A' + 'a')
                      : character;
    }
    return result;
}

std::string selectWord(std::mt19937& generator) {
    if (const char* override_word = std::getenv("HANGMAN_WORD")) {
        return override_word;
    }

    static constexpr std::array<std::string_view, 8> words = {
        "apple", "bridge", "candle", "forest", "garden", "planet", "puzzle", "window"};
    std::uniform_int_distribution<std::size_t> distribution(0, words.size() - 1);
    return std::string(words[distribution(generator)]);
}

void displayGame(const hangman::HangmanGame& game) {
    std::cout << "\nWord: " << game.maskedWord() << '\n';
    std::cout << "Guessed letters: "
              << (game.guessedLetters().empty() ? "(none)" : game.guessedLetters()) << '\n';
    std::cout << "Remaining guesses: " << game.remainingGuesses() << '\n';
}

bool playRound(std::mt19937& generator) {
    hangman::HangmanGame game(selectWord(generator));

    while (!game.isFinished()) {
        displayGame(game);
        std::cout << "Guess a letter: ";

        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cout << "Input ended—goodbye.\n";
            return false;
        }

        switch (game.guess(input)) {
            case hangman::GuessResult::correct:
                std::cout << "Correct!\n";
                break;
            case hangman::GuessResult::incorrect:
                std::cout << "Incorrect.\n";
                break;
            case hangman::GuessResult::duplicate:
                std::cout << "You already guessed that letter.\n";
                break;
            case hangman::GuessResult::invalid:
                std::cout << "Enter exactly one letter.\n";
                break;
            case hangman::GuessResult::game_over:
                break;
        }
    }

    displayGame(game);
    if (game.isWon()) {
        std::cout << "You won! The word was " << game.answer() << ".\n";
    } else {
        std::cout << "You lost! The word was " << game.answer() << ".\n";
    }
    return true;
}

bool shouldReplay() {
    while (true) {
        std::cout << "Play again? (yes/no): ";

        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cout << "Input ended—goodbye.\n";
            return false;
        }

        const std::string response = lowercase(trim(input));
        if (response == "y" || response == "yes") {
            return true;
        }
        if (response == "n" || response == "no") {
            std::cout << "Thanks for playing!\n";
            return false;
        }
        std::cout << "Please answer yes or no.\n";
    }
}

}  // namespace

int main() {
    try {
        std::random_device device;
        std::mt19937 generator(device());

        std::cout << "Welcome to Hangman!\n";
        do {
            if (!playRound(generator)) {
                return 0;
            }
        } while (shouldReplay());
    } catch (const std::exception& error) {
        std::cerr << "Unable to start game: " << error.what() << '\n';
        return 1;
    }
}
