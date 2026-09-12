#include "chess.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    chess::Board board;

    if (argc == 3 && std::string(argv[1]) == "--perft")
    {
        int depth = std::atoi(argv[2]);
        if (depth < 0)
        {
            std::cerr << "depth must be positive\n";
            return 1;
        }
        std::cout << board.perft(depth) << '\n';
        return 0;
    }

    std::cout << "ZakariaChess - development version 4\n\n";
    std::cout << board.pretty();

    std::vector<chess::Move> moves = board.generateLegalMoves();
    std::cout << "\nLegal moves from the starting position: " << moves.size() << "\n";
    std::cout << "Moves: ";
    for (std::size_t i = 0; i < moves.size(); ++i)
    {
        std::cout << chess::Board::moveToUci(moves[i]);
        if (i + 1 != moves.size())
            std::cout << ' ';
    }
    std::cout << "\n\nRun ./zakaria_chess --perft <depth> to test move generation.\n";
    return 0;
}
