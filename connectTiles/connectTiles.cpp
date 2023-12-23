#include <iostream>
#include <cctype>
#include <random>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <windows.h>

static const size_t STR_SIZE = 1000;
static const size_t BOARD_SIZE = 5;
static const size_t MAX_LAYERS = 10;
static const size_t FREE_SPACE_SIZE = 8;
static const double BOARD_FILL_FACTOR = 0.75;
static const unsigned TILES_TYPE_COUNT = 20;
static const char EMPTY_TILE = ' ';
static const char DEFAULT_TILES[TILES_TYPE_COUNT] = {
	'%', '&', '?', '*', '#', '^', '+', '=', '-', '/', '\\', '!', '_', '@', '$', '<', '>', '|', '~', ':'
};
//Unicode doesn't work :(
//'%', '&', '?', '*', '#', '⌺', '⌻', '⌹', '⍔', '⍍', '⍟', '⍰', '@', '$', '⌘', '⌗', 'ↂ', 'ↀ', '∰', '∬'
static const byte LAYER_COLORS[MAX_LAYERS] = {
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN,
	FOREGROUND_BLUE | FOREGROUND_INTENSITY, 
	FOREGROUND_BLUE,
	FOREGROUND_BLUE | FOREGROUND_GREEN,
    FOREGROUND_BLUE | FOREGROUND_RED,
    FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_GREEN | FOREGROUND_RED,
    FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_RED
};

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

void shuffleMatrix(char matrix[MAX_LAYERS][BOARD_SIZE][BOARD_SIZE], const size_t size) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (size_t layer = 0; layer < size; ++layer) {
	    for (size_t row = 0; row < BOARD_SIZE; ++row) {
	    	for (size_t col = 0; col < BOARD_SIZE; ++col) {
	    		const size_t swapLayer = rand() % size, swapRow = rand() % BOARD_SIZE, swapCol = rand() % BOARD_SIZE;
	    		std::swap(matrix[layer][row][col], matrix[swapLayer][swapRow][swapCol]);
	    	}
	    }
    }
}

unsigned initBoard(char board[MAX_LAYERS][BOARD_SIZE][BOARD_SIZE], const char* tiles, const unsigned tileTypes, const size_t layers) {
	if (tileTypes > TILES_TYPE_COUNT) {
        throw std::exception("Invalid count for tile types!");
	}

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    unsigned tilesFilled = 0;

    //fills the board with random amount of random tiles
    const size_t maxTilesCountOfTypeDivide3 = BOARD_SIZE * BOARD_SIZE * layers / tileTypes / 3;

    for (unsigned i = 0; i < tileTypes; ++i) {
        const size_t count = 3 + std::rand() % maxTilesCountOfTypeDivide3 * 3;

        for (size_t j = 0; j < count; ++j) {
            board[tilesFilled / (BOARD_SIZE * BOARD_SIZE)][tilesFilled % (BOARD_SIZE * BOARD_SIZE) / BOARD_SIZE][tilesFilled % (BOARD_SIZE * BOARD_SIZE) % BOARD_SIZE] = tiles[i];
            ++tilesFilled;
        }
    }

    //makes board at least 75% full
    while (tilesFilled < BOARD_SIZE * BOARD_SIZE * layers * BOARD_FILL_FACTOR) {
        board[tilesFilled / (BOARD_SIZE * BOARD_SIZE)][tilesFilled % (BOARD_SIZE * BOARD_SIZE) / BOARD_SIZE][tilesFilled % (BOARD_SIZE * BOARD_SIZE) % BOARD_SIZE] = tiles[tilesFilled / 3 % tileTypes];
        board[tilesFilled / (BOARD_SIZE * BOARD_SIZE)][(tilesFilled + 1) % (BOARD_SIZE * BOARD_SIZE) / BOARD_SIZE][(tilesFilled + 1) % (BOARD_SIZE * BOARD_SIZE) % BOARD_SIZE] = tiles[tilesFilled / 3 % tileTypes];
        board[tilesFilled / (BOARD_SIZE * BOARD_SIZE)][(tilesFilled + 2) % (BOARD_SIZE * BOARD_SIZE) / BOARD_SIZE][(tilesFilled + 2) % (BOARD_SIZE * BOARD_SIZE) % BOARD_SIZE] = tiles[tilesFilled / 3 % tileTypes];
        tilesFilled += 3;
    }

    //fill the rest of the board with empty tiles
    for (size_t tilesCount = tilesFilled; tilesCount < BOARD_SIZE * BOARD_SIZE * layers; ++tilesCount) {
        board[tilesCount / (BOARD_SIZE * BOARD_SIZE)][tilesCount % (BOARD_SIZE * BOARD_SIZE) / BOARD_SIZE][tilesCount % (BOARD_SIZE * BOARD_SIZE) % BOARD_SIZE] = EMPTY_TILE;
    }

    shuffleMatrix(board, layers);
    return tilesFilled;
}

