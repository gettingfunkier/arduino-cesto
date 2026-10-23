#include "pitches.h"
// #include <Adafruit_GFH.h>
// #include <Adafruit_ST7789.h>

const int BUTTON_PIN  = 2;
const int JOY_HORZ    = A0;
const int JOY_VERT    = A1;

const int MAX_WIDTH   = 128;
const int MAX_HEIGHT  = 64;
const int SET         = 1;
const int CLEAR       = 0;

int button_prev = 0;

#define SPEAKER_PIN 3

/* SETUP()
 * Function that runs once, when the board gets power or resets.
 * Used for config, like starting the display, or assigning pins as I/O.
 */
void setup() {

    Serial.begin(9600);

    // make input 2 button and output 3 speaker
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(SPEAKER_PIN, OUTPUT);
}

void playSound() {
    tone(SPEAKER_PIN, NOTE_G5);
    delay(100);
    tone(SPEAKER_PIN, NOTE_B5);
    delay(100);
    tone(SPEAKER_PIN, NOTE_D6);
    delay(100);
    tone(SPEAKER_PIN, NOTE_FS6);
    delay(100);
    tone(SPEAKER_PIN, NOTE_A6, 100);
}

/* LOOP()
 * Function that runs forever, while the board has power, starting as soon as setup() finishes.
 * Without a delay(ms), it can run thousands of times a second.
 */
void loop() {

    int button_rev = digitalRead(BUTTON_PIN);
    int horz       = analogRead(JOY_HORZ);
    int vert       = analogRead(JOY_VERT);

    int button = 0;
    if (button_rev == 0) button = 1;
    else button = 0;

    // DEBUG: button/buzzer states
    Serial.print("Button: ");
    Serial.print(button);

    // DEBUG: joystick position
    Serial.print(" X: ");
    Serial.print(horz);
    Serial.print(" Y: ");
    Serial.println(vert);

    if (button && ) {
        playSound();
    }

    int button_prev = button;
}