#include "pitches.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ARDUINO CESTO SOURCE CODE
 * (C) E. Balcioglu & J. Hamilton, 2026
 */

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

// HEALING
const int HEALTH_UP        = 2;
const int HEALTH_INSET_X   = 10;
const int HEALTH_INSET_Y   = 3;
int playerHealth           = 6;
int healthCol              = 0;
int healthRow              = 0;
bool healthFound;

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
int randNum               = 0;
bool isHighScore          = false;
int highScore;
int currStage;


/* SETUP()
 * Function that runs once, when the board gets power or resets.
 * Used for config, like starting the display, or assigning pins as I/O.
 */
void setup() {
    Serial.begin(9600);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    
    randomSeed(analogRead(RANDOM_PIN));
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(SPEAKER_PIN, OUTPUT);
    

    startLevel();
    playHello();
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

/* DRAWCURSOR()
 * Function that draws the cursor in its current position.
 */
void drawCursor() {
    int x = cursorCol * CELL_W + CURSOR_INSET;
    int y = cursorRow * CELL_H + CURSOR_INSET;
    display.drawRect(x, y, CURSOR_W, CURSOR_H, SSD1306_INVERSE);
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

/* ASSIGNCELL()
* Function that handles cell selection.
* 1/3 chance for cell to be an enemy (max 3 enemies).
* 1/4 chance for cell to be a health boost (max 1 health boost).
*/
void assignCell() {
    randNum = random(9);
    if (randNum < 3 && !allEnemiesFound()) {
        enemyFound[currEnemy] = true;
        enemyCol[currEnemy] = cursorCol;
        enemyRow[currEnemy] = cursorRow;
        
        playerHealth--;
        currEnemy++;

        playEnemy();
    }
    
    else if (randNum > 6 && !healthFound) {
        healthFound = true;
        healthCol = cursorCol;
        healthRow = cursorRow;
        playerHealth += HEALTH_UP;
        if(playerHealth > 6) playerHealth = 6;

        playHealth();
    } 
    
    else playSelect();
}

/* REVEALCELL()
* Function that handles the mechanic of revealing a cell.
* Updates selected cell as revealed, and increments number of open doors by 1.
*/
void revealCell() {
    if (!revealed[cursorCol][cursorRow]) {
        revealed[cursorCol][cursorRow] = true;
        doorsOpened++;
        assignCell();
    }
}

/* ALLENEMIESFOUND()
 * Function that returns whether all enemies have been found.
 */
bool allEnemiesFound() {
    int ef = 0;
    for (int e = 0; e < MAX_ENEMIES; e++) {
        if (enemyFound[e]) ef++;
    }
    return ef == MAX_ENEMIES;
}

/* STARTLEVEL()
 * Function that sets game parameters to a new level, by resetting:
 *      - All revealed cells to false;
 *      - Number of open doors;
 *      - Cursor position to (0,0);
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
    healthFound = false;
}

/* LEVELCOMPLETE()
 * Function that handles level completion and transition.
 */
void levelComplete() {
    delay(500);
    startLevel();
}

/* GAMEOVER()
 * Function that handles game over by:
 *      - Updating high score;
 *      - Resetting game variables;
 *      - Displaying game over screen, and high score value;
 *      - Restarting the game;
 */
void gameOver() {
    if (currStage > highScore) {
        highScore = currStage;
        isHighScore = true;
    }

    playerHealth = 6;
    currStage    = 1;
    cursorCol    = 0;
    cursorRow    = 0;
    
    display.clearDisplay();
    display.display();
    delay(500);
    display.setCursor(37, 24);
    display.setTextSize(1);
    display.print("GAME OVER");

    display.setCursor(24, 32);
    display.print("High Score: ");
    display.print(highScore);
    display.display();
    playGameOver();
    delay(500);

    display.clearDisplay();
    display.display();
    delay(1000);

    startLevel();
    playHello();
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
    display.setCursor(112, 4);
    display.setTextSize(1);
    display.print("HP");

    // Draw door information
    display.setCursor(98, 47);
    display.setTextSize(1);
    display.print(doorsOpened);
    display.print("|");
    display.print(DOORS_REQUIRED);
}

/* DRAWENEMIES()
 * Function that draws an enemy on the previously determined enemy cell
 */
void drawEnemies() {
    int e = 0;
    while (enemyFound[e] && e < MAX_ENEMIES) {
        display.setTextSize(1);
        display.setCursor(
            enemyCol[e] * CELL_W + ENEMY_INSET_X, 
            enemyRow[e] * CELL_H + ENEMY_INSET_Y
        );
        display.print("^~^");
        e++;
    }
}

/* DRAWHEALTH()
 * Function that draws an health sign on the previously determined health cell
 */
void drawHealth() {
    if(healthFound) {
        display.setTextSize(2);
        display.setCursor(
            healthCol * CELL_W + HEALTH_INSET_X,
            healthRow * CELL_H + HEALTH_INSET_Y 
        );
        display.print("+");
    }
}

/* PLAYHELLO()
 * Function that plays sound at game start.
 */
void playHello() {
    tone(SPEAKER_PIN, NOTE_C4, 30);
    delay(80);

    tone(SPEAKER_PIN, NOTE_E4, 30);
    delay(80);

    tone(SPEAKER_PIN, NOTE_G4, 30);
    delay(80);

    tone(SPEAKER_PIN, NOTE_C5, 30);
    delay(80);

    tone(SPEAKER_PIN, NOTE_C6, 30);
    delay(80);
}

/* PLAYSELECT()
 * Function that plays sound at cell selection.
 */
void playSelect() {
    tone(SPEAKER_PIN, NOTE_C5, 30);
}

/* PLAYENEMY()
 * Function that plays sound at enemy reveal.
 */
void playEnemy() {
    tone(SPEAKER_PIN, NOTE_C7, 200);
}

/* PLAYHEALTH()
 * Function that plays sound at health reveal.
 */
void playHealth() {
    tone(SPEAKER_PIN, NOTE_C6, 70);
    delay(70);
    tone(SPEAKER_PIN, NOTE_G6, 130);
}

/* PLAYGAMEOVER()
 * Function that plays sound at game over.
 */
void playGameOver() {
    tone(SPEAKER_PIN, NOTE_GS5, 50);
    delay(250);

    tone(SPEAKER_PIN, NOTE_G5, 50);
    delay(250);

    tone(SPEAKER_PIN, NOTE_FS5, 50);
    delay(250);

    tone(SPEAKER_PIN, NOTE_F5, 50);
    delay(250);

    tone(SPEAKER_PIN, NOTE_E5, 50);
    delay(250);

    tone(SPEAKER_PIN, NOTE_DS5, 1000);
    delay(250);
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
    drawHealth();
    display.display();

    // --- GAME STATES ---
    if (doorsOpened == DOORS_REQUIRED) {
        currStage++;
        levelComplete();
    }

    if (playerHealth == 0) gameOver();
}