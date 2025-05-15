#include <Gamebuino-Meta.h>
#include "Player.h"
#include "Enemy.h"
#include "map.h"

Player player(1, 1);
Enemy enemy(11, 9);

void setup()
{
    gb.begin();
    initializeMap();
}

void loop()
{
    while (!gb.update())
        ;
    gb.display.clear();

    if (!player.isAlive())
    {
        gb.display.setColor(255, 0, 0);
        gb.display.setCursor(30, 25);
        gb.display.print("GAME OVER");
        gb.display.setCursor(20, 40);
        gb.display.print("Press A");
        if (gb.buttons.pressed(BUTTON_A))
        {
            player.reset();
            enemy.reset();
            initializeMap();
        }
        return;
    }

    if (!enemy.isAlive())
    {
        gb.display.setColor(0, 255, 0);
        gb.display.setCursor(30, 25);
        gb.display.print("YOU WIN!");
        gb.display.setCursor(20, 40);
        gb.display.print("Press A");
        if (gb.buttons.pressed(BUTTON_A))
        {
            player.reset();
            enemy.reset();
            initializeMap();
        }
        return;
    }

    drawMap();
    player.update();
    player.updateBomb();
    enemy.update(player);
    enemy.updateBomb();

    player.draw();
    enemy.draw();
}
