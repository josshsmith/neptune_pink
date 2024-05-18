/*
 * ble_sensor_send.c File
 * Connor Davis - 46990275
 *
 * A file for to send sensor values from Thingy52
 */

#include <bt_ahu.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_stdint.h>
#include <uart_rec.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

#ifndef IBEACON_RSSI
#define IBEACON_RSSI 0xc8
#endif
/*
 * RSSI:  -56 dBm
 */

#define SEND_PERIOD 500
#define MAX_NUMBER_OF_VESSELS 4
#define MAX_DATA 25

/* New Protocol data positions */
extern struct vessel_data* vessel_data_array;

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* iBeacon Protocol data packet */
static uint8_t vessel_data_packet1[MAX_DATA] = {
    0x01, 0x00,                                /* Apple */
    0x02, 0x15,                                /* iBeacon */
    0xEF, 0x68, 0x02,        0xFF,             /* UUID[15..12] */
    0x9B, 0x35,                                /* UUID[11..10] */
    0x49, 0x33,                                /* UUID[9..8] */
    0x9B, 0x10,                                /* UUID[7..6] */
    0x52, 0xFF, 0xA9,        0x74, 0x00, 0x42, /* UUID[5..0] */
    0xFF, 0xFF,                                /* Major */
    0x00, 0x00, IBEACON_RSSI};
static uint8_t vessel_data_packet2[MAX_DATA] = {
    0x02, 0x00,                                /* Apple */
    0x02, 0x15,                                /* iBeacon */
    0xEF, 0x68, 0x02,        0xFF,             /* UUID[15..12] */
    0x9B, 0x35,                                /* UUID[11..10] */
    0x49, 0x33,                                /* UUID[9..8] */
    0x9B, 0x10,                                /* UUID[7..6] */
    0x52, 0xFF, 0xA9,        0x74, 0x00, 0x42, /* UUID[5..0] */
    0xFF, 0xFF,                                /* Major */
    0x00, 0x00, IBEACON_RSSI};
static uint8_t vessel_data_packet3[MAX_DATA] = {
    0x03, 0x00,                                /* Apple */
    0x02, 0x15,                                /* iBeacon */
    0xEF, 0x68, 0x02,        0xFF,             /* UUID[15..12] */
    0x9B, 0x35,                                /* UUID[11..10] */
    0x49, 0x33,                                /* UUID[9..8] */
    0x9B, 0x10,                                /* UUID[7..6] */
    0x52, 0xFF, 0xA9,        0x74, 0x00, 0x42, /* UUID[5..0] */
    0xFF, 0xFF,                                /* Major */
    0x00, 0x00, IBEACON_RSSI};
static uint8_t vessel_data_packet4[MAX_DATA] = {
    0x04, 0x00,                                /* Apple */
    0x02, 0x15,                                /* iBeacon */
    0xEF, 0x68, 0x02,        0xFF,             /* UUID[15..12] */
    0x9B, 0x35,                                /* UUID[11..10] */
    0x49, 0x33,                                /* UUID[9..8] */
    0x9B, 0x10,                                /* UUID[7..6] */
    0x52, 0xFF, 0xA9,        0x74, 0x00, 0x42, /* UUID[5..0] */
    0xFF, 0xFF,                                /* Major */
    0x00, 0x00, IBEACON_RSSI};

static uint8_t* vessel_data_packets[MAX_NUMBER_OF_VESSELS];

static struct bt_data* vessel_data_sends[4];

/* The generic bluetooth data to send */
struct bt_data vessel_data_send1[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, vessel_data_packet1,
            25), /* Calibrated RSSI @ 1m */
};
/* The generic bluetooth data to send */
struct bt_data vessel_data_send2[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, vessel_data_packet2,
            25), /* Calibrated RSSI @ 1m */
};
/* The generic bluetooth data to send */
struct bt_data vessel_data_send3[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, vessel_data_packet3,
            25), /* Calibrated RSSI @ 1m */
};
/* The generic bluetooth data to send */
struct bt_data vessel_data_send4[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, vessel_data_packet4,
            25), /* Calibrated RSSI @ 1m */
};

void print_array(uint8_t *arr, size_t size) {
    printk("Array: ");
    for (size_t i = 0; i < size; i++) {
        printk("0x%02X ", arr[i]);
    }
    printk("$");
}

void speed_heading(struct vessel_data rec_info) {

  int index = atoi(rec_info.position);
  double speed = atof(rec_info.speed);

  vessel_data_packets[index][6] = (long)(speed / 100) % 100;
  vessel_data_packets[index][7] = (long)(speed) % 100;
  vessel_data_packets[index][8] = (long)(speed * 100) % 100;


  double heading = atof(rec_info.heading);
  vessel_data_packets[index][9] = (long)(heading / 100) % 100;
  vessel_data_packets[index][10] = (long)(heading) % 100;
  vessel_data_packets[index][11] = (long)(heading * 100) % 100;

}

