#ifndef HANGMAN_GAME_H
#define HANGMAN_GAME_H

#include <string>
#include <string_view>

namespace hangman {

enum class GuessResult { correct, incorrect, duplicate, invalid, game_over };

class HangmanGame {
public:
    static constexpr int kMaxIncorrectGuesses = 6;

    explicit HangmanGame(std::string word);

    GuessResult guess(std::string_view input);
    const std::string& answer() const;
    std::string guessedLetters() const;
    std::string maskedWord() const;
    int remainingGuesses() const;
    bool isWon() const;
    bool isLost() const;
    bool isFinished() const;

private:
    std::string word_;
    std::string guessed_letters_;
    int incorrect_guesses_ = 0;
};

}  // namespace hangman

#endif
