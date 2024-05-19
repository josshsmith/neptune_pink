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
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

#define ALL_MODULES 4

#define BT_CONNECTION_TYPE                                           \
  BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_NAME, BT_GAP_ADV_FAST_INT_MIN_2, \
                  BT_GAP_ADV_FAST_INT_MAX_2, NULL)

LOG_MODULE_DECLARE(AHU, ALL_MODULES);

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* BLE data packets */
static uint8_t vessel_data_packet1[MAX_DATA] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t vessel_data_packet2[MAX_DATA] = {
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t vessel_data_packet3[MAX_DATA] = {
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t vessel_data_packet4[MAX_DATA] = {
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// array to hold data packets
static uint8_t* vessel_data_packets[MAX_NUMBER_OF_VESSELS];

/* The generic bluetooth data to send */
struct bt_data vessel_data_send1[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, vessel_data_packet1, MAX_DATA),
};

struct bt_data vessel_data_send2[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, vessel_data_packet2, MAX_DATA),
};

struct bt_data vessel_data_send3[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, vessel_data_packet3, MAX_DATA),
};

struct bt_data vessel_data_send4[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, vessel_data_packet4, MAX_DATA),
};

/**
 * void speed_heading(struct vessel_data rec_info)
 * -----------------------------------------------
 * Update appropriate data packets with speed and heading information
 *
 * input: struct vessel_data rec_info - struct containing all relevant
 * information about a single watercraft
 */
void speed_heading(struct vessel_data rec_info) {
  int index = atoi(rec_info.position) - 1;
  double speed = atof(rec_info.speed);

  // filling first two bytes with the whole values of the float
  vessel_data_packets[index][6] = (long)(speed / 100) % 100;
  vessel_data_packets[index][7] = (long)(speed) % 100;
  // last byte with decimal values of the float
  vessel_data_packets[index][8] = (long)(speed * 100) % 100;

  double heading = atof(rec_info.heading);
  // filling first two bytes with the whole values of the float
  vessel_data_packets[index][9] = (long)(heading / 100) % 100;
  vessel_data_packets[index][10] = (long)(heading) % 100;
  // last byte with decimal values of the float
  vessel_data_packets[index][11] = (long)(heading * 100) % 100;
}

/**
 * void lat_long(struct vessel_data rec_info)
 * -----------------------------------------------
 * Update appropriate data packets with latitude and longitude information
 *
 * input: struct vessel_data rec_info - struct containing all relevant
 * information about a single watercraft
 */
void lat_long(struct vessel_data rec_info) {
  int index = atoi(rec_info.position) - 1;
  double lat = atof(rec_info.latitude);

  if (lat < 0) {  // if negative we set MSB
    lat = -lat;
    vessel_data_packets[index][12] = 0x80;
  } else {
    vessel_data_packets[index][12] = 0x00;
  }
  // filling two bytes with whole number
  vessel_data_packets[index][12] |= (long)(lat / 100) % 100;
  vessel_data_packets[index][13] = (long)(lat) % 100;
  // three bytes for decimal values
  vessel_data_packets[index][14] = (long)(lat * 100) % 100;
  vessel_data_packets[index][15] = (long)(lat * 10000) % 100;
  vessel_data_packets[index][16] = (long)(lat * 1000000) % 100;

  double longitude = atof(rec_info.longitude);

  if (longitude < 0) {  // if negative we set MSB
    longitude = -longitude;
    vessel_data_packets[index][17] = 0x80;
  } else {
    vessel_data_packets[index][17] = 0x00;
  }
  // filling two bytes with whole number
  vessel_data_packets[index][17] |= (long)(longitude / 100) % 100;
  vessel_data_packets[index][18] = (long)(longitude) % 100;
  // three bytes for decimal values
  vessel_data_packets[index][19] = (long)(longitude * 100) % 100;
  vessel_data_packets[index][20] = (long)(longitude * 10000) % 100;
  vessel_data_packets[index][21] = (long)(longitude * 1000000) % 100;
}

/**
 * void mmsi_alter(struct vessel_data rec_info)
 * -----------------------------------------------
 * Update appropriate data packets with mmsi
 *
 * input: struct vessel_data rec_info - struct containing all relevant
 * information about a single watercraft
 */
