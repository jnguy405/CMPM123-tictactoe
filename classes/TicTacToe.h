#pragma once
#include "Game.h"
#include "Square.h"

//
// the classic game of tic tac toe
//

//
// the main game class
//
class TicTacToe : public Game
{
public:
    TicTacToe();
    ~TicTacToe();

    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() const override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder *holder) override;
    bool        canBitMoveFrom(Bit*bit, BitHolder *src) override;
    bool        canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst) override;
    void        stopGame() override;

	void        updateAI() override;
    bool        gameHasAI() override { return true; }
    BitHolder &getHolderAt(const int x, const int y) override { return _grid[y][x]; }
    
    // AI evaluation tracking
    std::vector<std::pair<int, int>> getLastAIEvaluations() const { return _lastAIEvaluations; }
    int getLastAIChoice() const { return _lastAIChoice; }
    
private:
    Bit *       PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int index ) const;
    bool        aiTestForTerminalState(const std::string& state);
    int         aiBoardEvaluation(const std::string& state);
    int         negamax(std::string state, int depth, int alpha, int beta, int playerColor);
    
    std::vector<std::pair<int, int>> _lastAIEvaluations;  // pair of (position, score)
    int _lastAIChoice;

    Square      _grid[3][3];
};

