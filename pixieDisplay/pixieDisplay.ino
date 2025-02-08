#include "Pixie.h"
#include "Pixie_Icon_Pack.h"  // <--- NEEDED FOR BUILT-IN ICONS

#define NUM_PIXIES  8                     // PCBs, not matrices
#define CLK_PIN     8                     // Any digital pin
#define DATA_PIN    7                     // Any digital pin
#define DISPLAY_WIDTH (NUM_PIXIES * 2)  // 16 character-wide display
Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN); // Set up display buffer

unsigned long previousMillis = 0;
unsigned long interval = 150;
int state = 1;
int dir = 0;
static char buffer[DISPLAY_WIDTH + 1] = "";  // +1 for null terminator
static String currentState = "A"; // Start in state "A"
static bool cPrinted = false; // Track if "C" has been executed already

void setup() {
  Serial.begin(115200);
  pix.begin(FULL_SPEED);
}

void loop() {

  if (Serial.available()) {
    String receivedData = Serial.readStringUntil('\n');
    receivedData.trim();
    if (receivedData.length() == 0) {
      Serial.println(" Ignore empty messages ");
      return;
    }
    Serial.print("receivedData: ");
    Serial.println(receivedData);

    if (receivedData == "bye") {
      currentState = "A";
    } else if (receivedData == "kitt") {
      currentState = "B";
    } else {
      currentState = "C";
      cPrinted = false;
      Serial.println("got to C");
    }
    pix.clear();
    receivedData.toCharArray(buffer, sizeof(buffer));
    Serial.print("Buffer: ");
    Serial.println(buffer);
  }

  if (currentState == "A") {
    A();
  } else if (currentState == "B") {
    B(buffer);
  } else if (currentState == "C" && !cPrinted) {

    int len = strlen(buffer);
    Serial.print("Displaying: ");  Serial.print(len);  Serial.println(buffer);

    if (len > DISPLAY_WIDTH) len = DISPLAY_WIDTH;  // Prevent overflow
    int start_pos = (DISPLAY_WIDTH - len) / 2;  // Centering calculation
    for (int i = 0; i < len; i++) {
      pix.write(buffer[i], start_pos + i);  // Place each character at the correct position
    }
    pix.show();
    cPrinted = true; // Ensure "C" runs only once
  }
}

void A() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    Serial.println(currentMillis);
  }
}

void B(char* buffer) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    Serial.println(state);
    if (!dir) {
      switch (state) {
        case 1:  pix.write(PIX_DITHER_16);  pix.show(); pix.shift(1); break;
        case 2:  pix.write(PIX_DITHER_12);  pix.show(); pix.shift(1); break;
        case 3:  pix.write(PIX_DITHER_8);   pix.show(); pix.shift(1); break;
        case 4:  pix.write(PIX_DITHER_4);   pix.show(); pix.shift(1); break;
        case 5:  pix.write(PIX_DITHER_2);   pix.show(); pix.shift(1); break;
        case 6:  pix.write(PIX_DITHER_1);   pix.show(); pix.shift(1); break;
        case 7:  pix.write(PIX_DICE_1);     pix.show(); pix.shift(1); break;
        case 23: pix.clear(); dir = 1; state = 0; break; //clear, change direction, reset state
        default:
          pix.shift(1); pix.show();
      }
    } else {
      switch (state) {
        case 1:  pix.push(PIX_DITHER_16);   pix.show(); break;
        case 2:  pix.push(PIX_DITHER_12);   pix.show(); break;
        case 3:  pix.push(PIX_DITHER_8);    pix.show(); break;
        case 4:  pix.push(PIX_DITHER_4);    pix.show(); break;
        case 5:  pix.push(PIX_DITHER_2);    pix.show(); break;
        case 6:  pix.push(PIX_DITHER_1);    pix.show(); break;
        case 7:  pix.push(PIX_DICE_1);      pix.show(); break;
        case 23: pix.clear(); dir = 0; state = 0; break; //clear, change direction, reset state
        default:
          pix.push(" "); pix.show();
      }
    }
    state++;
  }
}
