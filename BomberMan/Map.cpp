#include "map.h"

const int TILE_SIZE = 6;
const int MAP_WIDTH = 13;
const int MAP_HEIGHT = 11;

static const byte INITIAL_MAP[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 2, 0, 2, 0, 2, 2, 0, 0, 1},
    {1, 0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 1},
    {1, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 2, 1},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1},
    {1, 0, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0, 1},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1},
    {1, 2, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 1},
    {1, 0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 1},
    {1, 0, 0, 2, 2, 0, 2, 0, 2, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

byte levelMap[MAP_HEIGHT][MAP_WIDTH];

void initializeMap()
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            levelMap[y][x] = INITIAL_MAP[y][x];
        }
    }
}

void drawMap()
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int posX = x * TILE_SIZE;
            int posY = y * TILE_SIZE;

            switch (levelMap[y][x])
            {
            case EMPTY:
                gb.display.setColor(BLACK);
                break;
            case WALL:
                gb.display.setColor(WHITE);
                break;
            case BOX:
                gb.display.setColor(GRAY);
                break;
            }

            gb.display.fillRect(posX, posY, TILE_SIZE, TILE_SIZE);
        }
    }
}

bool isWalkable(int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
    {
        return false;
    }

    extern Player player;
    extern Enemy enemy;

    if (player.isBombActive() && player.getBombX() == x && player.getBombY() == y)
    {
        return false;
    }

    if (enemy.isBombActive() && enemy.getBombX() == x && enemy.getBombY() == y)
    {
        return false;
    }
    return levelMap[y][x] == EMPTY;
}

void destroyBox(int x, int y)
{
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
    {
        if (levelMap[y][x] == BOX)
        {
            levelMap[y][x] = EMPTY;
        }
    }
}

void updateBomb(int ex, int ey)
{
    destroyBox(ex, ey);

    extern Player player;
    extern Enemy enemy;

    if ((player.getX() == ex && player.getY() == ey))
    {
        player.damage();
    }

    if ((enemy.getX() == ex && enemy.getY() == ey))
    {
        enemy.damage();
    }
}