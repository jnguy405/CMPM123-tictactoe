#include "TicTacToe.h"

// -----------------------------------------------------------------------------
// TicTacToe.cpp
// -----------------------------------------------------------------------------
// This file is intentionally *full of comments* and gentle TODOs that guide you
// through wiring up a complete Tic‑Tac‑Toe implementation using the game engine’s
// Bit / BitHolder grid system.
//
// Rules recap:
//  - Two players place X / O on a 3x3 grid.
//  - Players take turns; you can only place into an empty square.
//  - First player to get three-in-a-row (row, column, or diagonal) wins.
//  - If all 9 squares are filled and nobody wins, it’s a draw.
//
// Notes about the provided engine types you'll use here:
//  - Bit              : a visual piece (sprite) that belongs to a Player
//  - BitHolder        : a square on the board that can hold at most one Bit
//  - Player           : the engine’s player object (you can ask who owns a Bit)
//  - Game options     : let the mouse know the grid is 3x3 (rowX, rowY)
//  - Helpers you’ll see used: setNumberOfPlayers, getPlayerAt, startGame, etc.
//
// I’ve already fully implemented PieceForPlayer() for you. Please leave that as‑is.
// The rest of the routines are written as “comment-first” TODOs for you to complete.
// -----------------------------------------------------------------------------

const int AI_PLAYER   = 1;      // index of the AI player (O)
const int HUMAN_PLAYER= -1;      // index of the human player (X)

TicTacToe::TicTacToe()
{
}

TicTacToe::~TicTacToe()
{
}

// -----------------------------------------------------------------------------
// make an X or an O
// -----------------------------------------------------------------------------
// DO NOT CHANGE: This returns a new Bit with the right texture and owner
Bit* TicTacToe::PieceForPlayer(const int playerNumber) {
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == 1 ? "x.png" : "o.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

//
// setup the game board, this is called once at the start of the game
//
void TicTacToe::setUpBoard() {
    // set number of players to 2
    setNumberOfPlayers(2);
    
    // set _gameOptions.rowX and rowY to 3
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;
    
    // for y in 0 to 2
    for (int y = 0; y < 3; y++) {
        // for x in 0 to 2
        for (int x = 0; x < 3; x++) {
            // calculate position based on x,y
            ImVec2 position(100.0f + x * 110.0f, 100.0f + y * 110.0f);
            // call _grid[y][x].initHolder with position, color, and "square.png"
            _grid[y][x].initHolder(position, "square.png", x, y);
        }
    }
    
    // call startGame to begin
    startGame();
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool TicTacToe::actionForEmptyHolder(BitHolder *holder) {
    // Step 1: Guard clause
    if (!holder) return false;

    // Step 2: Check if empty
    if (holder->bit() != nullptr) return false;

    // Step 3: Place current player's piece
    int currentPlayerIndex = getCurrentPlayer()->playerNumber();
    Bit* newBit = PieceForPlayer(currentPlayerIndex);
    newBit->setPosition(holder->getPosition());
    holder->setBit(newBit);
    
    // Step 4: Return true to indicate successful placement
    return true;
}

bool TicTacToe::canBitMoveFrom(Bit *bit, BitHolder *src) {
    // you can't move anything in tic tac toe
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst) {
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void TicTacToe::stopGame() {
    // clear out the board
    // loop through the 3x3 array and call destroyBit on each square
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            _grid[y][x].destroyBit();
        }
    }
}

//
// helper function for the winner check
//
Player* TicTacToe::ownerAt(int index) const {
    // index is 0..8, convert to x,y
    int y = index / 3;
    int x = index % 3;
    
    // if there is no bit at that location return nullptr
    Bit* bit = _grid[y][x].bit();
    if (!bit) {
        return nullptr;
    }
    
    // otherwise return the owner of the bit at that location
    return bit->getOwner();
}

Player* TicTacToe::checkForWinner() {
    // array of winning trios
    int winningCombos[8][3] = {
        {0, 1, 2},  // top row
        {3, 4, 5},  // middle row
        {6, 7, 8},  // bottom row
        {0, 3, 6},  // left column
        {1, 4, 7},  // middle column
        {2, 5, 8},  // right column
        {0, 4, 8},  // diagonal top-left to bottom-right
        {2, 4, 6}   // diagonal top-right to bottom-left
    };
    
    // check each winning combination
    for (int i = 0; i < 8; i++) {
        Player* first = ownerAt(winningCombos[i][0]);
        Player* second = ownerAt(winningCombos[i][1]);
        Player* third = ownerAt(winningCombos[i][2]);
        
        // if all three are owned by the same player (and not null), we have a winner
        if (first != nullptr && first == second && second == third) {
            return first;
        }
    }
    
    return nullptr;
}

bool TicTacToe::checkForDraw() {
    // if there's a winner, it's not a draw
    if (checkForWinner() != nullptr) {
        return false;
    }
    
    // check if board is full
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            // if any square is empty, it's not a draw yet
            if (_grid[y][x].bit() == nullptr) {
                return false;
            }
        }
    }
    
    // board is full with no winner
    return true;
}

//
// state strings
//
std::string TicTacToe::initialStateString() {
    return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString() const {
    std::string currentState = "";
    
    // for each bit in the grid
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            Bit* bit = _grid[y][x].bit();
            
            // if bit exists
            if (bit != nullptr) {
                // get the owner's player number and add 1 (to make it 1 or 2)
                int playerNum = bit->getOwner()->playerNumber() + 1;
                currentState += std::to_string(playerNum);
            } else {
                // empty square
                currentState += "0";
            }
        }
    }
    
    return currentState;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void TicTacToe::setStateString(const std::string &s) {
    // loop through the string and set each square
    for (int index = 0; index < 9; index++) {
        char c = s[index];
        int playerNumber = c - '0';
        
        // convert index to x,y coordinates
        int y = index / 3;
        int x = index % 3;
        
        if (playerNumber == 1) {
            // create piece for player 0 (X)
            Bit* piece = PieceForPlayer(0);
            piece->setPosition(_grid[y][x].getPosition());
            _grid[y][x].setBit(piece);
        } else if (playerNumber == 2) {
            // create piece for player 1 (O)
            Bit* piece = PieceForPlayer(1);
            piece->setPosition(_grid[y][x].getPosition());
            _grid[y][x].setBit(piece);
        } else {
            // playerNumber is 0, so leave empty
            _grid[y][x].setBit(nullptr);
        }
    }
}

//
// this is the function that will be called by the AI
//
void TicTacToe::updateAI() {
    // we will implement the AI in the next assignment!
    // meep meep im a bot
}