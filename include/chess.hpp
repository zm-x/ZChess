#ifndef CHESS_HPP
#define CHESS_HPP

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace chess {

enum Color { WHITE = 0, BLACK = 1 };
enum Piece {
    EMPTY = 0,
    WP = 1, WN, WB, WR, WQ, WK,
    BP = -1, BN = -2, BB = -3, BR = -4, BQ = -5, BK = -6
};

enum MoveFlag {
    QUIET = 0,
    CAPTURE = 1 << 0,
    DOUBLE_PAWN = 1 << 1,
    EN_PASSANT = 1 << 2,
    CASTLE = 1 << 3,
    PROMOTION = 1 << 4
};

struct Move {
    int from = -1;
    int to = -1;
    int promotion = 0;
    int flags = QUIET;

    bool operator==(const Move& other) const;
};

struct Undo {
    int captured = EMPTY;
    int castlingRights = 0;
    int enPassant = -1;
    int halfmoveClock = 0;
    int fullmoveNumber = 1;
};

class Board {
public:
    Board();

    bool setFen(const std::string& fen);
    void setStartPosition();

    std::string toFen() const;
    std::string pretty() const;

    std::vector<Move> generateLegalMoves();
    std::vector<Move> generatePseudoLegalMoves() const;

    bool makeMove(const Move& move, Undo& undo);
    void unmakeMove(const Move& move, const Undo& undo);

    bool isSquareAttacked(int square, Color byColor) const;
    bool inCheck(Color color) const;
    int kingSquare(Color color) const;

    Color sideToMove() const { return side; }
    int pieceAt(int square) const { return squares[square]; }
    int enPassantSquare() const { return enPassant; }
    int castling() const { return castlingRights; }

    Move parseUciMove(const std::string& text);
    static std::string moveToUci(const Move& move);

    std::uint64_t perft(int depth);

private:
    std::array<int, 64> squares{};
    Color side = WHITE;
    int castlingRights = 0;
    int enPassant = -1;
    int halfmoveClock = 0;
    int fullmoveNumber = 1;

    static bool onBoard(int square);
    static int fileOf(int square);
    static int rankOf(int square);
    static Color pieceColor(int piece);
    static int pieceType(int piece);

    void addPawnMoves(std::vector<Move>& moves, int square) const;
    void addKnightMoves(std::vector<Move>& moves, int square) const;
    void addSlidingMoves(std::vector<Move>& moves, int square, const int* directions, int count) const;
    void addKingMoves(std::vector<Move>& moves, int square) const;
    void addPromotionMoves(std::vector<Move>& moves, int from, int to, int flags) const;

    void updateCastlingRights(int from, int to, int movedPiece, int capturedPiece);
};


} // namespace chess

#endif
