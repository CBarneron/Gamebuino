#include <Gamebuino-Meta.h>

const int SCREEN_W = gb.display.width();
const int SCREEN_H = gb.display.height();
const int GAME_W = 10;
const int GAME_H = 13;
const int BLOCK_SIZE = 5;
const int SCOREBOARD_WIDTH = SCREEN_W - (GAME_W * BLOCK_SIZE) - 2;

enum State { MENU = 1,
             JEU,
             PAUSE,
             GAMEOVER };
State ETAT = MENU;

const char* KONAMI_CODE = "UUDDLRLRBA";  // U=Up, D=Down, L=Left, R=Right, B, A
String inputBuffer = "";
bool easterEgg = false;

bool rainbow = false;
int cycle = 0;

const Color GOLD = gb.createColor(255, 215, 0);
const Color BLOOD = gb.createColor(139, 0, 0);

// Variables de jeu
int menuSelection = 0;
int grid[GAME_H][GAME_W] = { 0 };
Color gridColors[GAME_H][GAME_W] = { BLACK };
int currentPiece[4][4] = { 0 };
int nextPiece[4][4] = { 0 };
int pieceX = 3, pieceY = 0;
int score = 0, highScore = 0;
int lines = 0, level = 0;
int speed = 500;
unsigned long lastMoveTime = 0;
Color pieceColor;

// Son
typedef int8_t sound_t;
sound_t gbSound;
Color bgColor;
Color blockColor;
bool soundOn = true;

// Bag randomizer
int bag[7];
int bagIndex = 0;
void shuffleBag() {
  // Initialiser bag avec 0..6
  for (int i = 0; i < 7; i++) bag[i] = i;
  // Fisher–Yates shuffle
  for (int i = 6; i > 0; i--) {
    int j = random(0, i + 1);
    int tmp = bag[i];
    bag[i] = bag[j];
    bag[j] = tmp;
  }
  bagIndex = 0;
}


const uint32_t interval = 44;
uint32_t prevMillis = 0;

Color colorsLed[8] = {
        gb.createColor(255, 0, 0),     // Rouge
        gb.createColor(255, 127, 0),   // Orange
        gb.createColor(255, 255, 0),   // Jaune
        gb.createColor(0, 255, 0),     // Vert
        gb.createColor(0, 255, 255),   // Cyan
        gb.createColor(0, 0, 255),     // Bleu
        gb.createColor(139, 0, 255),   // Violet
        gb.createColor(255, 0, 127)    // Rose
    };

int getNextPieceIndex() {
  if (bagIndex >= 7) {
    shuffleBag();
  }
  return bag[bagIndex++];
}

