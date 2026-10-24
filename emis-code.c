#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int RED_PIN  = 2;

const int JOY_HORZ = A0;
const int JOY_VERT = A1;

const int MAX_WIDTH  = 128;
const int MAX_HEIGHT = 64;

const int SET   = 1;
const int CLEAR = 0;

//player health, healing, damage variables:
int playerHealth = 6;
int healthUp = 1;
int damageReceived = 1;

int doorsOpened = 4;

//For following the current selected cell + knowing which cell will be randomized: 
int xCursor = 1;
int yCursor = 1;


/* SETUP()
 * Function that runs once, when the board gets power or resets.
 * Used for config, like starting the display, or assigning pins as I/O.
 */
void setup() {

    Serial.begin(9600);

    // make inputs 2 and 3 button inputs with pullup
    pinMode(RED_PIN, INPUT_PULLUP);
}


/* LOOP()
 * Function that runs forever, while the board has power, starting as soon as setup() finishes.
 * Without a delay(ms), it can run thousands of times a second.
 */
void loop() {

    //to clear previous frame:
    display.fillScreen(BLACK);

    int red  = digitalRead(RED_PIN);

    int horz = analogRead(JOY_HORZ);
    delay(200);
    int vert = analogRead(JOY_VERT);
    delay(200);

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


     //drawing the grid + rectangles:
    for(int i = 0; i < 65, i += 32) {
        for(int j = 0; j < 45; j += 22) {
            drawRect(uint16_t i, uint16_t j, uint16_t 30, uint16_t 20, uint16_t SET);
           
        }
    }


    //grid filling in first iteration of loop + refilling grid cells if stage is cleared:
    if (doorsOpened == 4) {
        for(int i = 0; i < 65, i += 32) {
            for(int j = 0; j < 45; j += 22) {
                 fillRect(uint16_t i; uint16_t j, uint16_t 30, uint16_t 20, uint16_t SET);

            }
        }
        doorsOpened = 0;
    }


    //Drawing the selected cell, and tracking it:
        //cursor movement:
    drawRect(uint16_t xCursor, uint16_t yCursor, uint16_t 28, uint16_t 18, uint16_t SET);
    delay(50);
    drawRect(uint16_t xCursor, uint16_t yCursor, uint16_t 28, uint16_t 18, uint16_t CLEAR);
    delay(100);

        //following analog stick movement:
    if ((horz == 0 && vert == 1) && yCursor > 22) { //moving up:
        yCursor -= 22;

    } else if ((horz == 1 && vert == 0) && xCursor < 34) { //moving right
        xCursor += 32;

    } else if ((horz == 0 && vert == -1) && yCursor < 24) { //moving down
        yCursor += 22;

    } else if ((horz == -1 && vert == 0) && xCursor > 32) {  //moving left
        xCursor -= 32;

    }


    //Selection and Randomization:
    //might need an array to hold which cells are drawn already, to draw them with each iteration of the loop:



    
    // drawing the player hp, "H" "P" and opened cell count:
    display.setCursor(98 , 4);
    display.setTextSize(2);
    display.setTextColor(1);
    display.print(playerHealth);

    drawChar(uint16_t 97, uint16_t 23, char H, uint16_t SET, uint16_t CLEAR, uint8_t 1);
    drawChar(uint16_t 106, uint16_t 23, char P, uint16_t SET, uint16_t CLEAR, uint8_t 1);

    //displaying opened door count:
    display.setCursor(98, 47);
    display.setTextSize(2);
    display.setTextColor(1);
    display.print(doorsOpened" / 4");



}