#ifndef MAP_H
#define MAP_H

#include <Gamebuino-Meta.h>
#include "Player.h"
#include "Enemy.h"

extern const int TILE_SIZE;
extern const int MAP_WIDTH;
extern const int MAP_HEIGHT;

enum TileType
{
    EMPTY = 0,
    WALL = 1,
    BOX = 2
};

extern byte levelMap[11][13];

void initializeMap();
void drawMap();
bool isWalkable(int x, int y);
void destroyBox(int x, int y);
void updateBomb(int ex, int ey);

#endif