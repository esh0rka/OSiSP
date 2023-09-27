#include "GameData.h"

const int boardWidth = 10;
const int boardHeight = 10;
const int numberOfMines = 10;
Cell gameBoard[boardHeight][boardWidth];

int openCellsNumber = 0;
int markedCellsNumber = 0;

bool isLose = false;

void PlaceMines()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int minesPlaced = 0;

    while (minesPlaced < numberOfMines)
    {
        int x = std::rand() % boardWidth;
        int y = std::rand() % boardHeight;

        if (!gameBoard[y][x].hasMine)
        {
            gameBoard[y][x].hasMine = true;
            ++minesPlaced;
        }
    }

    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            CountAdjacentMines(x, y, gameBoard[y][x]);
        }
    }
}

void CountAdjacentMines(int x, int y, Cell &cell)
{
    int count = 0;

    int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };

    for (int i = 0; i < 8; i++)
    {
        int newX = x + dx[i];
        int newY = y + dy[i];

        if (newX >= 0 && newX < boardWidth && newY >= 0 && newY < boardHeight)
        {
            if (gameBoard[newY][newX].hasMine)
            {
                count++;
            }
        }
    }

    cell.adjacentMines = count;
}

bool HasMine(int x, int y)
{
    return gameBoard[y][x].hasMine;
}

bool IsOpen(int x, int y)
{
    return gameBoard[y][x].isOpen;
}

int GetAdjacentMines(int x, int y)
{
    return gameBoard[y][x].adjacentMines;
}

bool IsZeroAdjacentMines(int x, int y)
{
    if (GetAdjacentMines(x, y) > 0)
        return false;
    else
        return true;
}

void OpenOneCell(int x, int y)
{
    if (!IsOpen(x, y))
    {
        ++openCellsNumber;
        gameBoard[y][x].isOpen = true;
    }
    if (IsMarked(x, y))
    {
        gameBoard[y][x].isMarked = false;
        --markedCellsNumber;
    }
}

bool BorderCheck(int x, int y)
{
    if (x >= 0 && x < boardWidth && y >= 0 && y < boardHeight)
        return true;
    else
        return false;
}

void OpenCell(int x, int y)
{
    std::queue<std::pair<int, int>> cellQueue;

    if (!gameBoard[y][x].isOpen && gameBoard[y][x].hasMine)
    {
        gameBoard[y][x].isOpen = true;
        if (IsMarked(x, y))
        {
            gameBoard[y][x].isMarked = false;
            --markedCellsNumber;
        }
        isLose = true;
        return;
    }

    if (gameBoard[y][x].isOpen)
    {
        return;
    }

    cellQueue.push(std::make_pair(x, y));

    while (!cellQueue.empty())
    {
        int currentX = cellQueue.front().first;
        int currentY = cellQueue.front().second;
        cellQueue.pop();
        OpenOneCell(currentX, currentY);
        gameBoard[currentY][currentX].isMarked = false;


        if (IsZeroAdjacentMines(currentX, currentY))
        {
            for (int dx = currentX - 1; dx < currentX + 2; ++dx)
            {
                for (int dy = currentY - 1; dy < currentY + 2; ++dy)
                {
                    if (BorderCheck(dx, dy) && !HasMine(dx, dy) && IsZeroAdjacentMines(dx, dy) && !IsOpen(dx, dy))
                    {
                        cellQueue.push(std::make_pair(dx, dy));
                    }
                    else if (BorderCheck(dx, dy) && !HasMine(dx, dy) && !IsZeroAdjacentMines(dx, dy) && !IsOpen(dx, dy))
                    {
                        OpenOneCell(dx, dy);
                    }
                }
            }
        }
    }
}

void MarkCell(int x, int y)
{
    if (!IsOpen(x, y) && IsMarked(x, y))
    {
        gameBoard[y][x].isMarked = false;
        ++markedCellsNumber;
    }
    else if (!IsOpen(x, y) && !IsMarked(x, y))
    {
        gameBoard[y][x].isMarked = true;
        --markedCellsNumber;
    }
}

bool IsMarked(int x, int y)
{
    return gameBoard[y][x].isMarked;
}

int CheckWinOrLose()
{
    if (isLose)
    {
        return 2;
    }
    else if (markedCellsNumber <= numberOfMines && openCellsNumber == boardWidth * boardHeight - numberOfMines)
    {
        return 0;
    }

    return 1;
}
