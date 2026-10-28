#include "pitches.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_PIN  2
#define SPEAKER_PIN 3
#define JOY_HORZ    A0
#define JOY_VERT    A1
#define RANDOM_PIN  A2


// DISPLAY
const int MAX_WIDTH        = 128;
const int MAX_HEIGHT       = 64;
const int SET              = 1;
const int CLEAR            = 0;

Adafruit_SSD1306 display = Adafruit_SSD1306(MAX_WIDTH, MAX_HEIGHT, &Wire);

// GRIDS
const int GRID_COLS        = 3;
const int GRID_ROWS        = 3;
const int CELL_W           = 32;
const int CELL_H           = 22;
const int BOX_W            = 30;
const int BOX_H            = 20;

bool revealed[GRID_COLS][GRID_ROWS];

// PLAYER
const int HEALTH_UP        = 2;
const int DAMAGE_RECEIVED  = 1;
int playerHealth           = 6;

// ENEMY
const int ENEMY_INSET_X    = 6;
const int ENEMY_INSET_Y    = 7;
const int MAX_ENEMIES      = 3;
int currEnemy              = 0;
int enemyCol[3]            = {0};
int enemyRow[3]            = {0};
bool enemyFound[3];

// DOOR
const int DOORS_REQUIRED   = 4;
int doorsOpened            = 0;

// CURSOR
const int CURSOR_W         = BOX_W - 2;
const int CURSOR_H         = BOX_H - 2;
const int CURSOR_INSET     = 1;
int cursorCol              = 0;
int cursorRow              = 0;

// EDGE DETECTION
int buttonPrev            = 0;
int horzPrev              = 0;
int vertPrev              = 0;

// GAME STATE
int currStage;


/* SETUP()
 * Function that runs once, when the board gets power or resets.
 * Used for config, like starting the display, or assigning pins as I/O.
 */
void setup() {

    Serial.begin(9600);
    randomSeed(analogRead(RANDOM_PIN));
    
    // make input 2 button and output 3 speaker
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(SPEAKER_PIN, OUTPUT);
    
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    startLevel();
}

/* PARSEJOYSTICK() - x and y
 * Function pair that turns raw joystick inputs to readable values.
 * Each need their respective cardinal input as a integer parameter.
 */
int parseJoystickX(int horz) {
    if (horz < 200) return 1;
    else if (horz > 800) return -1;
    else return 0;
}

int parseJoystickY(int vert) {
    if (vert < 200) return 1;
    else if (vert > 800) return -1;
    else return 0;
}

bool allEnemiesFound() {
    int ef = 0;
    for (int e = 0; e < MAX_ENEMIES; e++) {
        if (enemyFound[e]) ef++;
    }
    return ef == MAX_ENEMIES;
}

void assignEnemy() {
    if (random(3) == 1 && !allEnemiesFound()) {
        enemyFound[currEnemy] = true;
        enemyCol[currEnemy] = cursorCol;
        enemyRow[currEnemy] = cursorRow;

        playerHealth--;
        currEnemy++;
    }
}

/* REVEALCELL()
 * Function that handles the mechanic of revealing a cell.
 * Updates selected cell as revealed, and increments number of open doors by 1.
 */
void revealCell() {
    if (!revealed[cursorCol][cursorRow]) {
        revealed[cursorCol][cursorRow] = true;
        doorsOpened++;
        assignEnemy();
        playSelect();
    }
}

/* DRAWGRID()
 * Function that draws the game's grids.
 * Automatically checks whether a grid has been revealed (hollow) or not (full).
 */
void drawGrid() {
    for (int col = 0; col < GRID_COLS; col++) {
        for (int row = 0; row < GRID_ROWS; row++) {
            int x = col * CELL_W;
            int y = row * CELL_H;

            if (revealed[col][row]) display.drawRect(x, y, BOX_W, BOX_H, SET);
            else display.fillRect(x, y, BOX_W, BOX_H, SET);
        }
    }
}

/* MOVECURSOR()
 * Function that updates the cursor's current position.
 * Needs both cardinal joystick values as integer parameters.
 */
void moveCursor(int horz, int vert) {
    if (horz && !horzPrev) {
        int newCol = cursorCol + horz;
        if (newCol >= 0 && newCol <= GRID_COLS - 1) {
            cursorCol = newCol;
        }
    }

    if (vert && !vertPrev) {
        int newRow = cursorRow + vert;
        if (newRow >= 0 && newRow <= GRID_ROWS - 1) {
            cursorRow = newRow;
        }
    }

    horzPrev = horz;
    vertPrev = vert;
}

/* DRAWCURSOR()
 * Function that draws the cursor in its current position.
 */
