#include "chess.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>

namespace chess {

namespace {

const int KNIGHT_DELTAS[8][2] = {
    {1, 2}, {2, 1}, {2, -1}, {1, -2},
    {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}
};

const int KING_DELTAS[8][2] = {
    {1, 0}, {1, 1}, {0, 1}, {-1, 1},
    {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
};

const int BISHOP_DIRS[4] = {9, 7, -7, -9};
const int ROOK_DIRS[4] = {8, -8, 1, -1};
const int QUEEN_DIRS[8] = {9, 7, -7, -9, 8, -8, 1, -1};

char pieceToChar(int piece) {
    switch (piece) {
        case WP: return 'P'; case WN: return 'N'; case WB: return 'B';
        case WR: return 'R'; case WQ: return 'Q'; case WK: return 'K';
        case BP: return 'p'; case BN: return 'n'; case BB: return 'b';
        case BR: return 'r'; case BQ: return 'q'; case BK: return 'k';
        default: return '.';
    }
}

int charToPiece(char c) {
    switch (c) {
        case 'P': return WP; case 'N': return WN; case 'B': return WB;
        case 'R': return WR; case 'Q': return WQ; case 'K': return WK;
        case 'p': return BP; case 'n': return BN; case 'b': return BB;
        case 'r': return BR; case 'q': return BQ; case 'k': return BK;
        default: return EMPTY;
    }
}

int promotionCharToType(char c) {
    switch (std::tolower(static_cast<unsigned char>(c))) {
        case 'q': return 5;
        case 'r': return 4;
        case 'b': return 3;
        case 'n': return 2;
        default: return 0;
    }
}

char promotionTypeToChar(int type) {
    switch (type) {
        case 5: return 'q';
        case 4: return 'r';
        case 3: return 'b';
        case 2: return 'n';
        default: return 'q';
    }
}

} // namespace

bool Move::operator==(const Move& other) const {
    return from == other.from && to == other.to && promotion == other.promotion;
}

Board::Board() {
    setStartPosition();
}

bool Board::onBoard(int square) {
    return square >= 0 && square < 64;
}

int Board::fileOf(int square) {
    return square % 8;
}

int Board::rankOf(int square) {
    return square / 8;
}

Color Board::pieceColor(int piece) {
    return piece > 0 ? WHITE : BLACK;
}

int Board::pieceType(int piece) {
    return std::abs(piece);
}

void Board::setStartPosition() {
    setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

bool Board::setFen(const std::string& fen) {
    std::istringstream input(fen);
    std::string boardPart, sidePart, castlePart, epPart;
    int half = 0, full = 1;

    if (!(input >> boardPart >> sidePart >> castlePart >> epPart >> half >> full))
        return false;

    std::array<int, 64> newSquares{};
    int rank = 7;
    int file = 0;

    for (char c : boardPart) {
        if (c == '/') {
            if (file != 8 || rank == 0)
                return false;
            --rank;
            file = 0;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c))) {
            int emptyCount = c - '0';
            if (emptyCount < 1 || emptyCount > 8 || file + emptyCount > 8)
                return false;
            file += emptyCount;
            continue;
        }

        int piece = charToPiece(c);
        if (piece == EMPTY || file >= 8 || rank < 0)
            return false;
        newSquares[rank * 8 + file] = piece;
        ++file;
    }

    if (rank != 0 || file != 8)
        return false;

    Color newSide;
    if (sidePart == "w") newSide = WHITE;
    else if (sidePart == "b") newSide = BLACK;
    else return false;

    int rights = 0;
    if (castlePart != "-") {
        for (char c : castlePart) {
            if (c == 'K') rights |= 1;
            else if (c == 'Q') rights |= 2;
            else if (c == 'k') rights |= 4;
            else if (c == 'q') rights |= 8;
            else return false;
        }
    }

    int ep = -1;
    if (epPart != "-") {
        if (epPart.size() != 2 || epPart[0] < 'a' || epPart[0] > 'h' || epPart[1] < '1' || epPart[1] > '8')
            return false;
        ep = (epPart[1] - '1') * 8 + (epPart[0] - 'a');
    }

