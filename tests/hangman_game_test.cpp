#include "hangman_game.h"

#include <cassert>
#include <stdexcept>
#include <string>

namespace {

using hangman::GuessResult;
using hangman::HangmanGame;

void expectInvalidWord(const std::string& word) {
    try {
        HangmanGame game(word);
        (void)game;
        assert(false && "expected invalid word to throw");
    } catch (const std::invalid_argument&) {
    }
}

void testInitialState() {
    HangmanGame game("banana");

    assert(game.maskedWord() == "_ _ _ _ _ _");
    assert(game.guessedLetters().empty());
    assert(game.remainingGuesses() == HangmanGame::kMaxIncorrectGuesses);
    assert(!game.isWon());
    assert(!game.isLost());
}

void testCorrectGuessesRevealEveryOccurrence() {
    HangmanGame game("banana");

    assert(game.guess(" B ") == GuessResult::correct);
    assert(game.maskedWord() == "b _ _ _ _ _");
    assert(game.guess("a") == GuessResult::correct);
    assert(game.maskedWord() == "b a _ a _ a");
    assert(game.remainingGuesses() == HangmanGame::kMaxIncorrectGuesses);
    assert(game.guessedLetters() == "a b");
}

void testIncorrectAndDuplicateGuesses() {
    HangmanGame game("cat");

    assert(game.guess("x") == GuessResult::incorrect);
    assert(game.remainingGuesses() == 5);
    assert(game.guess(" X ") == GuessResult::duplicate);
    assert(game.remainingGuesses() == 5);
    assert(game.guessedLetters() == "x");
}

void testInvalidInputDoesNotChangeState() {
    HangmanGame game("cat");

    assert(game.guess("") == GuessResult::invalid);
    assert(game.guess("ab") == GuessResult::invalid);
    assert(game.guess("7") == GuessResult::invalid);
    assert(game.guess("!") == GuessResult::invalid);
    assert(game.guess("   ") == GuessResult::invalid);
    assert(game.guess("\xC3\xB1") == GuessResult::invalid);
    assert(game.remainingGuesses() == HangmanGame::kMaxIncorrectGuesses);
    assert(game.guessedLetters().empty());
}

void testWinAndGameOverResult() {
    HangmanGame game("cat");

    assert(game.guess("c") == GuessResult::correct);
    assert(game.guess("a") == GuessResult::correct);
    assert(game.guess("t") == GuessResult::correct);
    assert(game.isWon());
    assert(game.guess("x") == GuessResult::game_over);
    assert(game.remainingGuesses() == HangmanGame::kMaxIncorrectGuesses);
}

void testLossAndGameOverResult() {
    HangmanGame game("cat");

    for (const char* guess : {"b", "d", "e", "f", "g", "h"}) {
        assert(game.guess(guess) == GuessResult::incorrect);
    }

    assert(game.isLost());
    assert(game.remainingGuesses() == 0);
    assert(game.guess("c") == GuessResult::game_over);
}

void testWordValidation() {
    expectInvalidWord("");
    expectInvalidWord("Cat");
    expectInvalidWord("two words");
    expectInvalidWord("cat2");
}

}  // namespace

int main() {
    testInitialState();
    testCorrectGuessesRevealEveryOccurrence();
    testIncorrectAndDuplicateGuesses();
    testInvalidInputDoesNotChangeState();
    testWinAndGameOverResult();
    testLossAndGameOverResult();
    testWordValidation();
}
