#include <iostream>
#include <cctype>
#include <random>
#include <ctime>
#include <algorithm>

static const size_t BOARD_SIZE = 20;
static const size_t FREE_SPACE_SIZE = 8;
static const double BOARD_FILL_FACTOR = 0.7;
static const unsigned TILES_TYPE_COUNT = 20;
static const char EMPTY_TILE = ' ';
static const char DEFAULT_TILES[TILES_TYPE_COUNT] = {
	'%', '&', '?', '*', '#', '^', '+', '=', '-', '/', '\\', '!', '_', '@', '$', '<', '>', '|', '~', ':'
};

//Unicode doesn't work :(
//'%', '&', '?', '*', '#', '⌺', '⌻', '⌹', '⍔', '⍍', '⍟', '⍰', '@', '$', '⌘', '⌗', 'ↂ', 'ↀ', '∰', '∬'

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

    if (count > TILES_TYPE_COUNT) {
        throw std::exception("Invalid count for tile types! Must be <= 20.");
    }

    for (unsigned i = 0; i < count; ++i) {
        tiles[i] = DEFAULT_TILES[i];
    }
}

void shuffleMatrix(char matrix[BOARD_SIZE][BOARD_SIZE]) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (size_t row = 0; row < BOARD_SIZE; ++row) {
        for (size_t col = 0; col < BOARD_SIZE; ++col) {
            const size_t swapRow = rand() % BOARD_SIZE, swapCol = rand() % BOARD_SIZE;
            std::swap(matrix[row][col], matrix[swapRow][swapCol]);
        }
    }
}

void initBoard(char board[BOARD_SIZE][BOARD_SIZE], const char* tiles, const unsigned tileTypes) {
	if (tileTypes > 20) {
        throw std::exception("Invalid count for tile types! Must be <= 20.");
	}

    unsigned tilesFilled = 0;
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    //fills boards with random amount of random tiles
    for (unsigned i = 0; i < tileTypes; ++i) {
        const unsigned count = (1 + std::rand() % (BOARD_SIZE * BOARD_SIZE / tileTypes / 3)) * 3;

        for (unsigned j = 0; j < count; ++j) {
            board[tilesFilled / BOARD_SIZE][tilesFilled % BOARD_SIZE] = tiles[i];
            ++tilesFilled;
        }
    }

    //makes board at least 70% full
    while (tilesFilled < BOARD_SIZE * BOARD_SIZE * BOARD_FILL_FACTOR) {
        board[tilesFilled / BOARD_SIZE][tilesFilled % BOARD_SIZE] = tiles[tilesFilled / 3 % BOARD_SIZE];
        board[(tilesFilled + 1) / BOARD_SIZE][(tilesFilled + 1) % BOARD_SIZE] = tiles[tilesFilled / 3 % BOARD_SIZE];
        board[(tilesFilled + 2) / BOARD_SIZE][(tilesFilled + 2) % BOARD_SIZE] = tiles[tilesFilled / 3 % BOARD_SIZE];
        tilesFilled += 3;
    }

    //fill the rest of the board with empty tiles
    while (tilesFilled < BOARD_SIZE * BOARD_SIZE) {
        board[tilesFilled / BOARD_SIZE][tilesFilled % BOARD_SIZE] = EMPTY_TILE;
        ++tilesFilled;
    }

    shuffleMatrix(board);
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

    //can be used a static array too
    char* tiles = new char[tileTypes];
    initTiles(tiles, tileTypes, choice == 'y');

    initBoard(board, tiles, tileTypes);
    delete[] tiles;
}

void printGame(char board[BOARD_SIZE][BOARD_SIZE]) {
	for (size_t row = 0; row < BOARD_SIZE; ++row) {
        for (size_t col = 0; col < BOARD_SIZE; ++col) {
            std::cout << board[row][col] << " ";
        }

        std::cout << std::endl;
	}
}

int main() {
    char board[BOARD_SIZE][BOARD_SIZE];
    initGame(board);
    printGame(board);
    return 0;
}