    squares = newSquares;
    side = newSide;
    castlingRights = rights;
    enPassant = ep;
    halfmoveClock = half;
    fullmoveNumber = full;
    return true;
}

std::string Board::toFen() const {
    std::ostringstream out;

    for (int rank = 7; rank >= 0; --rank) {
        int empty = 0;
        for (int file = 0; file < 8; ++file) {
            int piece = squares[rank * 8 + file];
            if (piece == EMPTY) {
                ++empty;
            } else {
                if (empty) {
                    out << empty;
                    empty = 0;
                }
                out << pieceToChar(piece);
            }
        }
        if (empty) out << empty;
        if (rank) out << '/';
    }

    out << (side == WHITE ? " w " : " b ");

    if (castlingRights == 0) out << '-';
    else {
        if (castlingRights & 1) out << 'K';
        if (castlingRights & 2) out << 'Q';
        if (castlingRights & 4) out << 'k';
        if (castlingRights & 8) out << 'q';
    }

    out << ' ';
    if (enPassant == -1) out << '-';
    else out << static_cast<char>('a' + fileOf(enPassant)) << static_cast<char>('1' + rankOf(enPassant));

    out << ' ' << halfmoveClock << ' ' << fullmoveNumber;
    return out.str();
}

std::string Board::pretty() const {
    std::ostringstream out;
    for (int rank = 7; rank >= 0; --rank) {
        out << rank + 1 << "  ";
        for (int file = 0; file < 8; ++file)
            out << pieceToChar(squares[rank * 8 + file]) << ' ';
        out << '\n';
    }
    out << "\n   a b c d e f g h\n";
    out << "side: " << (side == WHITE ? "white" : "black") << '\n';
    return out.str();
}

void Board::addPromotionMoves(std::vector<Move>& moves, int from, int to, int flags) const {
    for (int type : {5, 4, 3, 2})
        moves.push_back({from, to, type, flags | PROMOTION});
}

void Board::addPawnMoves(std::vector<Move>& moves, int square) const {
    int piece = squares[square];
    Color color = pieceColor(piece);
    int dir = color == WHITE ? 8 : -8;
    int startRank = color == WHITE ? 1 : 6;
    int promotionRank = color == WHITE ? 6 : 1;
    int rank = rankOf(square);
    int file = fileOf(square);

    int one = square + dir;
    if (onBoard(one) && squares[one] == EMPTY) {
        if (rank == promotionRank)
            addPromotionMoves(moves, square, one, QUIET);
        else
            moves.push_back({square, one, 0, QUIET});

        int two = square + dir * 2;
        if (rank == startRank && squares[two] == EMPTY)
            moves.push_back({square, two, 0, DOUBLE_PAWN});
    }

    for (int df : {-1, 1}) {
        int targetFile = file + df;
        if (targetFile < 0 || targetFile > 7)
            continue;

        int target = square + dir + df;
        if (!onBoard(target))
            continue;

        if (squares[target] != EMPTY && pieceColor(squares[target]) != color) {
            if (rank == promotionRank)
                addPromotionMoves(moves, square, target, CAPTURE);
            else
                moves.push_back({square, target, 0, CAPTURE});
        }

        if (target == enPassant)
            moves.push_back({square, target, 0, CAPTURE | EN_PASSANT});
    }
}

void Board::addKnightMoves(std::vector<Move>& moves, int square) const {
    Color color = pieceColor(squares[square]);
    int file = fileOf(square);
    int rank = rankOf(square);

    for (const auto& d : KNIGHT_DELTAS) {
        int nf = file + d[0];
        int nr = rank + d[1];
        if (nf < 0 || nf > 7 || nr < 0 || nr > 7)
            continue;

        int to = nr * 8 + nf;
        if (squares[to] == EMPTY)
            moves.push_back({square, to, 0, QUIET});
        else if (pieceColor(squares[to]) != color)
            moves.push_back({square, to, 0, CAPTURE});
    }
}

void Board::addSlidingMoves(std::vector<Move>& moves, int square, const int* directions, int count) const {
    Color color = pieceColor(squares[square]);

    for (int i = 0; i < count; ++i) {
        int dir = directions[i];
        int current = square;

        while (true) {
            int next = current + dir;
            if (!onBoard(next))
                break;

            int fileDiff = std::abs(fileOf(next) - fileOf(current));
            if ((dir == 1 || dir == -1 || dir == 7 || dir == -7 || dir == 9 || dir == -9) && fileDiff != 1)
                break;

            if (squares[next] == EMPTY) {
                moves.push_back({square, next, 0, QUIET});
            } else {
                if (pieceColor(squares[next]) != color)
                    moves.push_back({square, next, 0, CAPTURE});
                break;
            }

            current = next;
        }
    }
}

void Board::addKingMoves(std::vector<Move>& moves, int square) const {
    Color color = pieceColor(squares[square]);
    int file = fileOf(square);
    int rank = rankOf(square);

    for (const auto& d : KING_DELTAS) {
        int nf = file + d[0];
        int nr = rank + d[1];
        if (nf < 0 || nf > 7 || nr < 0 || nr > 7)
            continue;

        int to = nr * 8 + nf;
        if (squares[to] == EMPTY)
            moves.push_back({square, to, 0, QUIET});
        else if (pieceColor(squares[to]) != color)
            moves.push_back({square, to, 0, CAPTURE});
    }

    if (color == WHITE && square == 4) {
        if ((castlingRights & 1) && squares[5] == EMPTY && squares[6] == EMPTY && squares[7] == WR &&
            !isSquareAttacked(4, BLACK) && !isSquareAttacked(5, BLACK) && !isSquareAttacked(6, BLACK))
            moves.push_back({4, 6, 0, CASTLE});

        if ((castlingRights & 2) && squares[1] == EMPTY && squares[2] == EMPTY && squares[3] == EMPTY && squares[0] == WR &&
            !isSquareAttacked(4, BLACK) && !isSquareAttacked(3, BLACK) && !isSquareAttacked(2, BLACK))
            moves.push_back({4, 2, 0, CASTLE});
    }

    if (color == BLACK && square == 60) {
        if ((castlingRights & 4) && squares[61] == EMPTY && squares[62] == EMPTY && squares[63] == BR &&
            !isSquareAttacked(60, WHITE) && !isSquareAttacked(61, WHITE) && !isSquareAttacked(62, WHITE))
            moves.push_back({60, 62, 0, CASTLE});

        if ((castlingRights & 8) && squares[57] == EMPTY && squares[58] == EMPTY && squares[59] == EMPTY && squares[56] == BR &&
            !isSquareAttacked(60, WHITE) && !isSquareAttacked(59, WHITE) && !isSquareAttacked(58, WHITE))
            moves.push_back({60, 58, 0, CASTLE});
    }
}

std::vector<Move> Board::generatePseudoLegalMoves() const {
    std::vector<Move> moves;
    moves.reserve(64);

    for (int square = 0; square < 64; ++square) {
        int piece = squares[square];
        if (piece == EMPTY || pieceColor(piece) != side)
            continue;

        switch (pieceType(piece)) {
            case 1: addPawnMoves(moves, square); break;
            case 2: addKnightMoves(moves, square); break;
            case 3: addSlidingMoves(moves, square, BISHOP_DIRS, 4); break;
            case 4: addSlidingMoves(moves, square, ROOK_DIRS, 4); break;
            case 5: addSlidingMoves(moves, square, QUEEN_DIRS, 8); break;
            case 6: addKingMoves(moves, square); break;
        }
    }

    return moves;
}

int Board::kingSquare(Color color) const {
    int king = color == WHITE ? WK : BK;
    for (int i = 0; i < 64; ++i)
        if (squares[i] == king)
            return i;
    return -1;
}

bool Board::isSquareAttacked(int square, Color byColor) const {
    int file = fileOf(square);
    int rank = rankOf(square);

    int pawn = byColor == WHITE ? WP : BP;
    int pawnRank = rank + (byColor == WHITE ? -1 : 1);
    if (pawnRank >= 0 && pawnRank < 8) {
        for (int df : {-1, 1}) {
            int pf = file + df;
            if (pf >= 0 && pf < 8 && squares[pawnRank * 8 + pf] == pawn)
                return true;
        }
    }

    int knight = byColor == WHITE ? WN : BN;
    for (const auto& d : KNIGHT_DELTAS) {
        int nf = file + d[0];
        int nr = rank + d[1];
        if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8 && squares[nr * 8 + nf] == knight)
            return true;
    }