// Palette dynamique par niveau (niveau 0 à 15)
const Color couleursParNiveau[16][7] = {
  { gb.createColor(0xFF, 0x00, 0x00), gb.createColor(0x00, 0xFF, 0x00), gb.createColor(0x00, 0xFF, 0xFF),
    gb.createColor(0xFF, 0xFF, 0x00), gb.createColor(0xFF, 0x7F, 0x00), gb.createColor(0x80, 0x00, 0x80), gb.createColor(0x00, 0x00, 0xFF) },
  { gb.createColor(255, 80, 80), gb.createColor(80, 255, 80), gb.createColor(80, 255, 255),
    gb.createColor(255, 255, 80), gb.createColor(255, 160, 80), gb.createColor(200, 80, 200), gb.createColor(80, 80, 255) },
  { gb.createColor(240, 0, 0), gb.createColor(0, 240, 0), gb.createColor(0, 240, 240),
    gb.createColor(240, 240, 0), gb.createColor(240, 120, 0), gb.createColor(160, 0, 160), gb.createColor(0, 0, 240) },
  { gb.createColor(255, 140, 140), gb.createColor(140, 255, 140), gb.createColor(140, 255, 255),
    gb.createColor(255, 255, 140), gb.createColor(255, 200, 140), gb.createColor(220, 140, 220), gb.createColor(140, 140, 255) },
  { gb.createColor(200, 50, 50), gb.createColor(50, 200, 50), gb.createColor(50, 200, 200),
    gb.createColor(200, 200, 50), gb.createColor(200, 150, 50), gb.createColor(180, 50, 180), gb.createColor(50, 50, 200) },
  { gb.createColor(180, 0, 0), gb.createColor(0, 180, 0), gb.createColor(0, 180, 180),
    gb.createColor(180, 180, 0), gb.createColor(180, 90, 0), gb.createColor(120, 0, 120), gb.createColor(0, 0, 180) },
  { gb.createColor(255, 200, 200), gb.createColor(200, 255, 200), gb.createColor(200, 255, 255),
    gb.createColor(255, 255, 200), gb.createColor(255, 230, 200), gb.createColor(230, 200, 255), gb.createColor(200, 200, 255) },
  { gb.createColor(150, 0, 0), gb.createColor(0, 150, 0), gb.createColor(0, 150, 150),
    gb.createColor(150, 150, 0), gb.createColor(150, 80, 0), gb.createColor(100, 0, 100), gb.createColor(0, 0, 150) },
  { gb.createColor(255, 100, 100), gb.createColor(100, 255, 100), gb.createColor(100, 255, 255),
    gb.createColor(255, 255, 100), gb.createColor(255, 180, 100), gb.createColor(180, 100, 180), gb.createColor(100, 100, 255) },
  { gb.createColor(255, 0, 0), gb.createColor(0, 255, 0), gb.createColor(0, 255, 255),
    gb.createColor(255, 255, 0), gb.createColor(255, 127, 0), gb.createColor(128, 0, 128), gb.createColor(0, 0, 255) },
  { gb.createColor(255, 80, 80), gb.createColor(80, 255, 80), gb.createColor(80, 255, 255),
    gb.createColor(255, 255, 80), gb.createColor(255, 160, 80), gb.createColor(200, 80, 200), gb.createColor(80, 80, 255) },
  { gb.createColor(240, 0, 0), gb.createColor(0, 240, 0), gb.createColor(0, 240, 240),
    gb.createColor(240, 240, 0), gb.createColor(240, 120, 0), gb.createColor(160, 0, 160), gb.createColor(0, 0, 240) },
  { gb.createColor(255, 140, 140), gb.createColor(140, 255, 140), gb.createColor(140, 255, 255),
    gb.createColor(255, 255, 140), gb.createColor(255, 200, 140), gb.createColor(220, 140, 220), gb.createColor(140, 140, 255) },
  { gb.createColor(200, 50, 50), gb.createColor(50, 200, 50), gb.createColor(50, 200, 200),
    gb.createColor(200, 200, 50), gb.createColor(200, 150, 50), gb.createColor(180, 50, 180), gb.createColor(50, 50, 200) },
  { gb.createColor(180, 0, 0), gb.createColor(0, 180, 0), gb.createColor(0, 180, 180),
    gb.createColor(180, 180, 0), gb.createColor(180, 90, 0), gb.createColor(120, 0, 120), gb.createColor(0, 0, 180) },
  { gb.createColor(5, 77, 4), gb.createColor(5, 32, 156), gb.createColor(5, 77, 4),
    gb.createColor(5, 32, 156), gb.createColor(23, 23, 25), gb.createColor(5, 33, 163), gb.createColor(4, 77, 4) }
};

const Color backgroundColorsPerLevel[16] = {
  gb.createColor(0, 50, 0), gb.createColor(30, 0, 50), gb.createColor(10, 10, 60), gb.createColor(50, 30, 0),
  gb.createColor(20, 20, 20), gb.createColor(0, 30, 30), gb.createColor(40, 0, 0), gb.createColor(0, 0, 0),
  gb.createColor(10, 10, 30), gb.createColor(15, 10, 15), gb.createColor(5, 15, 20), gb.createColor(10, 5, 10),
  gb.createColor(5, 10, 5), gb.createColor(10, 5, 5), gb.createColor(5, 5, 10), gb.createColor(0, 0, 0)
};

