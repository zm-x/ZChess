#include "chess.hpp"

#include <iostream>

int main()
{
    chess::Board board;

    std::cout << "ZakariaChess - development version 1\n\n";
    std::cout << board.pretty();
    std::cout << "\nFEN: " << board.toFen() << '\n';

    return 0;
}