void lat_long(struct vessel_data rec_info) {
  int index = atoi(rec_info.position);
  double lat = atof(rec_info.latitude);

  if (lat < 0) {
    lat = -lat;
    vessel_data_packets[index][12] = 0x80;
  } else {
    vessel_data_packets[index][12] = 0x00;
  }
  vessel_data_packets[index][12] |= (long)(lat / 100) % 100;
  vessel_data_packets[index][13] = (long)(lat) % 100;
  vessel_data_packets[index][14] = (long)(lat * 100) % 100;
  vessel_data_packets[index][15] = (long)(lat * 10000) % 100;
  vessel_data_packets[index][16] = (long)(lat * 1000000) % 100;

  double longitude = atof(rec_info.longitude);

  if (longitude < 0) {
    longitude = -longitude;
    vessel_data_packets[index][17] = 0x80;
  } else {
    vessel_data_packets[index][17] = 0x00;
  }
  vessel_data_packets[index][17] |= (long)(longitude / 100) % 100;
  vessel_data_packets[index][18] = (long)(longitude) % 100;
  vessel_data_packets[index][19] = (long)(longitude * 100) % 100;
  vessel_data_packets[index][20] = (long)(longitude * 10000) % 100;
  vessel_data_packets[index][21] = (long)(longitude * 1000000) % 100;


}

void mmsi_alter(struct vessel_data rec_info) {
  char* endptr;
  int index = atoi(rec_info.position);
  long long mmsi = strtol(rec_info.mmsi, &endptr, 10);
  vessel_data_packets[index][5] = mmsi % 100;
  vessel_data_packets[index][4] = (mmsi / 100) % 100;
  vessel_data_packets[index][3] = (mmsi / 10000) % 100;
  vessel_data_packets[index][2] = (mmsi / 1000000) % 100;
  vessel_data_packets[index][1] = (mmsi / 100000000) % 100;
}

void alter_packet(struct vessel_data rec_info) {
  int8_t index = (uint8_t)atoi(rec_info.position);
  if (index < 0 && index > 4) {
    return;
  }
  mmsi_alter(rec_info);
  speed_heading(rec_info);
  lat_long(rec_info);
  print_array(vessel_data_packets[index], 25);
  
    
}



/*
 * int bluetooth_thread(void)
 *
 * Handles the bluetooth thread that initialise the bluetooth and LED systems
 * and then goes into a continuous loops of:
 * 1. Receive sensor data from the sensor queue,
 * 2. Update the data packet to be sent
 * 3. Update the data that is sent via bluetooth to the new data packet
 *
 * params:
 * returns:
 */
void bt_task(void* p1, void* p2, void* p3) {
  /* Initialise bluetooth requirements */
  int err = bt_enable(NULL);
  // if (err) {
  //   // printk("Bluetooth init failed (err %d)\n", err);
  //   return 0;
  // }

  /* Allow all items to initialise*/
  k_msleep(SEND_PERIOD);

  /* Initialise and toggle the LED */
  // if (!gpio_is_ready_dt(&led)) {
  //   return 0;
  // }

  gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  gpio_pin_toggle_dt(&led);

  vessel_data_packets[0] = vessel_data_packet1;
  vessel_data_packets[1] = vessel_data_packet2;
  vessel_data_packets[2] = vessel_data_packet3;
  vessel_data_packets[3] = vessel_data_packet4;


  vessel_data_sends[0] = vessel_data_send1;
  vessel_data_sends[1] = vessel_data_send2;
  vessel_data_sends[2] = vessel_data_send3;
  vessel_data_sends[3] = vessel_data_send4;

  /* Start a generic, connectible, scannable bluetooth advertising */
  /* Create a non-connectable non-scannable advertising set */
  err = bt_le_adv_start(BT_LE_ADV_NCONN_NAME, vessel_data_send1,
                        ARRAY_SIZE(vessel_data_send1), NULL, 0);

  printk("Bluetooth Successfully Initialised\n");

  /* Enter the infinite loop after initialising*/
 
  while (true) {
    for (int i = 0; i < 4; i++) {
      switch(i) {
        case 0:
          bt_le_adv_update_data(vessel_data_send1, ARRAY_SIZE(vessel_data_send1), NULL, 0);
          break;
        case 1:
          bt_le_adv_update_data(vessel_data_send2, ARRAY_SIZE(vessel_data_send2), NULL, 0);
          break;
        case 2:
          bt_le_adv_update_data(vessel_data_send3, ARRAY_SIZE(vessel_data_send3), NULL, 0);
          break;
        case 3:
          bt_le_adv_update_data(vessel_data_send4, ARRAY_SIZE(vessel_data_send4), NULL, 0);
          break;
        default:
          break;


      }
      //bt_le_adv_update_data(vessel_data_sends[i], ARRAY_SIZE(vessel_data_sends[i]), NULL, 0);
      k_msleep(SEND_PERIOD);
    }
  }

  /* Should never reach */
  return;
}


void alter_data_task(void* p1, void* p2, void* p3) {
   struct vessel_data rec_info;
  while (1) {
    if (!k_msgq_get(&bt_msgq, &rec_info, K_NO_WAIT)) {
        alter_packet(rec_info);
        gpio_pin_toggle_dt(&led);
      }
    k_msleep(50);
  }
  
}
