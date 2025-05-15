#ifndef ENEMY_H
#define ENEMY_H

#include <Gamebuino-Meta.h>
#include "Player.h"

class Enemy
{
public:
    Enemy(int startX, int startY);
    void update(const Player &player);
    void draw();
    void reset();
    bool isAlive() const;
    void damage();

    void updateBomb();
    void placeBomb();

    int getX() const { return x; }
    int getY() const { return y; }
    bool isBombActive() const { return bombActive; }
    int getBombX() const { return bombX; }
    int getBombY() const { return bombY; }

    bool isInExplosion(int bombX, int bombY) const;

private:
    int x, y;
    int life;
    bool bombActive;
    unsigned long bombPlacedTime;
    unsigned long lastBombTime;
    int bombX, bombY;
    bool fleeing;
    static const unsigned long BOMB_COOLDOWN = 4000;
    static const unsigned long BOMB_FUSE = 2000;

    void moveTowards(int targetX, int targetY);
    void fleeFrom(int dangerX, int dangerY);
};

#endif