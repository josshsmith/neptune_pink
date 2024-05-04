#include <M5Core2.h>  
#include <ArduinoBLE.h>

class Vessel {
public:
    // Constructor
    MyObject(int mmsi_init) {
        mmsi = mmsi_init;
    }

    float lat;
    float lon;
    float vel;
    float heading;
    char* name;

private:
    int mmsi;
};

void setup() {
  M5.begin();

  // begin initialization
  if (!BLE.begin()) {
    M5.Lcd.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  M5.Lcd.println("Bluetooth® Low Energy Central scan callback");

  // set the discovered event handle
  BLE.setEventHandler(BLEDiscovered, bleCentralDiscoverHandler);

  // start scanning for peripherals with duplicates
  BLE.scanForName("Data Collection", true);
}

void loop() {
  // poll the central for events
  BLE.poll();
  delay(100);
}

void bleCentralDiscoverHandler(BLEDevice peripheral) {

  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(0, 0); 
  // discovered a peripheral
  M5.Lcd.println("Discovered Our Device");
  M5.Lcd.println("-----------------------");

  // print address
  M5.Lcd.print("Address: ");
  M5.Lcd.println(peripheral.address());

  // print the local name, if present
  if (peripheral.hasLocalName()) {
    M5.Lcd.print("Local Name: ");
    M5.Lcd.println(peripheral.localName());
  }

  // print the RSSI
  M5.Lcd.print("RSSI: ");
  M5.Lcd.println(peripheral.rssi());

  if (peripheral.hasManufacturerData()) {
    int ManuDataLen = peripheral.manufacturerDataLength();
      M5.Lcd.println(ManuDataLen);
      // retrieve the data.... using manufacturerData etc.
      uint8_t manuDataBuffer[ManuDataLen];
      if (peripheral.manufacturerData(manuDataBuffer, ManuDataLen)) {
        M5.Lcd.print("Manufacturer Data: ");
        for (int x = 0; x < ManuDataLen; x++) {
          M5.Lcd.print(manuDataBuffer[x], HEX);
        }
        M5.Lcd.println();
    }
  }
}