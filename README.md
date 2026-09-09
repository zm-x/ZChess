# ZakariaChess

A chess engine written in C++ as a learning project.

This repository is being built step by step. Each development version adds a real part of the engine instead of starting from the final code.

## Development Progress

- [x] Board representation
- [ ] Legal move generation
- [ ] Castling / En passant / Promotion
- [ ] Perft validation
- [ ] Alpha-beta search
- [ ] UCI protocol
- [ ] Graphical interface
- [ ] Move animations
- [ ] Transposition table
- [ ] Iterative deepening
- [ ] Time management
- [ ] Opening book

## Version 1

The first version contains only the chess board representation and FEN parsing.

Implemented:
- 64-square board
- Piece representation
- Side to move
- Castling-right state
- En-passant square state
- Halfmove/fullmove counters
- Starting position
- FEN loading and export
- Terminal board display

No chess moves are generated yet.

## Build

```bash
make
./zakaria_chess
```

Example output:

```text
8  r n b q k b n r
7  p p p p p p p p
6  . . . . . . . .
5  . . . . . . . .
4  . . . . . . . .
3  . . . . . . . .
2  P P P P P P P P
1  R N B Q K B N R

   a b c d e f g h
side: white
```
