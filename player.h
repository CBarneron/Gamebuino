#ifndef PLAYER_H
#define PLAYER_H

#include <Gamebuino-Meta.h>

class Player
{
public:
    Player(int startX, int startY);
    void update();
    void draw();
    void reset();
    bool isAlive() const;
    void damage();

    void updateBomb();
    void placeBomb();
    bool canPlaceBomb() const;

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
    static const unsigned long BOMB_COOLDOWN = 4000;
    static const unsigned long BOMB_FUSE = 2000;
};

#endif