// Définition des 7 pièces dans leur rotation de base
const int pieces[7][4][4] = {
  { { 1, 1, 1, 1 }, { 0 } },           // I
  { { 1, 1, 0, 0 }, { 0, 1, 1, 0 } },  // Z
  { { 0, 1, 1, 0 }, { 1, 1, 0, 0 } },  // S
  { { 1, 1, 1, 0 }, { 0, 1, 0, 0 } },  // T
  { { 1, 1, 1, 0 }, { 1, 0, 0, 0 } },  // L
  { { 1, 1, 1, 0 }, { 0, 0, 1, 0 } },  // J
  { { 1, 1, 0, 0 }, { 1, 1, 0, 0 } }   // O
};

char buttonToChar(Button b) {
  switch (b) {
    case BUTTON_UP: return 'U';
    case BUTTON_DOWN: return 'D';
    case BUTTON_LEFT: return 'L';
    case BUTTON_RIGHT: return 'R';
    case BUTTON_B: return 'B';
    case BUTTON_A: return 'A';
    default: return '\0';
  }
}

void rainbowEffect() {
  uint32_t currentMillis = millis();
  if (currentMillis - prevMillis >= interval) {
    prevMillis = currentMillis;

    // Mise à jour des LEDs avec décalage basé sur 'cycle'
    for (uint8_t i = 0; i < 8; ++i) {
      uint8_t index = (i + cycle) % 8;
      uint8_t x = i % 2;
      uint8_t y = i / 2;
      gb.lights.drawPixel(x, y, colorsLed[index]);
    }

    cycle++;
    
    if (cycle % 16 == 0) {
      rainbow = false;
      gb.lights.clear();
    }
  }
}

void processKonamiInput() {
  static const char* code = KONAMI_CODE;
  static int pos = 0;
  Button buttons[] = { BUTTON_UP, BUTTON_DOWN, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_B, BUTTON_A };
  for (Button b : buttons) {
    if (gb.buttons.pressed(b)) {
      char c = buttonToChar(b);
      if (c == code[pos]) {
        pos++;
        if (code[pos] == '\0') {
          easterEgg = true;
          ETAT = JEU;
          gb.sound.stop(gbSound);
          gb.sound.play("Secret.wav");
          pos = 0;
        }
      } else {
        pos = (c == code[0]) ? 1 : 0;
      }
    }
  }
}

void gameOverEffect() {
  static unsigned long lastUpdate = 0;
  static bool isRed = true;
  static int blinkCount = 0;
  if (blinkCount >= 16) {
    gb.lights.clear();
    return;
  }
  if (millis() - lastUpdate > 44) {
    if (isRed) {
      gb.lights.fill(gb.createColor(255, 0, 0));
    } else {
      gb.lights.clear();
      blinkCount++;
    }
    isRed = !isRed;
    lastUpdate = millis();
  } else {gb.lights.clear();}
}

void generatePiece(int dest[4][4]) {
  int idx = getNextPieceIndex();
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      dest[y][x] = pieces[idx][y][x];
}

bool isCollision(int nx, int ny, int p[4][4]) {
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      if (p[y][x]) {
        int gx = nx + x, gy = ny + y;
        if (gx < 0 || gx >= GAME_W || gy >= GAME_H || (gy >= 0 && grid[gy][gx])) return true;
      }
  return false;
}

void mergePiece() {
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      if (currentPiece[y][x]) {
        grid[pieceY + y][pieceX + x] = 1;
        gridColors[pieceY + y][pieceX + x] = pieceColor;
      }
  score += 1;
}

void clearLines() {
  for (int y = 0; y < GAME_H; y++) {
    bool full = true;
    for (int x = 0; x < GAME_W; x++)
      if (!grid[y][x]) {
        full = false;
        break;
      }
    if (full) {
      lines++;
      rainbow = true;
      score += 16 * level;
      if (lines % 4 == 0) {
        gb.sound.play("NextLevel.wav");
        level++;
        speed = max(244, speed - 50);
        for (int yy = 0; yy < GAME_H; yy++)
          for (int x = 0; x < GAME_W; x++)
            if (grid[yy][x]) gridColors[yy][x] = couleursParNiveau[min(level, 15)][6];
      } else {
        gb.sound.play("ClearLine.wav");
      }
      for (int yy = y; yy > 0; yy--)
        for (int x = 0; x < GAME_W; x++) {
          grid[yy][x] = grid[yy - 1][x];
          gridColors[yy][x] = gridColors[yy - 1][x];
        }
      for (int x = 0; x < GAME_W; x++) grid[0][x] = 0;
    }
  }
}

