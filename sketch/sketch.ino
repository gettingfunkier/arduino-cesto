#include "pitches.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_PIN  2
#define SPEAKER_PIN 3
#define JOY_HORZ    A0
#define JOY_VERT    A1

const int MAX_WIDTH    = 128;
const int MAX_HEIGHT   = 64;
const int SET          = 1;
const int CLEAR        = 0;

const int GRID_COLS    = 3;
const int GRID_ROWS    = 3;
const int CELL_W       = 32;
const int CELL_H       = 22;
const int BOX_W        = 30;
const int BOX_H        = 20;

Adafruit_SSD1306 display = Adafruit_SSD1306(MAX_WIDTH, MAX_HEIGHT, &Wire);

// player and door variables
int playerHealth       = 6;
int healthUp           = 1;
int damageReceived     = 1;
int doorsOpened        = 4;

// For following the current selected cell + knowing which cell will be randomized: 
const int CURSOR_W     = BOX_W - 2;
const int CURSOR_H     = BOX_H - 2;
const int CURSOR_INSET = 1;
int cursorCol          = 0;
int cursorRow          = 0;

int button_prev        = 0;
int horz_prev          = 0;
int vert_prev          = 0;

/* SETUP()
 * Function that runs once, when the board gets power or resets.
 * Used for config, like starting the display, or assigning pins as I/O.
 */
void setup() {

    Serial.begin(9600);
    
    // make input 2 button and output 3 speaker
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(SPEAKER_PIN, OUTPUT);
    
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.display();
}

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

void drawGrid() {
    for (int col = 0; col < GRID_COLS; col++) {
        for (int row = 0; row < GRID_ROWS; row++) {
            int x = col * CELL_W;
            int y = row * CELL_H;
            display.drawRect(x, y, BOX_W, BOX_H, SET);
        }
    }
}

void drawCursor() {
    int x = cursorCol * CELL_W + CURSOR_INSET;
    int y = cursorRow * CELL_H + CURSOR_INSET;
    display.drawRect(x, y, CURSOR_W, CURSOR_H, SET);
}

void moveCursor(int horz, int vert) {
    if (horz && !horz_prev) {
        int newCol = cursorCol + horz;
        if (newCol >= 0 && newCol <= GRID_COLS - 1) {
            cursorCol = newCol;
        }
    }

    if (vert && !vert_prev) {
        int newRow = cursorRow + vert;
        if (newRow >= 0 && newRow <= GRID_ROWS - 1) {
            cursorRow = newRow;
        }
    }

    horz_prev = horz;
    vert_prev = vert;
}

void playSound() {
}

void playDeath() {
    int pitch_main = NOTE_C7;

    for (int i = 0; i < 5; i++) {
        tone(SPEAKER_PIN, pitch_main);
        pitch_main -= 50;
        delay(250);
    }

    for (byte i = 0; i < 10; i++) {
        for (int pitch = -10; pitch <= 10; pitch++) {
            tone(SPEAKER_PIN, NOTE_C5 + pitch);
            delay(5);
        }
    }

    noTone(SPEAKER_PIN);
}

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

    // INPUT HANDLING
    int button_raw = digitalRead(BUTTON_PIN);
    int horz       = parseJoystickX(analogRead(JOY_HORZ));
    int vert       = parseJoystickY(analogRead(JOY_VERT));

    int button = 0;
    if (button_raw == 0) { button = 1;
    } else button = 0;

    debug(button, horz, vert);

    // UPDATE
    moveCursor(horz, vert);

    // DRAW
    if (button && horz && !button_prev) {
        playDeath();
    }

    else if (button && !button_prev) {
        playSound();
    }

    display.clearDisplay();
    drawGrid();
    drawCursor();
    display.display();

    button_prev = button;
}