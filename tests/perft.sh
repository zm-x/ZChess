#!/usr/bin/env bash
set -e

ENGINE="${1:-./ZChess}"

check() {
    depth="$1"
    expected="$2"
    got="$($ENGINE --perft "$depth")"

    if [ "$got" != "$expected" ]; then
        echo "[FAIL] depth $depth: expected $expected, got $got"
        exit 1
    fi
    echo "[OK] depth $depth = $got"
}

check 1 20
check 2 400
check 3 8902
check 4 197281

echo "All starting-position perft tests passed."
