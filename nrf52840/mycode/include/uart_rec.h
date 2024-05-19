#ifndef UART_REC_H
#define UART_REC_H

#include <stdbool.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#define UART_PRIORITY 7

void serial_cb(const struct device *dev, void *user_data);
void uart_task(void *p1, void *p2, void *p3);

// Initialise a struct of the the vessel data
struct vessel_data {
  // TO BE DONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  char *mmsi;
  char *vessel_name;
  char *call_sign;
  char *update_time;
  char *latitude;
  char *longitude;
  char *speed;
  char *heading;
  char *position;
};

extern struct k_msgq bt_msgq;

#define POLL_INTERVAL_MS 20
#define ALL_MODULES 4
#define MSG_SIZE 512

#endif
