#ifndef BLE_NRF_ADVERTISING_H_ /* Include guard */
#define BLE_NRF_ADVERTISING_H_

#include <stdbool.h>

// Initialise a struct of the the vessel data 
struct vessel_data {
  // TO BE DONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  int mmsi;
  char* vessel_name;
  char* call_sign;
  char* update_time;
  int latitude;
  int longitude;
  int speed;
  int heading;
};

struct test_data {
  int mmsi;
};

int init_read_thread();

#endif // BLE_NRF_ADVERTISING_H_