size_t setUpGame(char*& tiles, unsigned& tileTypes) {
    std::cout << "Enter the count of different tile types (must be in [8, 20]): ";
    std::cin >> tileTypes;

    while (tileTypes < 8 || tileTypes > 20) {
        std::cout << "Invalid input! Must be in [8, 20]. Please try again: ";
        std::cin >> tileTypes;
    }

    char choice;
    std::cout << "Do you wish to enter your own symbols for the tiles? (y/n)" << std::endl;
    std::cin >> choice;
    choice = static_cast<char>(std::tolower(choice));

    while (choice != 'y' && choice != 'n') {
        std::cout << "Invalid input! Please try again (y/n)" << std::endl;
        std::cin >> choice;
        choice = static_cast<char>(std::tolower(choice));
    }
    
    tiles = new char[tileTypes];
    initTiles(tiles, tileTypes, choice == 'y');

    size_t layers;
    std::cout << "Enter the count of layers the game will have (must be in [1, 10]): ";
    std::cin >> layers;

    return layers;
}

void printLayersLegend(const size_t layers) {
    std::cout << "Each layer is represented by a different color:" << std::endl;
    const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    for (size_t layer = 0; layer < layers; ++layer) {
        SetConsoleTextAttribute(hStdOut, LAYER_COLORS[layer]);
        std::cout << "Layer " << layer + 1 << std::endl;
    }

    SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    std::cout << std::endl;
}

size_t topNonEmptyLayerAtPos(const char board[MAX_LAYERS][BOARD_SIZE][BOARD_SIZE], const size_t layers, const size_t row, const size_t col) {
    size_t layer = 0;

    while (board[layer][row][col] == EMPTY_TILE && layer < layers - 1) {
        ++layer;
    }

    return layer;
}

void printBoard(const char board[MAX_LAYERS][BOARD_SIZE][BOARD_SIZE], const size_t layers, const size_t currentLayer = 0) {
    std::cout << std::setw(2) << "   ";

    for (size_t i = 1; i <= BOARD_SIZE; ++i) {
        std::cout << std::setw(2) << i << " ";
    }

    std::cout << std::endl;

    const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    for (size_t row = 0; row < BOARD_SIZE; ++row) {
        std::cout << std::setw(2) << row + 1 << " ";

        for (size_t col = 0; col < BOARD_SIZE; ++col) {
            const size_t layer = currentLayer + topNonEmptyLayerAtPos(board + currentLayer, layers, row, col);

            SetConsoleTextAttribute(hStdOut, LAYER_COLORS[layer]);
            std::cout << std::setw(2) << board[layer][row][col] << " ";
        }

        SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        std::cout << std::endl;
    }
}

void printGame(const char board[MAX_LAYERS][BOARD_SIZE][BOARD_SIZE], const size_t layers, const char freeSpace[FREE_SPACE_SIZE], const size_t currentLayer = 0) {
    if (currentLayer >= layers) {
        throw std::exception("Invalid layer to print!");
    }

    const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (currentLayer == 0) {
	    std::cout << "Current board:";
    } else {
        SetConsoleTextAttribute(hStdOut, LAYER_COLORS[currentLayer]);
        std::cout << "Showing layer " << currentLayer + 1 << ":";
        SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    }

    std::cout << std::endl << std::endl;
    printBoard(board, layers, currentLayer);

    std::cout << std::endl << "Free space: " << std::endl;

    SetConsoleTextAttribute(hStdOut, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);

    for (size_t i = 0; i < FREE_SPACE_SIZE; ++i) {
        std::cout << std::setw(2) << freeSpace[i];
    }

    SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    std::cout << std::endl;
}