    int king = byColor == WHITE ? WK : BK;
    for (const auto& d : KING_DELTAS) {
        int nf = file + d[0];
        int nr = rank + d[1];
        if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8 && squares[nr * 8 + nf] == king)
            return true;
    }

    auto attackedBySlider = [&](const int* dirs, int count, int bishopOrRookType) {
        for (int i = 0; i < count; ++i) {
            int current = square;
            int dir = dirs[i];
            while (true) {
                int next = current + dir;
                if (!onBoard(next)) break;
                int fileDiff = std::abs(fileOf(next) - fileOf(current));
                if ((dir == 1 || dir == -1 || dir == 7 || dir == -7 || dir == 9 || dir == -9) && fileDiff != 1)
                    break;

                int piece = squares[next];
                if (piece != EMPTY) {
                    if (pieceColor(piece) == byColor) {
                        int type = pieceType(piece);
                        if (type == bishopOrRookType || type == 5)
                            return true;
                    }
                    break;
                }
                current = next;
            }
        }
        return false;
    };

    if (attackedBySlider(BISHOP_DIRS, 4, 3)) return true;
    if (attackedBySlider(ROOK_DIRS, 4, 4)) return true;
    return false;
}

bool Board::inCheck(Color color) const {
    int king = kingSquare(color);
    if (king == -1)
        return true;
    return isSquareAttacked(king, color == WHITE ? BLACK : WHITE);
}