void mmsi_alter(struct vessel_data rec_info) {
  char* endptr;
  int index = atoi(rec_info.position) - 1;
  long long mmsi = strtol(rec_info.mmsi, &endptr, 10);
  // putting two digits in each byte
  vessel_data_packets[index][5] = mmsi % 100;
  vessel_data_packets[index][4] = (mmsi / 100) % 100;
  vessel_data_packets[index][3] = (mmsi / 10000) % 100;
  vessel_data_packets[index][2] = (mmsi / 1000000) % 100;
  vessel_data_packets[index][1] = (mmsi / 100000000) % 100;
}

/**
 * void alter_packet(struct vessel_data rec_info)
 * -----------------------------------------------
 * Update appropriate data packets
 *
 * input: struct vessel_data rec_info - struct containing all relevant
 * information about a single watercraft
 */
void alter_packet(struct vessel_data rec_info) {
  int8_t index = (uint8_t)atoi(rec_info.position);
  if (index < 1 && index > 4) {
    LOG_ERR("INDEX ERROR : %d", index);
    return;
  }
  mmsi_alter(rec_info);
  speed_heading(rec_info);
  lat_long(rec_info);
}

/*
 * int bluetooth_thread(void)
 *
 * Handles the bluetooth thread that initialise the bluetooth and LED systems
 * and then goes into a continuous loops of:
 * 1. Receive sensor data from the sensor queue,
 * 2. Update the data packet to be sent
 * 3. Update the data that is sent via bluetooth to the new data packet
 */
void bt_task(void* p1, void* p2, void* p3) {
  /* Initialise bluetooth requirements */
  int err;
  err = bt_enable(NULL);
  if (err) {
    LOG_ERR("ERROR IN BT ENABLE: %d", err);
    return;
  }

  /* Allow all items to initialise*/
  k_msleep(SEND_PERIOD);

  err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (err) {
    LOG_ERR("ERROR IN LED ENABLE: %d", err);
    return;
  }
  gpio_pin_toggle_dt(&led);

  // fill array with data packets
  vessel_data_packets[0] = vessel_data_packet1;
  vessel_data_packets[1] = vessel_data_packet2;
  vessel_data_packets[2] = vessel_data_packet3;
  vessel_data_packets[3] = vessel_data_packet4;

  /* Start a generic, connectible, scannable bluetooth advertising */
  /* Create a non-connectable non-scannable advertising set */
  err = bt_le_adv_start(BT_CONNECTION_TYPE, vessel_data_send1,
                        ARRAY_SIZE(vessel_data_send1), NULL, 0);
  if (err) {
    LOG_ERR("ERROR IN BT ADVERTISING ENABLE: %d", err);
    return;
  }

  /* Enter the infinite loop after initialising*/
  LOG_INF("Beginning bt loop");
  while (true) {
    for (int i = 0; i < 4; i++) {
      switch (i) {
        // alternating which packet is updated
        case 0:
          err = bt_le_adv_update_data(vessel_data_send1,
                                      ARRAY_SIZE(vessel_data_send1), NULL, 0);
          break;
        case 1:
          err = bt_le_adv_update_data(vessel_data_send2,
                                      ARRAY_SIZE(vessel_data_send2), NULL, 0);
          break;
        case 2:
          err = bt_le_adv_update_data(vessel_data_send3,
                                      ARRAY_SIZE(vessel_data_send3), NULL, 0);
          break;
        case 3:
          err = bt_le_adv_update_data(vessel_data_send4,
                                      ARRAY_SIZE(vessel_data_send4), NULL, 0);
          break;
        default:
          break;
      }
      if (err) {
        LOG_ERR("ERROR IN BT UPDATING: %d at index: %d", err, i);
        return;
      }
      k_msleep(SEND_PERIOD);
    }
  }

  /* Should never reach */
  return;
}

void print_array(uint8_t arr[]) {
  LOG_INF("[ ");
  for (int i = 0; i < 25; i++) {
    printf("%u ", arr[i]);
  }
  LOG_INF("]\n");
}

void alter_data_task(void* p1, void* p2, void* p3) {
  struct vessel_data rec_info;
  while (1) {
    if (!k_msgq_get(&bt_msgq, &rec_info, K_NO_WAIT)) {
      alter_packet(rec_info);
      gpio_pin_toggle_dt(&led);
      print_array(vessel_data_packet1);
    }
    k_msleep(50);
  }
}