size_t getTopNonEmptyLayer(char board[MAX_LAYERS][BOARD_SIZE][BOARD_SIZE], const size_t layers) {
    for (size_t layer = 0; layer < layers; ++layer) {
    	for (size_t row = 0; row < BOARD_SIZE; ++row) {
            for (size_t col = 0; col < BOARD_SIZE; ++col) {
                if (board[layer][row][col] != EMPTY_TILE) {
                    return layer;
                }
            }
        }
    }

    return layers;
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

bool checkFreeSpaceForTriple(char freeSpace[FREE_SPACE_SIZE], const char newTile) {
    unsigned count = 0;

    for (size_t i = 0; i < FREE_SPACE_SIZE; ++i) {
        count += freeSpace[i] == newTile;
    }

    if (count < 3) {
	    return false;
    }

    //remove the matching tiles
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
    return true;
}

bool playTurn(char board[MAX_LAYERS][BOARD_SIZE][BOARD_SIZE], const size_t layers, char freeSpace[FREE_SPACE_SIZE]) {
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

    size_t row, col;
    getCoordinatesChoice(row, col);
    size_t layer = topNonEmptyLayerAtPos(board, layers, row, col);
    freeSpace[freeSpaceTiles] = board[layer][row][col];

    while (freeSpace[freeSpaceTiles] == EMPTY_TILE) {
        std::cout << "The selected position is empty! Please try again!" << std::endl;
	    getCoordinatesChoice(row, col);
        layer = topNonEmptyLayerAtPos(board, layers, row, col);
	    freeSpace[freeSpaceTiles] = board[layer][row][col];
    }

    board[layer][row][col] = EMPTY_TILE;
    return checkFreeSpaceForTriple(freeSpace, freeSpace[freeSpaceTiles]) || freeSpaceTiles + 1 < FREE_SPACE_SIZE;
}

void spectatorMode(char board[MAX_LAYERS][BOARD_SIZE][BOARD_SIZE], const size_t layers, char freeSpace[FREE_SPACE_SIZE]) {
    std::cout << std::endl << "Possible commands: " << std::endl;
    std::cout << "DOWN: visualize lower layer" << std::endl;
    std::cout << "UP: visualize upper layer" << std::endl;
    std::cout << "EXIT: return to top layer" << std::endl;

    char command[STR_SIZE];
    std::cin.ignore();
    std::cin.getline(command, STR_SIZE);
    std::cout << std::endl;

    size_t currentLayer = getTopNonEmptyLayer(board, layers);

    while (strcmp(command, "EXIT") != 0) {
        if (strcmp(command, "UP") == 0 && currentLayer > 0) {
            printGame(board, layers, freeSpace, --currentLayer);
        } else if (strcmp(command, "DOWN") == 0 && currentLayer + 1 < layers) {
            printGame(board, layers, freeSpace, ++currentLayer);
        } else {
            std::cout << "Invalid command! Please try again:" << std::endl;
        }

        std::cout << "Possible commands: " << std::endl;
        std::cout << "DOWN: visualize lower layer" << std::endl;
        std::cout << "UP: visualize upper layer" << std::endl;
        std::cout << "EXIT: return to top layer" << std::endl;

        std::cin.getline(command, STR_SIZE);
        std::cout << std::endl;
    }
}

void playGame(const char* tiles, const unsigned tileTypesCount, const size_t layers) {
    char board[MAX_LAYERS][BOARD_SIZE][BOARD_SIZE], freeSpace[FREE_SPACE_SIZE];

    //initialize the free space
    for (size_t i = 0; i < FREE_SPACE_SIZE; ++i) {
        freeSpace[i] = ' ';
    }

    //Each game the count of each type of tiles is random, because it's more fun.
    //For static count the first board needs to be saved, and shuffled each game.
    unsigned tilesCount = initBoard(board, tiles, tileTypesCount, layers);
    bool hasLost = false;

    while (tilesCount != 0 && !hasLost) {
        printGame(board, layers, freeSpace);

        std::cout << "What would you like to do?" << std::endl;
        std::cout << "1) Take a tile out" << std::endl;
        std::cout << "2) Visualize other layers" << std::endl;

        char choice;
        std::cin >> choice;

        while (choice != '1' && choice != '2') {
            std::cout << "Invalid input (1/2)! Please try again: ";
            std::cin >> choice;
        }

        try {
            if (choice == '1') {
		        hasLost = !playTurn(board, layers, freeSpace);
            	--tilesCount;
	        } else {
		        spectatorMode(board, layers, freeSpace);
	        }
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    printGame(board, layers, freeSpace);
    std::cout << (tilesCount == 0 ? "You win! Congratulations!!!" : "Game over! Better luck next time!") << std::endl;
}

int main() {
    bool inGame = true;
    char* tiles;
    unsigned tileTypesCount;
    const size_t layers = setUpGame(tiles, tileTypesCount);

    while (inGame) {
        printLayersLegend(layers);
        playGame(tiles, tileTypesCount, layers);

        char choice;
        std::cout << "Do you want to play again? (y/n)" << std::endl;
        std::cin >> choice;
        choice = static_cast<char>(std::tolower(choice));

        while (choice != 'y' && choice != 'n') {
            std::cout << "Invalid input! Please try again (y/n)" << std::endl;
            std::cin >> choice;
            choice = static_cast<char>(std::tolower(choice));
        }

        inGame = choice == 'y';

        if (inGame) {
	        std::cout << std::endl << "A new game has started!" << std::endl;
        }
    }

    delete[] tiles;
    return 0;
}