void spawnPiece() {
  memcpy(currentPiece, nextPiece, sizeof(currentPiece));
  generatePiece(nextPiece);
  pieceX = 3;
  pieceY = 0;
  for (int i = 0; i < 7; i++) {
    if (memcmp(currentPiece, pieces[i], sizeof(currentPiece)) == 0) {
      pieceColor = easterEgg ? BLOOD : couleursParNiveau[min(level, 15)][i];
      break;
    }
  }
  if (isCollision(pieceX, pieceY, currentPiece)) ETAT = GAMEOVER;
}

void rotatePiece() {
  int r[4][4] = { 0 };
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      r[x][3 - y] = currentPiece[y][x];
  if (!isCollision(pieceX, pieceY, r)) memcpy(currentPiece, r, sizeof(r));
}

void updateGame() {
  processKonamiInput();
  if (gb.buttons.pressed(BUTTON_LEFT) && !isCollision(pieceX - 1, pieceY, currentPiece)) pieceX--;
  if (gb.buttons.pressed(BUTTON_RIGHT) && !isCollision(pieceX + 1, pieceY, currentPiece)) pieceX++;
  if (gb.buttons.pressed(BUTTON_DOWN)) {
    if (!isCollision(pieceX, pieceY + 1, currentPiece)) pieceY++;
    else {
      mergePiece();
      clearLines();
      spawnPiece();
    }
  }
  if (gb.buttons.pressed(BUTTON_A)) rotatePiece();
  if (gb.buttons.pressed(BUTTON_MENU)) ETAT = PAUSE;
  if (millis() - lastMoveTime > speed) {
    if (!isCollision(pieceX, pieceY + 1, currentPiece)) pieceY++;
    else {
      mergePiece();
      clearLines();
      spawnPiece();
    }
    lastMoveTime = millis();
  }
}

