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
Button vessel_1(0, 0, 160, 100, "vessel_1");
Button vessel_2(160, 0, 160, 100, "vessel_2");
Button vessel_3(0, 100, 160, 100, "vessel_3");
Button vessel_4(160, 100, 160, 100, "vessel_4");

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
  BLE.scanForName("test", true);

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
    } else if (b.x == 160 && b.y == 0) {
      display_screen_2();
    } else if (b.x == 0 && b.y == 100) {
      display_screen_3();
    } else if (b.x == 160 && b.y == 100) {
      display_screen_4();
    }
  }
  

}

void draw_boundary() {
  // Draw the boundary lines
  M5.Lcd.drawFastVLine(0, 0, 240, WHITE);
  M5.Lcd.drawFastHLine(0, 0, 320, WHITE);
  M5.Lcd.drawFastVLine(319, 0, 240, WHITE);
  M5.Lcd.drawFastHLine(0, 239, 320, WHITE);
  return;
}

void draw_table() {
  // Horizontal Lines
  M5.Lcd.drawFastHLine(0, 90, 320, WHITE);
  M5.Lcd.drawFastHLine(0, 120, 320, WHITE);
  M5.Lcd.drawFastHLine(0, 150, 320, WHITE);
  M5.Lcd.drawFastHLine(0, 180, 320, WHITE);
  M5.Lcd.drawFastHLine(0, 210, 320, WHITE);

  // Vertical Lines
  M5.Lcd.drawFastVLine(140, 90, 150, WHITE);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextDatum(MC_DATUM);

  // Names
  M5.Lcd.drawString("MMSI", 70, 90+15, 1);
  M5.Lcd.drawString("Speed", 70, 120+15, 1);
  M5.Lcd.drawString("Heading", 70, 150+15, 1);
  M5.Lcd.drawString("Latitude", 70, 180+15, 1);
  M5.Lcd.drawString("Longitude", 70, 210+15, 1);
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
  M5.Lcd.drawString("1", 80, 50, 4);

  // Top right cursor
  M5.Lcd.drawString("2", 240, 50, 4);

  // Bottom left cursor
  M5.Lcd.drawString("3", 80, 150, 4);

  // Bottom right cursor
  M5.Lcd.drawString("4", 240, 150, 4);

  // Write tha main vessel information
  M5.Lcd.drawString("All Vessels", 160, 220, 2);

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
  M5.Lcd.drawString("Vessel 1", 160, 50, 4);

  // Set Datum
  M5.Lcd.setTextDatum(ML_DATUM);

  // Draw numbers
  // Should probably ENUM these MAGIC NUMBERS
  M5.Lcd.drawNumber(Data1.mmsi, 160, 90+15); // Add value here
  M5.Lcd.drawFloat(Data1.speed, 2, 160, 120+15); // Add value here
  M5.Lcd.drawFloat(Data1.heading, 2, 160, 150+15); // Add value here
  M5.Lcd.drawFloat(Data1.latitude, 6, 160, 180+15); // Add value here
  M5.Lcd.drawFloat(Data1.longitude, 6, 160, 210+15); // Add value here

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
  M5.Lcd.drawString("Vessel 2", 160, 50, 4);

  // Set Datum
  M5.Lcd.setTextDatum(ML_DATUM);

  // Draw numbers
  // Should probably ENUM these MAGIC NUMBERS
  M5.Lcd.drawNumber(Data2.mmsi, 160, 90+15); // Add value here
  M5.Lcd.drawFloat(Data2.speed, 2, 160, 120+15); // Add value here
  M5.Lcd.drawFloat(Data2.heading, 2, 160, 150+15); // Add value here
  M5.Lcd.drawFloat(Data2.latitude, 6, 160, 180+15); // Add value here
  M5.Lcd.drawFloat(Data2.longitude, 6, 160, 210+15); // Add value here

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
  M5.Lcd.drawString("Vessel 3", 160, 50, 4);

  // Set Datum
  M5.Lcd.setTextDatum(ML_DATUM);

  // Draw numbers
  // Should probably ENUM these MAGIC NUMBERS
  M5.Lcd.drawNumber(Data3.mmsi, 160, 90+15); // Add value here
  M5.Lcd.drawFloat(Data3.speed, 2, 160, 120+15); // Add value here
  M5.Lcd.drawFloat(Data3.heading, 2, 160, 150+15); // Add value here
  M5.Lcd.drawFloat(Data3.latitude, 6, 160, 180+15); // Add value here
  M5.Lcd.drawFloat(Data3.longitude, 6, 160, 210+15); // Add value here

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
  M5.Lcd.drawString("Vessel 4", 160, 50, 4);

  // Set Datum
  M5.Lcd.setTextDatum(ML_DATUM);

  // Draw numbers
  // Should probably ENUM these MAGIC NUMBERS
  M5.Lcd.drawNumber(Data4.mmsi, 160, 90+15); // Add value here
  M5.Lcd.drawFloat(Data4.speed, 2, 160, 120+15); // Add value here
  M5.Lcd.drawFloat(Data4.heading, 2, 160, 150+15); // Add value here
  M5.Lcd.drawFloat(Data4.latitude, 6, 160, 180+15); // Add value here
  M5.Lcd.drawFloat(Data4.longitude, 6, 160, 210+15); // Add value here

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
  
  latitude = char_to_number(manuDataBuffer[12]) * pow(100, 1) + char_to_number(manuDataBuffer[13]) + 
            char_to_number(manuDataBuffer[14]) * pow(100, -1) + char_to_number(manuDataBuffer[15]) * pow(100, -2) +
            char_to_number(manuDataBuffer[16]) * pow(100, -3);

  longitude = char_to_number(manuDataBuffer[17]) * pow(100, 1) + char_to_number(manuDataBuffer[18]) + 
            char_to_number(manuDataBuffer[19]) * pow(100, -1) + char_to_number(manuDataBuffer[20]) * pow(100, -2) +
            char_to_number(manuDataBuffer[21]) * pow(100, -3);

  // Update all values here, 
  // Maybe only update display if there has been a change, and if the screen is on the data
  int updateDisplayFlag = 0;

  switch (dataPos) {
    case (1):
      if (currentScreen == SCREEN_1) {
        if (Data1.mmsi == mmsi && Data1.speed == speed &&
            Data1.heading == heading && Data1.latitude == latitude &&
            Data1.longitude == longitude) {
          updateDisplayFlag = 1;
        }
      }
      
      Data1.mmsi = mmsi;
      Data1.speed = speed;
      Data1.heading = heading;
      Data1.latitude = latitude;
      Data1.longitude = longitude;

      if (updateDisplayFlag) {
        display_screen_1();
      }      

      break;

    case (2):
      if (currentScreen == SCREEN_2) {
        if (Data2.mmsi == mmsi && Data2.speed == speed &&
            Data2.heading == heading && Data2.latitude == latitude &&
            Data2.longitude == longitude) {
          updateDisplayFlag = 1;
        }
      }

      Data2.mmsi = mmsi;
      Data2.speed = speed;
      Data2.heading = heading;
      Data2.latitude = latitude;
      Data2.longitude = longitude;

      if (updateDisplayFlag) {
        display_screen_2();
      }    

      break;

    case (3):
      if (currentScreen == SCREEN_3) {
        if (Data3.mmsi == mmsi && Data3.speed == speed &&
            Data3.heading == heading && Data3.latitude == latitude &&
            Data3.longitude == longitude) {
          updateDisplayFlag = 1;
        }
      }

      Data3.mmsi = mmsi;
      Data3.speed = speed;
      Data3.heading = heading;
      Data3.latitude = latitude;
      Data3.longitude = longitude;

      if (updateDisplayFlag) {
        display_screen_3();
      }    

      break;

    case (4):
      if (currentScreen == SCREEN_4) {
        if (Data4.mmsi == mmsi && Data4.speed == speed &&
            Data4.heading == heading && Data4.latitude == latitude &&
            Data4.longitude == longitude) {
          updateDisplayFlag = 1;
        }
      }

      Data4.mmsi = mmsi;
      Data4.speed = speed;
      Data4.heading = heading;
      Data4.latitude = latitude;
      Data4.longitude = longitude;

      if (updateDisplayFlag) {
        display_screen_4();
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
