#include <M5Core2.h>  
#include <ArduinoBLE.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

/* Define constants */
#define MAIN_SCREEN 0
#define SCREEN_1 1
#define SCREEN_2 2
#define SCREEN_3 3
#define SCREEN_4 4

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BUTTON_WIDTH SCREEN_WIDTH/2
#define BUTTON_HEIGHT 100

#define VESSEL_SCREEN_TITLE_INDENT 90
#define VESSEL_SCREEN_DATA_SPACING 30
#define VESSEL_SCREEN_DATA_HORIZONTAL_INDENT 140

/* Implement Vessel Data */
#define DATA_INIT() \
{ \
    .mmsi = (0), \
    .speed = (0), \
    .heading = (0), \
    .latitude = (0), \
    .longitude = (0) \
}

struct VesselData {
  long mmsi;
  float speed;
  float heading;
  double latitude;
  double longitude;
};

struct VesselData Data1;
struct VesselData Data2;
struct VesselData Data3;
struct VesselData Data4;

/* Implement Screen Global */
int currentScreen = 0;

/* Define Buttons */
Button vessel_1(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, "vessel_1");
Button vessel_2(BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT, "vessel_2");
Button vessel_3(0, BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, "vessel_3");
Button vessel_4(BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, "vessel_4");

void setup() {
  // Initialise the empty data structs
  init_data_structs();
  
  // Begin the m5 and the touch screen
  M5.begin();
  M5.Touch.begin();

  // Begin initialization of bluetooth
  if (!BLE.begin()) {
    M5.Lcd.println("Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  // Set the discovered event
  BLE.setEventHandler(BLEDiscovered, bleCentralDiscoverHandler);

  // Start scanning for peripherals with duplicates
  BLE.scanForName("Vessel Advertising", true);

  // Add Button Handlers
  M5.Buttons.addHandler(handle_buttons,  E_TOUCH);

  // Display the main screen
  display_main_screen();
}

void loop() {
  // Poll the central for events
  BLE.poll();

  // Update all data
  M5.update();

  if (M5.BtnB.wasPressed()) {
    // Return to main disply
    display_main_screen();
  }

  // Poll every 50ms
  delay(50);
}

// -------------------------------- TOUCH HANDLING ---------------------------------
void handle_buttons(Event& e) {
  Button& b = *e.button;
  
  // Update the relevant screen depending on touch
  if (currentScreen == MAIN_SCREEN) {
    if (b.x == 0 && b.y == 0) {
      display_screen_1();
    } else if (b.x == BUTTON_WIDTH && b.y == 0) {
      display_screen_2();
    } else if (b.x == 0 && b.y == BUTTON_HEIGHT) {
      display_screen_3();
    } else if (b.x == BUTTON_WIDTH && b.y == BUTTON_HEIGHT) {
      display_screen_4();
    }
  }
  

}

void draw_boundary() {
  // Draw the boundary lines
  M5.Lcd.drawFastVLine(0, 0, SCREEN_HEIGHT, WHITE);
  M5.Lcd.drawFastHLine(0, 0, SCREEN_WIDTH, WHITE);
  M5.Lcd.drawFastVLine(SCREEN_WIDTH-1, 0, SCREEN_HEIGHT, WHITE);
  M5.Lcd.drawFastHLine(0, SCREEN_HEIGHT-1, SCREEN_WIDTH, WHITE);
  return;
}

void draw_table() {
  // Horizontal Lines
  M5.Lcd.drawFastHLine(0, VESSEL_SCREEN_TITLE_INDENT, SCREEN_WIDTH, WHITE);
  M5.Lcd.drawFastHLine(0, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING, SCREEN_WIDTH, WHITE);
  M5.Lcd.drawFastHLine(0, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING*2, SCREEN_WIDTH, WHITE);
  M5.Lcd.drawFastHLine(0, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING*3, SCREEN_WIDTH, WHITE);
  M5.Lcd.drawFastHLine(0, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING*4, SCREEN_WIDTH, WHITE);

  // Vertical Lines
  M5.Lcd.drawFastVLine(VESSEL_SCREEN_DATA_HORIZONTAL_INDENT, VESSEL_SCREEN_TITLE_INDENT, VESSEL_SCREEN_DATA_SPACING*5, WHITE);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextDatum(MC_DATUM);

  // Names
  M5.Lcd.drawString("MMSI", VESSEL_SCREEN_DATA_HORIZONTAL_INDENT/2, 
                      VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 0.5, 1);
  M5.Lcd.drawString("Speed", VESSEL_SCREEN_DATA_HORIZONTAL_INDENT/2, 
                      VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 1.5, 1);
  M5.Lcd.drawString("Heading", VESSEL_SCREEN_DATA_HORIZONTAL_INDENT/2, 
                      VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 2.5, 1);
  M5.Lcd.drawString("Latitude", VESSEL_SCREEN_DATA_HORIZONTAL_INDENT/2, 
                      VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 3.5, 1);
  M5.Lcd.drawString("Longitude", VESSEL_SCREEN_DATA_HORIZONTAL_INDENT/2, 
                      VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 4.5, 1);
}

void display_main_screen() {
  /*
   * IMPORTANT INFORMATION:
   *
   * The M5Core2's display resolution is 320x240 pixels, 0,0 being the top left
   * 
   * The following segments can be split:
   * - Top Left (0, 0) to (160, 120)
   * - Top Right (160, 0) to (320, 120)
   * - Bottom Left (0, 120) to (160, 240)
   * - Bottom Right (160, 120) to (320, 240)
   *
   * MIGHT WANT TO HAVE SOME KIND OF TITLE AT THE TOP
   *
   */
  currentScreen = MAIN_SCREEN;

  // Fill screen
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextDatum(MC_DATUM);

  // Draw vertical line and horizontal lines
  M5.Lcd.drawFastVLine(160, 0, 200, WHITE);
  M5.Lcd.drawFastHLine(0, 100, 320, WHITE);
  M5.Lcd.drawFastHLine(0, 200, 320, WHITE);

  // Draw the boundary lines
  draw_boundary();

  // Top left cursor
  M5.Lcd.drawString("1", BUTTON_WIDTH * 0.5, BUTTON_HEIGHT * 0.5, 4);

  // Top right cursor
  M5.Lcd.drawString("2", BUTTON_WIDTH * 1.5, BUTTON_HEIGHT * 0.5, 4);

  // Bottom left cursor
  M5.Lcd.drawString("3", BUTTON_WIDTH * 0.5, BUTTON_HEIGHT * 1.5, 4);

  // Bottom right cursor
  M5.Lcd.drawString("4", BUTTON_WIDTH * 1.5, BUTTON_HEIGHT * 1.5, 4);

  // Write tha main vessel information
  M5.Lcd.drawString("All Vessels", BUTTON_WIDTH, SCREEN_HEIGHT - (SCREEN_HEIGHT/2 - BUTTON_HEIGHT), 2);

  return;
}

void display_screen_1() {
  // Update screen flag
  currentScreen = SCREEN_1;
  
  // Fill screen
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextDatum(MC_DATUM);

  // Draw the boundary lines
  draw_boundary();
  draw_table();

  // Identify Vessel
  M5.Lcd.drawString("Vessel 1", SCREEN_WIDTH/2, BUTTON_HEIGHT/2, 4);

  // Set Datum
  M5.Lcd.setTextDatum(ML_DATUM);

  // Draw numbers
  M5.Lcd.drawNumber(Data1.mmsi, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 0.5); 
  M5.Lcd.drawFloat(Data1.speed, 2, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 1.5); 
  M5.Lcd.drawFloat(Data1.heading, 2, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 2.5); 
  M5.Lcd.drawFloat(Data1.latitude, 6, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 3.5); 
  M5.Lcd.drawFloat(Data1.longitude, 6, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 4.5);

  return;
}

void display_screen_2() {
  // Update screen flag
  currentScreen = SCREEN_2;
  
  // Fill screen
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextDatum(MC_DATUM);

  // Draw the boundary lines
  draw_boundary();
  draw_table();

  // Identify Vessel
  M5.Lcd.drawString("Vessel 2", SCREEN_WIDTH/2, BUTTON_HEIGHT/2, 4);

  // Set Datum
  M5.Lcd.setTextDatum(ML_DATUM);

  // Draw numbers
  M5.Lcd.drawNumber(Data2.mmsi, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 0.5); 
  M5.Lcd.drawFloat(Data2.speed, 2, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 1.5); 
  M5.Lcd.drawFloat(Data2.heading, 2, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 2.5); 
  M5.Lcd.drawFloat(Data2.latitude, 6, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 3.5); 
  M5.Lcd.drawFloat(Data2.longitude, 6, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 4.5);

  return;
}

void display_screen_3() {
  // Update screen flag
  currentScreen = SCREEN_3;
  
  // Fill screen
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextDatum(MC_DATUM);

  // Draw the boundary lines
  draw_boundary();
  draw_table();

  // Identify Vessel
  M5.Lcd.drawString("Vessel 3", SCREEN_WIDTH/2, BUTTON_HEIGHT/2, 4);

  // Set Datum
  M5.Lcd.setTextDatum(ML_DATUM);

  // Draw numbers
  M5.Lcd.drawNumber(Data3.mmsi, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 0.5); 
  M5.Lcd.drawFloat(Data3.speed, 2, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 1.5); 
  M5.Lcd.drawFloat(Data3.heading, 2, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 2.5); 
  M5.Lcd.drawFloat(Data3.latitude, 6, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 3.5); 
  M5.Lcd.drawFloat(Data3.longitude, 6, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 4.5);

  return;
}

void display_screen_4() {
  // Update screen flag
  currentScreen = SCREEN_4;
  
  // Fill screen
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextDatum(MC_DATUM);

  // Draw the boundary lines
  draw_boundary();
  draw_table();

  // Identify Vessel
  M5.Lcd.drawString("Vessel 4", SCREEN_WIDTH/2, BUTTON_HEIGHT/2, 4);

  // Set Datum
  M5.Lcd.setTextDatum(ML_DATUM);

  // Draw numbers
  M5.Lcd.drawNumber(Data4.mmsi, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 0.5); 
  M5.Lcd.drawFloat(Data4.speed, 2, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 1.5); 
  M5.Lcd.drawFloat(Data4.heading, 2, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 2.5); 
  M5.Lcd.drawFloat(Data4.latitude, 6, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 3.5); 
  M5.Lcd.drawFloat(Data4.longitude, 6, SCREEN_WIDTH/2, VESSEL_SCREEN_TITLE_INDENT + VESSEL_SCREEN_DATA_SPACING * 4.5);

  return;
}

// --------------------------------- BLE HANDLING ---------------------------------
void bleCentralDiscoverHandler(BLEDevice peripheral) {
  // Check if there is peripheral data
  if (peripheral.hasManufacturerData()) {
    // Manufacturing Data Length
    int manuDataLen = peripheral.manufacturerDataLength();
    uint8_t manuDataBuffer[manuDataLen];

    // Fill the buffer for the manufacturer data if relevant
    if (peripheral.manufacturerData(manuDataBuffer, manuDataLen)) {
      // Unpack manufacturing data here
      unpack_manufacturing_data(manuDataBuffer, manuDataLen);
    }
  }
}

void unpack_manufacturing_data(uint8_t *manuDataBuffer, int manuDataLen) {
  // Identify the position and populate the class
  uint8_t pos = manuDataBuffer[0];
  populate_class(manuDataBuffer, manuDataLen, pos);  
}

void populate_class(uint8_t *manuDataBuffer, int manuDataLen, int dataPos) {
  // Initialise all values
  long long mmsi = 0;
  float speed = 0;
  float heading = 0;
  double latitude = 0;
  double longitude = 0;

  // Initialise different values from the data buffer
  mmsi = char_to_number(manuDataBuffer[1]) * pow(100, 4) + char_to_number(manuDataBuffer[2]) * pow(100, 3) + 
          char_to_number(manuDataBuffer[3]) * pow(100, 2) + char_to_number(manuDataBuffer[4]) * pow(100, 1) + 
          char_to_number(manuDataBuffer[5]);

  speed = char_to_number(manuDataBuffer[6]) * pow(100, 1) + char_to_number(manuDataBuffer[7]) + 
            char_to_number(manuDataBuffer[8]) * pow(100, -1);

  heading = char_to_number(manuDataBuffer[9]) * pow(100, 1) + char_to_number(manuDataBuffer[10]) + 
            char_to_number(manuDataBuffer[11]) * pow(100, -1);

  latitude = char_to_number(manuDataBuffer[12] & 0x7F) * pow(100, 1) + char_to_number(manuDataBuffer[13]) + 
            char_to_number(manuDataBuffer[14]) * pow(100, -1) + char_to_number(manuDataBuffer[15]) * pow(100, -2) +
            char_to_number(manuDataBuffer[16]) * pow(100, -3);

  longitude = char_to_number(manuDataBuffer[17] & 0x7F) * pow(100, 1) + char_to_number(manuDataBuffer[18]) + 
            char_to_number(manuDataBuffer[19]) * pow(100, -1) + char_to_number(manuDataBuffer[20]) * pow(100, -2) +
            char_to_number(manuDataBuffer[21]) * pow(100, -3);

  if (manuDataBuffer[12] & 0x80) {
    latitude = -latitude;
  }

  if (manuDataBuffer[17] & 0x80) {
    longitude = -longitude;
  }

  switch (dataPos) {
    case (SCREEN_1):
      // Check if the data is the exact same
      if (Data1.mmsi == mmsi && Data1.speed == speed &&
            Data1.heading == heading && Data1.latitude == latitude &&
            Data1.longitude == longitude) {
        return;
      } else {
        // If not the same, update
        Data1.mmsi = mmsi;
        Data1.speed = speed;
        Data1.heading = heading;
        Data1.latitude = latitude;
        Data1.longitude = longitude;

        if (currentScreen == SCREEN_1) {
          // If the current data is active, update the screen
          display_screen_1();
        }
      }  

      break;

    case (SCREEN_2):
      // Check if the data is the exact same
      if (Data2.mmsi == mmsi && Data2.speed == speed &&
            Data2.heading == heading && Data2.latitude == latitude &&
            Data2.longitude == longitude) {
        return;
      } else {
        // If not the same, update
        Data2.mmsi = mmsi;
        Data2.speed = speed;
        Data2.heading = heading;
        Data2.latitude = latitude;
        Data2.longitude = longitude;

        if (currentScreen == SCREEN_2) {
          // If the current data is active, update the screen
          display_screen_2();
        }
      }    

      break;

    case (SCREEN_3):
      // Check if the data is the exact same
      if (Data3.mmsi == mmsi && Data3.speed == speed &&
            Data3.heading == heading && Data3.latitude == latitude &&
            Data3.longitude == longitude) {
        return;
      } else {
        // If not the same, update
        Data3.mmsi = mmsi;
        Data3.speed = speed;
        Data3.heading = heading;
        Data3.latitude = latitude;
        Data3.longitude = longitude;

        if (currentScreen == SCREEN_3) {
          // If the current data is active, update the screen
          display_screen_3();
        }
      }   

      break;

    case (SCREEN_4):
      // Check if the data is the exact same
      if (Data4.mmsi == mmsi && Data4.speed == speed &&
            Data4.heading == heading && Data4.latitude == latitude &&
            Data4.longitude == longitude) {
        return;
      } else {
        // If not the same, update
        Data4.mmsi = mmsi;
        Data4.speed = speed;
        Data4.heading = heading;
        Data4.latitude = latitude;
        Data4.longitude = longitude;

        if (currentScreen == SCREEN_4) {
          // If the current data is active, update the screen
          display_screen_4();
        }
      }   

      break;

    default:
      break;
  }

  return;
}

void init_data_structs() {
  Data1 = DATA_INIT();
  Data2 = DATA_INIT();
  Data3 = DATA_INIT();
  Data4 = DATA_INIT();
}

long double char_to_number(uint8_t data) {
  return (long double)data;
}
