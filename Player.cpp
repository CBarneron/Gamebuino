#include "Player.h"
#include "map.h"

Player::Player(int startX, int startY) : x(startX), y(startY)
{
    reset();
}

void Player::reset()
{
    x = 1;
    y = 1;
    life = 1;
    bombActive = false;
    bombX = -1;
    bombY = -1;
    lastBombTime = 0;
}

void Player::update()
{
    if (!isAlive())
        return;

    // Déplacements
    if (gb.buttons.pressed(BUTTON_UP) && isWalkable(x, y - 1))
        y--;
    if (gb.buttons.pressed(BUTTON_DOWN) && isWalkable(x, y + 1))
        y++;
    if (gb.buttons.pressed(BUTTON_LEFT) && isWalkable(x - 1, y))
        x--;
    if (gb.buttons.pressed(BUTTON_RIGHT) && isWalkable(x + 1, y))
        x++;

    static bool bombWasUnavailable = true;
    if (millis() - lastBombTime > BOMB_COOLDOWN) {
        if (bombWasUnavailable) {
            gb.sound.playTick();
            bombWasUnavailable = false;
        }
    } else {
        bombWasUnavailable = true;
    }
    // Pose de bombe
    if (gb.buttons.pressed(BUTTON_A) && canPlaceBomb())
    {
        placeBomb();
    }
}

void Player::draw()
{
    if (!isAlive())
        return;
    gb.display.setColor(255, 0, 0);
    gb.display.fillRect(x * TILE_SIZE + 2, y * TILE_SIZE + 2, TILE_SIZE - 4, TILE_SIZE - 4);

    if (bombActive)
    {
        gb.display.setColor(125, 0, 0);
        gb.display.fillRect(bombX * TILE_SIZE + 2, bombY * TILE_SIZE + 2, TILE_SIZE - 4, TILE_SIZE - 4);
    }
}

bool Player::isAlive() const
{
    return life > 0;
}

void Player::damage()
{
    if (life > 0)
        life--;
}

void Player::placeBomb()
{
    bombActive = true;
    bombPlacedTime = millis();
    lastBombTime = millis();
    bombX = x;
    bombY = y;
}

bool Player::canPlaceBomb() const
{
    return !bombActive && millis() - lastBombTime > BOMB_COOLDOWN;
}

bool Player::isInExplosion(int bombX, int bombY) const
{
    return (x == bombX && abs(y - bombY) <= 1) ||
           (y == bombY && abs(x - bombX) <= 1);
}

void Player::updateBomb() {
    if (!bombActive) return;
    
    if (millis() - bombPlacedTime > BOMB_FUSE) {
        // Check explosion in all directions (+)
        int directions[5][2] = {
            {0,0},   // Center
            {0,-1},  // Up
            {0,1},   // Down
            {-1,0},  // Left
            {1,0}    // Right
        };
        
        for(int i = 0; i < 5; i++) {
            int ex = bombX + directions[i][0];
            int ey = bombY + directions[i][1];
            
            if (ex >= 0 && ex < MAP_WIDTH && ey >= 0 && ey < MAP_HEIGHT) {
                // Show explosion effect
                gb.display.setColor(RED);
                gb.display.fillRect(ex * TILE_SIZE, ey * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                
                // Check damage
                extern Enemy enemy;
                if (enemy.getX() == ex && enemy.getY() == ey) {
                    enemy.damage();
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
    }
}