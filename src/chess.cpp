#include "chess.hpp"

#include <cctype>
#include <sstream>

namespace chess {

namespace {

char pieceToChar(int piece)
{
    switch (piece) {
        case WP: return 'P';
        case WN: return 'N';
        case WB: return 'B';
        case WR: return 'R';
        case WQ: return 'Q';
        case WK: return 'K';
        case BP: return 'p';
        case BN: return 'n';
        case BB: return 'b';
        case BR: return 'r';
        case BQ: return 'q';
        case BK: return 'k';
        default: return '.';
    }
}

int charToPiece(char c)
{
    switch (c) {
        case 'P': return WP;
        case 'N': return WN;
        case 'B': return WB;
        case 'R': return WR;
        case 'Q': return WQ;
        case 'K': return WK;
        case 'p': return BP;
        case 'n': return BN;
        case 'b': return BB;
        case 'r': return BR;
        case 'q': return BQ;
        case 'k': return BK;
        default: return EMPTY;
    }
}

} // namespace

Board::Board()
{
    setStartPosition();
}

int Board::fileOf(int square)
{
    return square % 8;
}

int Board::rankOf(int square)
{
    return square / 8;
}

Color Board::sideToMove() const
{
    return side;
}

int Board::pieceAt(int square) const
{
    if (square < 0 || square >= 64)
        return EMPTY;
    return squares[square];
}

void Board::setStartPosition()
{
    setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

bool Board::setFen(const std::string& fen)
{
    std::istringstream input(fen);
    std::string boardPart;
    std::string sidePart;
    std::string castlePart;
    std::string epPart;
    int half = 0;
    int full = 1;

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
    if (sidePart == "w")
        newSide = WHITE;
    else if (sidePart == "b")
        newSide = BLACK;
    else
        return false;

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
        if (epPart.size() != 2 || epPart[0] < 'a' || epPart[0] > 'h'
            || epPart[1] < '1' || epPart[1] > '8')
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

std::string Board::pretty() const
{
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

std::string Board::toFen() const
{
    std::ostringstream out;

    for (int rank = 7; rank >= 0; --rank) {
        int empty = 0;

        for (int file = 0; file < 8; ++file) {
            int piece = squares[rank * 8 + file];

            if (piece == EMPTY) {
                ++empty;
            } else {
                if (empty > 0) {
                    out << empty;
                    empty = 0;
                }
                out << pieceToChar(piece);
            }
        }

        if (empty > 0)
            out << empty;
        if (rank > 0)
            out << '/';
    }

    out << (side == WHITE ? " w " : " b ");

    if (castlingRights == 0) {
        out << '-';
    } else {
        if (castlingRights & 1) out << 'K';
        if (castlingRights & 2) out << 'Q';
        if (castlingRights & 4) out << 'k';
        if (castlingRights & 8) out << 'q';
    }

    out << ' ';
    if (enPassant == -1)
        out << '-';
    else
        out << static_cast<char>('a' + fileOf(enPassant))
            << static_cast<char>('1' + rankOf(enPassant));

    out << ' ' << halfmoveClock << ' ' << fullmoveNumber;
    return out.str();
}

} // namespace chess
