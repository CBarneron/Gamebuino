#include <Gamebuino-Meta.h>
#include "Bomberman.h"

enum GameState {
    STATE_MENU,
    STATE_BOMBERMAN,
    STATE_SNAKE,
    STATE_TETRIS
};

struct MenuItem {
    const char* name;
    GameState state;
};

const int MAX_MENU_ITEMS = 3;
MenuItem menuItems[MAX_MENU_ITEMS] = {
    {"BOMBERMAN", STATE_BOMBERMAN},
    {"SNAKE", STATE_SNAKE},
    {"TETRIS", STATE_TETRIS}
};

GameState currentState = STATE_MENU;
int menuSelection = 0;


void setup() {
    gb.begin();
}

void drawMenu() {
    gb.display.clear();
    gb.display.setColor(WHITE);
    gb.display.setCursor(25, 10);
    gb.display.print("RETRO GAMES");
    
    for(int i = 0; i < MAX_MENU_ITEMS; i++) {
        gb.display.setCursor(30, 25 + i * 10);
        if(menuSelection == i) {
            gb.display.setColor(YELLOW);
        } else {
            gb.display.setColor(WHITE);
        }
        gb.display.print(menuItems[i].name);
    }
}

void updateMenu() {
    if(gb.buttons.pressed(BUTTON_UP)) {
        menuSelection--;
        if(menuSelection < 0) menuSelection = MAX_MENU_ITEMS - 1;
    }
    if(gb.buttons.pressed(BUTTON_DOWN)) {
        menuSelection++;
        if(menuSelection >= MAX_MENU_ITEMS) menuSelection = 0;
    }
    if(gb.buttons.pressed(BUTTON_A)) {
        currentState = menuItems[menuSelection].state;
    }
}

void loop() {
    while(!gb.update());
    
    if(gb.buttons.pressed(BUTTON_MENU) && currentState != STATE_MENU) {
        currentState = STATE_MENU;
        return;
    }

    switch(currentState) {
        case STATE_MENU:
            drawMenu();
            updateMenu();
            break;
            
        case STATE_BOMBERMAN:
            bombermanLoop();
            break;
            
        case STATE_SNAKE:
            gb.display.clear();
            gb.display.setCursor(25, 30);
            gb.display.print("SNAKE - WIP");
            break;
            
        case STATE_TETRIS:
            gb.display.clear();
            gb.display.setCursor(25, 30);
            gb.display.print("TETRIS - WIP");
            break;
    }
}