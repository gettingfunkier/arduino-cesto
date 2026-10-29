# ARDUINO-CESTO

> Soul-cesto style arduino game

### Directories:

```
arduino-cesto/
│
├─ sketch/            # GAME FILES
│   ├── diagram.json  # WOKWI SIMULATION
│   ├── pitches.h     # DEF. MUSIC NOTE
│   └── sketch.ino    # ARDUINO CODE
│
├─ .gitignore         # UNTRACKED FILES
├─ notebook.md        # NOTEBOOK
└─ README.md          # PROJECT FRONTPAGE
```

### Requirements:

- Arduino Uno
- Adafruit SSD1306 128x64 OLED display
- Push-button (pin 3)
- Piezo buzzer (pin 8)
- Bi-directional analog stick (VRx: pin A0, VRy: pin A1)

### Achievements:

- Reading button and joystick inputs
- Working grid and cursor
- Printing HUD
- Revealing cells
- Enemy decreases player health when encountered (1/4 chance)
- Increments stage after required doors opened

### Blueprint:

<div align="center">
  <img src="https://github.com/gettingfunkier/arduino-cesto/blob/demo.png"/>
</div>