void drawCursor() {
    int x = cursorCol * CELL_W + CURSOR_INSET;
    int y = cursorRow * CELL_H + CURSOR_INSET;
    display.drawRect(x, y, CURSOR_W, CURSOR_H, SSD1306_INVERSE);
}

/* DRAWHUD()
 * Function that draws the game's HUD, including:
 *      - Player health;
 *      - HP label;
 *      - Door information;
 */
void drawHUD() {
    // Draw player health
    display.setCursor(98, 4);
    display.setTextSize(2);
    display.setTextColor(SET);
    display.print(playerHealth);

    // Draw HP label
    display.setCursor(97, 23);
    display.setTextSize(1);
    display.print("HP");

    // Draw door information
    display.setCursor(98, 47);
    display.setTextSize(1);
    display.print(doorsOpened);
    display.print("|");
    display.print(DOORS_REQUIRED);
}

void drawEnemies() {
    int e = 0;
    while (enemyFound[e]) {
        display.setTextSize(1);
        display.setCursor(
            enemyCol[e] * CELL_W + ENEMY_INSET_X, 
            enemyRow[e] * CELL_H + ENEMY_INSET_Y
        );
        display.print("^~^");
        e++;
    }
}

/* STARTLEVEL()
 * Function that sets game parameters to a new level, by resetting:
 *      - All revealed cells to false;
 *      - Number of open doors;
 *      - Cursor position to (0,0);
 *      
 */
void startLevel() {
    for (int col = 0; col < GRID_COLS; col++) {
        for (int row = 0; row < GRID_ROWS; row++) {
            revealed[col][row] = false;
        }
    }

    for (int enemy = 0; enemy < MAX_ENEMIES; enemy++) {
        enemyFound[enemy] = false;
    }

    doorsOpened = 0;
    cursorCol   = 0;
    cursorRow   = 0;
    currEnemy   = 0;
}

/* LEVELCOMPLETE()
 * --- not defined yet ---
 */
void levelComplete() {
    delay(500);
    startLevel();
}

void playSelect() {
  tone(SPEAKER_PIN, NOTE_C5, 30);
}

void playGameOver() {
    tone(SPEAKER_PIN, NOTE_GS5, 50);
    noTone(SPEAKER_PIN);
    delay(250);

    tone(SPEAKER_PIN, NOTE_G5, 50);
    noTone(SPEAKER_PIN);
    delay(250);

    tone(SPEAKER_PIN, NOTE_FS5, 50);
    noTone(SPEAKER_PIN);
    delay(250);

    tone(SPEAKER_PIN, NOTE_F5, 50);
    noTone(SPEAKER_PIN);
    delay(250);

    tone(SPEAKER_PIN, NOTE_E5, 50);
    noTone(SPEAKER_PIN);
    delay(250);

    tone(SPEAKER_PIN, NOTE_DS5, 1000);
    noTone(SPEAKER_PIN);
    delay(250);
}

void gameOver() {
    playerHealth = 6;
    currStage    = 1;
    cursorCol    = 0;
    cursorRow    = 0;
    
    display.clearDisplay();
    display.setCursor(37, 24);
    display.setTextSize(1);
    display.print("GAME OVER");
    display.display();
    playGameOver();

    display.clearDisplay();
    display.display();
    delay(1000);

    startLevel();
}


/* DEBUG()
 * Utility function to debug code.
 */
void debug(int button, int horz, int vert) {
    // DEBUG: button state
    Serial.print("Button: ");
    Serial.print(button);

    // DEBUG: joystick position
    Serial.print(" X: ");
    Serial.print(horz);
    Serial.print(" Y: ");
    Serial.println(vert);
}

/* LOOP()
 * Function that runs forever, while the board has power, starting as soon as setup() finishes.
 * Without a delay(ms), it can run thousands of times a second.
 */
void loop() {

    // --- INPUT HANDLING ---
    int button_raw = digitalRead(BUTTON_PIN);
    int horz       = parseJoystickX(analogRead(JOY_HORZ));
    int vert       = parseJoystickY(analogRead(JOY_VERT));

    int button = 0;
    if (button_raw == 0) { button = 1;
    } else button = 0;

    debug(button, horz, vert);

    // --- UPDATE GAME ---
    moveCursor(horz, vert);
    
    if (button && !buttonPrev) {
        revealCell();
    }
    
    buttonPrev = button;

    // --- DRAW DISPLAY ---
    display.clearDisplay();
    drawHUD();
    drawGrid();
    drawCursor();
    drawEnemies();
    display.display();

    // --- GAME STATES ---
    if (doorsOpened == DOORS_REQUIRED) {
        currStage++;
        levelComplete();
    }

    if (playerHealth == 0) gameOver();
}