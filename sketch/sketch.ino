#include <Adafruit_GFH.h>
#include <Adafruit_ST7789.h>

const int RED_PIN  = 2;
const int BLUE_PIN = 3;

const int JOY_HORZ = A0;
const int JOY_VERT = A1;

const int MAX_WIDTH  = 128;
const int MAX_HEIGHT = 64;

const int SET   = 1;
const int CLEAR = 0;

/* SETUP()
 * Function that runs once, when the board gets power or resets.
 * Used for config, like starting the display, or assigning pins as I/O.
 */
void setup() {

    Serial.begin(9600);

    // make inputs 2 and 3 button inputs with pullup
    pinMode(RED_PIN, INPUT_PULLUP);
    pinMode(BLUE_PIN, INPUT_PULLUP);
}

/* LOOP()
 * Function that runs forever, while the board has power, starting as soon as setup() finishes.
 * Without a delay(ms), it can run thousands of times a second.
 */
void loop() {

    int red  = digitalRead(RED_PIN);
    int blue = digitalRead(BLUE_PIN);

    int horz = analogRead(JOY_HORZ);
    int vert = analogRead(JOY_VERT);

    // DEBUG: button states
    Serial.print("Red: ");
    Serial.print(red);
    Serial.print(" Blue: ");
    Serial.print(blue);

    // DEBUG: joystick position
    Serial.print(" X: ");
    Serial.print(horz);
    Serial.print(" Y: ");
    Serial.println(vert);
}

void drawPixel(uint16_t x, uint16_t y, uint16_t color);