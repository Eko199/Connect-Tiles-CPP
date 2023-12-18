#include <iostream>
#include <cctype>
#include <random>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <windows.h>

static const size_t BOARD_SIZE = 20;
static const size_t FREE_SPACE_SIZE = 8;
static const double BOARD_FILL_FACTOR = 0.75;
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
    const unsigned maxTilesCountOfTypeDivide3 = BOARD_SIZE * BOARD_SIZE / tileTypes / 3;

    for (unsigned i = 0; i < tileTypes; ++i) {
        const unsigned count = 3 + std::rand() % maxTilesCountOfTypeDivide3 * 3;

        for (unsigned j = 0; j < count; ++j) {
            board[tilesFilled / BOARD_SIZE][tilesFilled % BOARD_SIZE] = tiles[i];
            ++tilesFilled;
        }
    }

    //makes board at least 75% full
    while (tilesFilled < BOARD_SIZE * BOARD_SIZE * BOARD_FILL_FACTOR) {
        board[tilesFilled / BOARD_SIZE][tilesFilled % BOARD_SIZE] = tiles[tilesFilled / 3 % tileTypes];
        board[(tilesFilled + 1) / BOARD_SIZE][(tilesFilled + 1) % BOARD_SIZE] = tiles[tilesFilled / 3 % tileTypes];
        board[(tilesFilled + 2) / BOARD_SIZE][(tilesFilled + 2) % BOARD_SIZE] = tiles[tilesFilled / 3 % tileTypes];
        tilesFilled += 3;
    }

    //fill the rest of the board with empty tiles
    while (tilesFilled < BOARD_SIZE * BOARD_SIZE) {
        board[tilesFilled / BOARD_SIZE][tilesFilled % BOARD_SIZE] = EMPTY_TILE;
        ++tilesFilled;
    }

    shuffleMatrix(board);
}

void initGame(char board[BOARD_SIZE][BOARD_SIZE], char freeSpace[FREE_SPACE_SIZE]) {
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

    //initialize the free space
    for (size_t i = 0; i < FREE_SPACE_SIZE; ++i) {
        freeSpace[i] = ' ';
    }
}

void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    std::cout << std::setw(2) << "   ";

    for (size_t i = 1; i <= BOARD_SIZE; ++i) {
        std::cout << std::setw(2) << i << " ";
    }

    std::cout << std::endl;

    for (size_t row = 0; row < BOARD_SIZE; ++row) {
        std::cout << std::setw(2) << row + 1 << " ";

        for (size_t col = 0; col < BOARD_SIZE; ++col) {
            std::cout << std::setw(2) << board[row][col] << " ";
        }

        std::cout << std::endl;
    }
}

void printGame(char board[BOARD_SIZE][BOARD_SIZE], char freeSpace[FREE_SPACE_SIZE]) {
    std::cout << "Current board:" << std::endl << std::endl;
    printBoard(board);

    std::cout << std::endl << "Free space: " << std::endl;

    const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);

    for (size_t i = 0; i < FREE_SPACE_SIZE; ++i) {
        std::cout << std::setw(2) << freeSpace[i];
    }

    SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    std::cout << std::endl;
}

bool areValidCoordinates(const size_t row, const size_t col) {
    return row < BOARD_SIZE && col < BOARD_SIZE;
}

void getCoordinatesChoice(size_t& row, size_t& col) {
    std::cout << "Enter the x and y of the tile you want to get: ";
    std::cin >> col >> row;

    while (!areValidCoordinates(--row, --col)) {
        std::cout << "Invalid coordinates! Must be between 0 and " << BOARD_SIZE << ". Please try again: ";
        std::cin >> col >> row;
    }
}

void checkFreeSpaceForTriple(char freeSpace[FREE_SPACE_SIZE], const char newTile) {
    unsigned count = 0;

    for (size_t i = 0; i < FREE_SPACE_SIZE; ++i) {
        count += freeSpace[i] == newTile;
    }

    if (count < 3) {
	    return;
    }

    //removed the matching tiles
    unsigned currI = 0;
    for (size_t i = 0; i < FREE_SPACE_SIZE; ++i) {
        if (freeSpace[i] == newTile) {
	        continue;
        }

        freeSpace[currI++] = freeSpace[i];
    }

    while (currI < FREE_SPACE_SIZE) {
        freeSpace[currI++] = ' ';
    }

    std::cout << "Successfully connected " << newTile << "!" << std::endl;
}

void playTurn(char board[BOARD_SIZE][BOARD_SIZE], char freeSpace[FREE_SPACE_SIZE], const size_t row, const size_t col) {
    size_t freeSpaceTiles = 0;

	for (size_t i = 0; i < FREE_SPACE_SIZE; ++i) {
		if (freeSpace[i] == ' ') {
            break;
		}

        ++freeSpaceTiles;
	}

    if (freeSpaceTiles == FREE_SPACE_SIZE) {
        throw std::exception("Free space is full!");
    }

    freeSpace[freeSpaceTiles] = board[row][col];
    board[row][col] = ' ';

    if (freeSpace[freeSpaceTiles] != ' ') {
	    checkFreeSpaceForTriple(freeSpace, freeSpace[freeSpaceTiles]);
    }
}

int main() {
    char board[BOARD_SIZE][BOARD_SIZE], freeSpace[FREE_SPACE_SIZE];
    initGame(board, freeSpace);

    while (true) {
	    printGame(board, freeSpace);

        size_t tileRow, tileCol;
        getCoordinatesChoice(tileRow, tileCol);

        try {
	        playTurn(board, freeSpace, tileRow, tileCol);
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            continue;
        }
    }

    return 0;
}
