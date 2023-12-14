﻿#include <iostream>
#include <cctype>
#include <random>
#include <ctime>

static const size_t BOARD_SIZE = 20;
static const size_t FREE_SPACE_SIZE = 8;
static const unsigned TILES_TYPE_COUNT = 20;
static const char DEFAULT_TILES[TILES_TYPE_COUNT] = { '%', '&', '?', '*', '#', '⌺', '⌻', '⌹', '⍔', '⍍', '⍟', '⍰', '@', '$', '⌘', '⌗', 'ↂ', 'ↀ', '∰', '∬' };

bool contains(const char* arr, const size_t size, const char c) {
	for (size_t i = 0; i < size; ++i) {
		if (arr[i] == c) {
            return true;
		}
	}

    return false;
}

void initTiles(char* tiles, const unsigned count, const bool useCustomTiles) {
	if (useCustomTiles) {
		for (unsigned i = 0; i < count; ++i) {
            std::cout << i + 1 << ") Enter a tile symbol: ";
            std::cin >> tiles[i];

            while (i > 0 && contains(tiles, i - 1, tiles[i])) {
                std::cout << "You have already entered this symbol. Please enter a new one: ";
                std::cin >> tiles[i];
            }
		}

        return;
	}

    if (count > 20) {
        throw std::exception("Invalid count for tile types! Must be <= 20.");
    }

    for (unsigned i = 0; i < count; ++i) {
        tiles[i] = DEFAULT_TILES[i];
    }
}

void initBoard(char board[BOARD_SIZE][BOARD_SIZE], const char* tiles, const unsigned tileTypes) {
	if (tileTypes > 20) {
        throw std::exception("Invalid count for tile types! Must be <= 20.");
	}
 
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void initGame(char board[BOARD_SIZE][BOARD_SIZE]) {
    int tileTypes;
    std::cout << "Enter the count of different tile types (must be in [8, 20]): ";
    std::cin >> tileTypes;

    while (tileTypes < 8 || tileTypes > 20) {
        std::cout << "Invalid input! Must be in [8, 20]. Please try again: ";
        std::cin >> tileTypes;
    }

    char choice;
    std::cout << "Do you wish to enter your own symbols for the tiles? (y/n) ";
    std::cin >> choice;
    choice = static_cast<char>(std::tolower(choice));

    while (choice != 'y' && choice != 'n') {
        std::cout << "Invalid input! Please try again (y/n): ";
        std::cin >> choice;
        choice = static_cast<char>(std::tolower(choice));
    }

    char* tiles = new char[tileTypes];
    initTiles(tiles, tileTypes, choice == 'y');

    initBoard(board, tiles, tileTypes);
    delete[] tiles;
}

int main() {
    char board[BOARD_SIZE][BOARD_SIZE];
    initGame(board);
    return 0;
}