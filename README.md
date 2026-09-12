# ZakariaChess

A chess engine written in C++ as a learning project.

The project is being built step by step. Each development version adds a real part of the engine and keeps the previous work working.

## Development Progress

- [x] Board representation
- [x] Legal move generation
- [x] Castling / En passant / Promotion
- [x] Perft validation
- [ ] Alpha beta search
- [ ] UCI protocol
- [ ] Graphical interface
- [ ] Move animations
- [ ] Transposition table
- [ ] Iterative deepening
- [ ] Time management
- [ ] Opening book

## Version 4

Version 4 focuses on **correct chess move generation**. The board can now generate legal moves, apply and undo them, handle the special chess rules, and validate the move generator with perft.

### Implemented

- 64 square board representation
- FEN loading and export
- Pawn, knight, bishop, rook, queen and king movement
- Capture generation
- Check detection
- Legal move filtering
- Make / unmake move
- Castling on both sides
- En passant
- Pawn promotion to queen, rook, bishop or knight
- Castling right updates
- Halfmove and fullmove counters
- UCI style move strings such as `e2e4` and `a7a8q`
- Perft move generation validation

The engine does **not** search for the best move yet. Search is the next major stage.

## Build

```bash
make
./zakaria_chess
```

The program prints the starting board and all 20 legal opening moves.

## Perft

Perft recursively counts all legal move sequences to a given depth. It is useful for finding bugs in chess move generation before adding the AI search.

```bash
./zakaria_chess --perft 1
./zakaria_chess --perft 2
./zakaria_chess --perft 3
./zakaria_chess --perft 4
```

Expected results from the standard starting position:

| Depth | Nodes |
|------:|------:|
| 1 | 20 |
| 2 | 400 |
| 3 | 8,902 |
| 4 | 197,281 |

Run the included test script:

```bash
./tests/perft.sh
```

Expected output:

```text
[OK] depth 1 = 20
[OK] depth 2 = 400
[OK] depth 3 = 8902
[OK] depth 4 = 197281
All starting position perft tests passed.
```

## Project Structure

```text
.
├── include/
│   └── chess.hpp
├── src/
│   ├── chess.cpp
│   └── main.cpp
├── tests/
│   └── perft.sh
├── Makefile
├── .gitignore
└── README.md
```

## Next Step

The next development version will start the engine's decision making layer by adding position evaluation and search.