void Board::updateCastlingRights(int from, int to, int movedPiece, int capturedPiece) {
    if (movedPiece == WK) castlingRights &= ~(1 | 2);
    if (movedPiece == BK) castlingRights &= ~(4 | 8);

    if (from == 0 || to == 0) castlingRights &= ~2;
    if (from == 7 || to == 7) castlingRights &= ~1;
    if (from == 56 || to == 56) castlingRights &= ~8;
    if (from == 63 || to == 63) castlingRights &= ~4;

    if (capturedPiece == WR) {
        if (to == 0) castlingRights &= ~2;
        if (to == 7) castlingRights &= ~1;
    }
    if (capturedPiece == BR) {
        if (to == 56) castlingRights &= ~8;
        if (to == 63) castlingRights &= ~4;
    }
}

bool Board::makeMove(const Move& move, Undo& undo) {
    if (!onBoard(move.from) || !onBoard(move.to))
        return false;

    int movedPiece = squares[move.from];
    if (movedPiece == EMPTY || pieceColor(movedPiece) != side)
        return false;

    undo.captured = squares[move.to];
    undo.castlingRights = castlingRights;
    undo.enPassant = enPassant;
    undo.halfmoveClock = halfmoveClock;
    undo.fullmoveNumber = fullmoveNumber;

    if (move.flags & EN_PASSANT) {
        int capturedSquare = move.to + (side == WHITE ? -8 : 8);
        undo.captured = squares[capturedSquare];
        squares[capturedSquare] = EMPTY;
    }

    squares[move.to] = movedPiece;
    squares[move.from] = EMPTY;

    if (move.flags & PROMOTION)
        squares[move.to] = side == WHITE ? move.promotion : -move.promotion;

    if (move.flags & CASTLE) {
        if (move.to == 6) {
            squares[5] = WR;
            squares[7] = EMPTY;
        } else if (move.to == 2) {
            squares[3] = WR;
            squares[0] = EMPTY;
        } else if (move.to == 62) {
            squares[61] = BR;
            squares[63] = EMPTY;
        } else if (move.to == 58) {
            squares[59] = BR;
            squares[56] = EMPTY;
        }
    }

    updateCastlingRights(move.from, move.to, movedPiece, undo.captured);

    enPassant = -1;
    if (move.flags & DOUBLE_PAWN)
        enPassant = move.from + (side == WHITE ? 8 : -8);

    if (pieceType(movedPiece) == 1 || undo.captured != EMPTY)
        halfmoveClock = 0;
    else
        ++halfmoveClock;

    Color movedSide = side;
    if (side == BLACK)
        ++fullmoveNumber;
    side = side == WHITE ? BLACK : WHITE;

    if (inCheck(movedSide)) {
        unmakeMove(move, undo);
        return false;
    }

    return true;
}

