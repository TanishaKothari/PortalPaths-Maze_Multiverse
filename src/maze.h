#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <stack>
#include "raylib.h"
#include "game_state.h"
#include "resources.h"

// Desert obstacle types
enum class DesertObstacleType {
    Cactus,
    Tumbleweed
};

// Directions for maze generation
const int directions[4][2] = {
    {0, -1},  // Up
    {1, 0},   // Right
    {0, 1},   // Down
    {-1, 0}   // Left
};

// Cell structure
struct Cell {
    int x, y;
    bool visited;
    bool walls[4]; // Top, Right, Bottom, Left
    bool isObstacle;
    int spriteRow;
    int spriteCol;
    int logSpriteRow;
    int logSpriteCol;
    DesertObstacleType desertObstacleType;

    Cell(int x_, int y_);
    void Draw() const;
};

void InitializeGrid();
bool IsInBounds(int x, int y);
Cell* GetUnvisitedNeighbor(Cell* current);
void RemoveWalls(Cell* current, Cell* next);
void GenerateMaze();
void DrawBorder();
void PlaceRandomExit();
void DrawExit(int exitX, int exitY, float time);
int Heuristic(int x1, int y1, int x2, int y2);
bool PathExists(int startX, int startY, int endX, int endY);
void AddRandomObstacles(int exitX, int exitY);

#endif