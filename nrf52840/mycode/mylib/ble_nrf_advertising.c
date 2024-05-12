/*
 * ble_sensor_send.c File
 * Connor Davis - 46990275
 *
 * A file for to send sensor values from Thingy52
 */

#include <stddef.h>
#include <stdio.h>

#include <sys/_stdint.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

#include <zephyr/drivers/gpio.h>
#include "json_nrf_receive.h"

#ifndef IBEACON_RSSI
#define IBEACON_RSSI 0xc8
#endif
/*
 * RSSI:  -56 dBm
 */

#define SEND_PERIOD 200
#define MAX_NUMBER_OF_VESSELS 5

/* New Protocol data positions */
extern struct vessel_data *vessel_data_array;

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* iBeacon Protocol data packet */
static uint8_t vessel_data_packet[128] = {
    0x4c,        0x00,                         /* Apple */
    0x02,        0x15,                         /* iBeacon */
    0xEF,        0x68, 0x02, 0xFF,             /* UUID[15..12] */
    0x9B,        0x35,                         /* UUID[11..10] */
    0x49,        0x33,                         /* UUID[9..8] */
    0x9B,        0x10,                         /* UUID[7..6] */
    0x52,        0xFF, 0xA9, 0x74, 0x00, 0x42, /* UUID[5..0] */
    0xFF,        0xFF,                         /* Major */
    0x00,        0x00,                         /* Minor */
    IBEACON_RSSI};

/* The generic bluetooth data to send */
struct bt_data vessel_data[2] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, vessel_data_packet,
            25), /* Calibrated RSSI @ 1m */
};

/* Function used to update the vessel_data_packet to the vessel data contained
 * within a global array. The index is the one supplied
 */
static int update_vessel_packet(int index) {
    // Retrieve relevant data to send
    struct vessel_data data = vessel_data_array[index];

    // Set generic data packet structure
    
    return 0;
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
int ble_nrf_adv_thread(void) {
  /* Initialise bluetooth requirements */
  int err = bt_enable(NULL);
  if (err) {
    // printk("Bluetooth init failed (err %d)\n", err);
    return 0;
  }

  /* Allow all items to initialise*/
  k_msleep(SEND_PERIOD);

  /* Initialise and toggle the LED */
  if (!gpio_is_ready_dt(&led)) {
    return 0;
  }

  gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  gpio_pin_toggle_dt(&led);

  /* Start a generic, connectible, scannable bluetooth advertising */
  err = bt_le_adv_start(
      BT_LE_ADV_NCONN_NAME, vessel_data, ARRAY_SIZE(vessel_data), NULL,
      0);
  if (err) {
    printk("Failed to create advertising set (err %d)\n", err);
    return err;
  }

  printk("Bluetooth Successfully Initialised\n");

  /* Enter the infinite loop after initialising*/
  while (true) {
    // Iterate through the 5 closest vessels
    for (int index = 0; index < MAX_NUMBER_OF_VESSELS; index++) {
        /* Sleep to give an opportunity to switch */
        k_msleep(SEND_PERIOD);

        /* Retrieve the ith item, assuming that it is sorted from closest to furthest */
        update_vessel_packet(index);
        printk("Updated Transfer Packet\n");

        /* Update the data using the data_generic as a global */
        err = bt_le_adv_update_data(vessel_data, ARRAY_SIZE(vessel_data), NULL,
                                0);
        if (err) {
            gpio_pin_toggle_dt(&led);
            printk("Failed to set advertising data");
            return err;
        }

        /* Show that something is sending by toggling the LED */
        gpio_pin_toggle_dt(&led);
    }        
  
  /* Finished sending the closest 5 */
  }
  
  /* Should never reach */
  return 0;
}