#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>

extern const int boardWidth;
extern const int boardHeight;
extern const int numberOfMines;

struct Cell
{
    bool isOpen;
    bool hasMine;
    int adjacentMines;
    bool isMarked;

    Cell() : isOpen(false), hasMine(false), adjacentMines(0), isMarked(false) {}
};

void PlaceMines();
void CountAdjacentMines(int x, int y, Cell &cell);
bool HasMine(int x, int y);
bool IsOpen(int x, int y);
void OpenCell(int x, int y);
int GetAdjacentMines(int x, int y);
bool BorderCheck(int x, int y);
bool IsZeroAdjacentMines(int x, int y);
void OpenOneCell(int x, int y);
void MarkCell(int x, int y);
bool IsMarked(int x, int y);
int CheckWinOrLose();