void drawBlock(int bx, int by, Color col) {
  gb.display.setColor(col);
  gb.display.fillRect(bx * BLOCK_SIZE, by * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
}

void drawGrid() {
  for (int y = 0; y < GAME_H; y++)
    for (int x = 0; x < GAME_W; x++)
      if (grid[y][x]) drawBlock(x, y, gridColors[y][x]);
}

void drawPiece() {
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      if (currentPiece[y][x]) drawBlock(pieceX + x, pieceY + y, pieceColor);
}

void drawNext(int x0, int y0) {
  gb.display.setColor(WHITE);
  gb.display.setCursor(x0, y0);
  gb.display.print("Next:");
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      if (nextPiece[y][x]) drawBlock((x0 / BLOCK_SIZE) + x + 1.5, (y0 / BLOCK_SIZE) + 3 + y,
                                     easterEgg ? BLOOD : couleursParNiveau[min(level, 15)][0]);
}

void drawGameArea() {
  bgColor = easterEgg ? GOLD : backgroundColorsPerLevel[level % 16];
  gb.display.clear(bgColor);
}

void drawScoreboard() {
  int x0 = GAME_W * BLOCK_SIZE + 1;
  gb.display.setColor(DARKBLUE);
  gb.display.fillRect(x0, 0, SCOREBOARD_WIDTH, SCREEN_H);
  gb.display.setColor(WHITE);
  gb.display.setCursor(x0 + 1, 9);
  gb.display.print("Nv:");
  gb.display.print(level + 1);
  gb.display.setCursor(x0 + 1, 24);
  gb.display.print("Ln:");
  gb.display.print(lines);
  drawNext(x0 + 1, 39);
}

void drawPause() {
  gb.display.setColor(WHITE);
  gb.display.setCursor((SCREEN_W / 2) - 20, SCREEN_H / 2);
  gb.display.print("PAUSE");
}

void drawGameOverScreen() {
  gb.display.setColor(BLACK);
  gb.display.fillRect(0, 0, SCREEN_W, SCREEN_H);
  gb.display.setColor(WHITE);
  gb.display.setCursor(SCREEN_W / 2 - 30, SCREEN_H / 2 - 25);
  gb.display.print("GAME OVER");
  gb.display.setCursor(SCREEN_W / 2 - 30, SCREEN_H / 2);
  gb.display.print("Score: ");
  gb.display.print(score);
  gb.display.setCursor(SCREEN_W / 2 - 30, SCREEN_H / 2 + 10);
  gb.display.print("Level: ");
  gb.display.print(level + 1);
  gb.display.setCursor(SCREEN_W / 2 - 30, SCREEN_H / 2 + 20);
  gb.display.print("High Score: ");
  gb.display.print(highScore);
  gb.display.setCursor(SCREEN_W / 2 - 30, SCREEN_H / 2 + 40);
  gb.display.print("PRESS A to Restart");
}

void updateGameOver() {
  if (score > highScore) highScore = score;
  changeSound();
  if (gb.buttons.pressed(BUTTON_A)) {
    memset(grid, 0, sizeof(grid));
    memset(gridColors, 0, sizeof(gridColors));
    score = lines = 0;
    level = 0;
    speed = 500;
    shuffleBag();
    generatePiece(nextPiece);
    ETAT = MENU;
  }
}

void updatePause() {
  if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B) || gb.buttons.pressed(BUTTON_MENU) || gb.buttons.pressed(BUTTON_LEFT) || gb.buttons.pressed(BUTTON_RIGHT) || gb.buttons.pressed(BUTTON_DOWN) || gb.buttons.pressed(BUTTON_UP)) {
    ETAT = JEU;
  }
}

void drawMenu() {
  gb.display.setColor(DARKBLUE);
  gb.display.fillRect(0, 15, SCREEN_W, 40);
  gb.display.setColor(WHITE);
  gb.display.setCursor(20, 18);
  gb.display.print("TETRIS");
  const char* items[2] = { "START", "QUIT" };
  for (int i = 0; i < 2; i++) {
    gb.display.setCursor(30, 32 + i * 8);
    gb.display.print(items[i]);
  }
  gb.display.drawTriangle(22, 32 + menuSelection * 8, 26, 34 + menuSelection * 8, 22, 36 + menuSelection * 8);
}

void updateMenu() {
  if (gb.buttons.pressed(BUTTON_UP) || gb.buttons.pressed(BUTTON_DOWN))
    menuSelection = (menuSelection + 1) % 2;
  if (gb.buttons.pressed(BUTTON_A)) {
    if (menuSelection == 0) {
      ETAT = JEU;
      spawnPiece();
    } else {
      soundOn = !soundOn;
      changeSound();
    }
  }
}

void changeSound() {
  gb.sound.stop(gbSound);
  switch (ETAT) {
    case MENU: gbSound = gb.sound.play("Title.wav", true); break;
    case JEU: gbSound = gb.sound.play("Game.wav", true); break;
    case GAMEOVER: gbSound = gb.sound.play("GameOver.wav"); break;
    default: break;
  }
}

void setup() {
  gb.begin();
  randomSeed(millis());
  shuffleBag();
  generatePiece(nextPiece);
  changeSound();
  prevMillis = millis();
}

void loop() {
  while (!gb.update())
    ;
  drawGameArea();
  switch (ETAT) {
    case MENU:
      updateMenu();
      drawMenu();
      break;
    case JEU:
      if (rainbow) {
        rainbowEffect();
      };
      updateGame();
      drawGrid();
      drawPiece();
      drawScoreboard();
      break;
    case PAUSE:
      drawPause();
      updatePause();
      break;
    case GAMEOVER:
      drawGameOverScreen();
      updateGameOver();
      break;
  }
  if (ETAT == GAMEOVER) gameOverEffect();
}
