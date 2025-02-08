#include "Pixie.h"
#include "Pixie_Icon_Pack.h"  // Required for built-in icons
#include <avr/wdt.h>

// Display Configuration
#define NUM_PIXIES  8                     // Number of Pixie PCBs
#define CLK_PIN     8                     // Clock pin (any digital pin)
#define DATA_PIN    7                     // Data pin (any digital pin)
#define DISPLAY_WIDTH (NUM_PIXIES * 2)  // Display width in characters (16 total)
Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN); // Initialize the Pixie display

// Timing and State Variables
unsigned long previousMillis = 0; // Stores the last update time
unsigned long interval = 150;     // Interval between updates (ms)
int state = 1;                    // Current animation state
bool dir = 0;                      // Direction of animation

// Buffer for Display and State Management
static char buffer[DISPLAY_WIDTH + 1] = "";  // Display buffer (+1 for null terminator)
static String currentState = "A"; // Current display mode, starting at "A"

void setup() {
  Serial.begin(115200);      // Start serial communication
  pix.begin(FULL_SPEED);     // Initialize the Pixie display
}

void loop() {
  handleSerialInput(); // Process incoming serial data

  // Execute corresponding display function
  if (currentState == "A") {
    A();
  } else if (currentState == "B") {
    B();
  } else if (currentState == "C") {
    displayC();
  }
}

// Function to handle serial input
void handleSerialInput() {
  if (Serial.available()) {
    String receivedData = Serial.readStringUntil('\n');
    receivedData.trim(); // Remove leading/trailing whitespace

    // Reset to state "A" if an empty message is received
    if (receivedData.length() == 0) {
      reboot();
    }

    Serial.print("receivedData: ");
    Serial.println(receivedData);

    // Determine new state based on received data
    if (receivedData == "bye") {
      currentState = "A";
    } else if (receivedData == "kitt") {
      currentState = "B";
    } else {
      currentState = "C";
      Serial.println("got to C");
    }

    pix.clear(); // Clear display before updating
    receivedData.toCharArray(buffer, sizeof(buffer));
    Serial.print("Buffer: "); Serial.println(buffer);
  }
}

// Placeholder function for state "A" (no display updates)
void A() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
  }
}

// Animation function for state "B" (Knight Rider effect)
void B() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    Serial.println(state);

    if (!dir) { // Forward direction
      switch (state) {
        case 1:  pix.write(PIX_DITHER_16); pix.show(); pix.shift(1); break;
        case 2:  pix.write(PIX_DITHER_12); pix.show(); pix.shift(1); break;
        case 3:  pix.write(PIX_DITHER_8);  pix.show(); pix.shift(1); break;
        case 4:  pix.write(PIX_DITHER_4);  pix.show(); pix.shift(1); break;
        case 5:  pix.write(PIX_DITHER_2);  pix.show(); pix.shift(1); break;
        case 6:  pix.write(PIX_DITHER_1);  pix.show(); pix.shift(1); break;
        case 7:  pix.write(PIX_DICE_1);    pix.show(); pix.shift(1); break;
        case 23: pix.clear(); dir = 1; state = 0; break; // Reset & reverse
        default:
          pix.shift(1); pix.show();
      }
    } else { // Reverse direction
      switch (state) {
        case 1:  pix.push(PIX_DITHER_16); pix.show(); break;
        case 2:  pix.push(PIX_DITHER_12); pix.show(); break;
        case 3:  pix.push(PIX_DITHER_8);  pix.show(); break;
        case 4:  pix.push(PIX_DITHER_4);  pix.show(); break;
        case 5:  pix.push(PIX_DITHER_2);  pix.show(); break;
        case 6:  pix.push(PIX_DITHER_1);  pix.show(); break;
        case 7:  pix.push(PIX_DICE_1);    pix.show(); break;
        case 23: pix.clear(); dir = 0; state = 0; break; // Reset & forward
        default:
          pix.push(' '); pix.show();
      }
    }
    state++; // Increment state for next update
  }
}

// Function to handle state "C"
void displayC() {
  int len = strlen(buffer);
  Serial.print("Displaying: "); Serial.print(len); Serial.println(buffer);

  if (len > DISPLAY_WIDTH) len = DISPLAY_WIDTH; // Prevent overflow
  int start_pos = (DISPLAY_WIDTH - len) / 2;   // Center text
  for (int i = 0; i < len; i++) {
    pix.write(buffer[i], start_pos + i); // Write each character to display
  }
  pix.show(); // Refresh display

  currentState = "A"; // Reset state after execution
}

void reboot() {
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (1) {}
}
