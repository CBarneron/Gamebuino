#include "Enemy.h"
#include "map.h"

Enemy::Enemy(int startX, int startY) : x(startX), y(startY)
{
    reset();
}

void Enemy::reset()
{
    x = 11;
    y = 9;
    life = 1;
    bombActive = false;
    bombX = -1;
    bombY = -1;
    lastBombTime = 0;
    fleeing = false;
}

void Enemy::update(const Player &player)
{
    if (!isAlive())
        return;

    if (bombActive)
    {
        fleeFrom(bombX, bombY);
    }
    else
    {
        moveTowards(player.getX(), player.getY());
    }

    if (!bombActive && millis() - lastBombTime > BOMB_COOLDOWN)
    {
        if (abs(x - player.getX()) + abs(y - player.getY()) <= 2)
        {
            placeBomb();
        }
    }
}

void Enemy::moveTowards(int targetX, int targetY)
{
    if (targetX > x && isWalkable(x + 1, y))
        x++;
    else if (targetX < x && isWalkable(x - 1, y))
        x--;
    else if (targetY > y && isWalkable(x, y + 1))
        y++;
    else if (targetY < y && isWalkable(x, y - 1))
        y--;
}

void Enemy::fleeFrom(int dangerX, int dangerY)
{
    int bestX = x;
    int bestY = y;
    int maxDist = abs(x - dangerX) + abs(y - dangerY);

    int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (int i = 0; i < 4; i++)
    {
        int nx = x + dirs[i][0];
        int ny = y + dirs[i][1];
        if (isWalkable(nx, ny))
        {
            int dist = abs(nx - dangerX) + abs(ny - dangerY);
            if (dist > maxDist)
            {
                maxDist = dist;
                bestX = nx;
                bestY = ny;
            }
        }
    }

    x = bestX;
    y = bestY;
}

void Enemy::draw()
{
    if (!isAlive())
        return;

    gb.display.setColor(0, 200, 0);
    gb.display.fillRect(x * TILE_SIZE + 2, y * TILE_SIZE + 2, TILE_SIZE - 4, TILE_SIZE - 4);

    if (bombActive)
    {
        gb.display.setColor(0, 0, 255);
        gb.display.fillRect(bombX * TILE_SIZE + 2, bombY * TILE_SIZE + 2, TILE_SIZE - 4, TILE_SIZE - 4);
    }
}

bool Enemy::isAlive() const
{
    return life > 0;
}

void Enemy::damage()
{
    if (life > 0)
        life--;
}

void Enemy::placeBomb()
{
    bombActive = true;
    bombPlacedTime = millis();
    lastBombTime = millis();
    bombX = x;
    bombY = y;
    fleeing = true;
}

bool Enemy::isInExplosion(int bombX, int bombY) const
{
    // Check if enemy is in the explosion range (cross pattern)
    int dx = abs(x - bombX);
    int dy = abs(y - bombY);
    return (dx == 0 && dy <= 1) || (dy == 0 && dx <= 1);
}

void Enemy::updateBomb()
{
    if (!bombActive) return;
    
    if (millis() - bombPlacedTime > BOMB_FUSE) {
       
        int directions[5][2] = {
            {0,0},   
            {0,-1},  
            {0,1},   
            {-1,0},  
            {1,0}    
        };
        
        for(int i = 0; i < 5; i++) {
            int ex = bombX + directions[i][0];
            int ey = bombY + directions[i][1];
            
            if (ex >= 0 && ex < MAP_WIDTH && ey >= 0 && ey < MAP_HEIGHT) {
                
                gb.display.setColor(BLUE);
                gb.display.fillRect(ex * TILE_SIZE, ey * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                extern Player player;
                if (player.getX() == ex && player.getY() == ey) {
                    player.damage();
                }
                if (x == ex && y == ey) {
                    damage();
                }
                destroyBox(ex, ey);
            }
        }
        
        bombActive = false;
        bombX = -1;
        bombY = -1;
        fleeing = false;
    }
}