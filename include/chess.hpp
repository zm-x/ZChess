#ifndef CHESS_HPP
#define CHESS_HPP

#include <array>
#include <string>

namespace chess {

enum Color {
    WHITE = 0,
    BLACK = 1
};

enum Piece {
    EMPTY = 0,
    WP = 1, WN, WB, WR, WQ, WK,
    BP = -1, BN = -2, BB = -3, BR = -4, BQ = -5, BK = -6
};

class Board {
public:
    Board();

    void setStartPosition();
    bool setFen(const std::string& fen);

    std::string pretty() const;
    std::string toFen() const;

    Color sideToMove() const;
    int pieceAt(int square) const;

private:
    std::array<int, 64> squares;
    Color side;
    int castlingRights;
    int enPassant;
    int halfmoveClock;
    int fullmoveNumber;

    static int fileOf(int square);
    static int rankOf(int square);
};

} // namespace chess

#endif