void Board::unmakeMove(const Move& move, const Undo& undo) {
    side = side == WHITE ? BLACK : WHITE;
    castlingRights = undo.castlingRights;
    enPassant = undo.enPassant;
    halfmoveClock = undo.halfmoveClock;
    fullmoveNumber = undo.fullmoveNumber;

    int movedPiece = squares[move.to];
    if (move.flags & PROMOTION)
        movedPiece = side == WHITE ? WP : BP;

    squares[move.from] = movedPiece;
    squares[move.to] = undo.captured;

    if (move.flags & EN_PASSANT) {
        squares[move.to] = EMPTY;
        int capturedSquare = move.to + (side == WHITE ? -8 : 8);
        squares[capturedSquare] = undo.captured;
    }

    if (move.flags & CASTLE) {
        if (move.to == 6) {
            squares[7] = WR;
            squares[5] = EMPTY;
        } else if (move.to == 2) {
            squares[0] = WR;
            squares[3] = EMPTY;
        } else if (move.to == 62) {
            squares[63] = BR;
            squares[61] = EMPTY;
        } else if (move.to == 58) {
            squares[56] = BR;
            squares[59] = EMPTY;
        }
    }
}

std::vector<Move> Board::generateLegalMoves() {
    std::vector<Move> legal;
    auto pseudo = generatePseudoLegalMoves();
    legal.reserve(pseudo.size());

    for (const Move& move : pseudo) {
        Undo undo;
        if (makeMove(move, undo)) {
            legal.push_back(move);
            unmakeMove(move, undo);
        }
    }

    return legal;
}

Move Board::parseUciMove(const std::string& text) {
    if (text.size() < 4)
        return {};

    int ff = text[0] - 'a';
    int fr = text[1] - '1';
    int tf = text[2] - 'a';
    int tr = text[3] - '1';

    if (ff < 0 || ff > 7 || fr < 0 || fr > 7 || tf < 0 || tf > 7 || tr < 0 || tr > 7)
        return {};

    int from = fr * 8 + ff;
    int to = tr * 8 + tf;
    int promotion = text.size() >= 5 ? promotionCharToType(text[4]) : 0;

    auto legal = generateLegalMoves();
    for (const Move& move : legal) {
        if (move.from == from && move.to == to && move.promotion == promotion)
            return move;
    }

    return {};
}

std::string Board::moveToUci(const Move& move) {
    if (move.from < 0 || move.to < 0)
        return "0000";

    std::string out;
    out += static_cast<char>('a' + fileOf(move.from));
    out += static_cast<char>('1' + rankOf(move.from));
    out += static_cast<char>('a' + fileOf(move.to));
    out += static_cast<char>('1' + rankOf(move.to));
    if (move.promotion)
        out += promotionTypeToChar(move.promotion);
    return out;
}

std::uint64_t Board::perft(int depth) {
    if (depth == 0)
        return 1;

    auto moves = generateLegalMoves();
    if (depth == 1)
        return moves.size();

    std::uint64_t nodes = 0;
    for (const Move& move : moves) {
        Undo undo;
        if (!makeMove(move, undo))
            continue;
        nodes += perft(depth - 1);
        unmakeMove(move, undo);
    }
    return nodes;
}

} // namespace chess
