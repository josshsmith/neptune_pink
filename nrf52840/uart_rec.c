#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_stdint.h>
#include <uart_rec.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

/* SHELL Command Header Files */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

/* UART Header Files */
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>

/* JSON Includes */
#include <zephyr/data/json.h>

// #include "json_nrf_receive.h"

/* JSON Includes */
#include <zephyr/data/json.h>

#define POLL_INTERVAL_MS 20

// struct k_sem data_sem;
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

#define MSG_SIZE 512

struct vessel_data rx_struct;

#define VESSEL_DATA_INIT(_mmsi, _name, _call_sign, _time, _latitude,    \
                         _longitude, _speed, _heading)                  \
  {                                                                     \
    .mmsi = (_mmsi), .vessel_name = (_name), .call_sign = (_call_sign), \
    .update_time = (_time), .latitude = (_latitude),                    \
    .longitude = (_longitude), .speed = (_speed), .heading = (_heading) \
  }

static const struct json_obj_descr JSON_vessel_data_descr[] = {
    JSON_OBJ_DESCR_PRIM(struct vessel_data, mmsi, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, vessel_name, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, call_sign, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, update_time, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, latitude, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, longitude, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, speed, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, heading, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, position, JSON_TOK_STRING)
    // Can't use floats

    /* Maximum value is 32767 for int and 2 147 483 647 for two ints,
     * Would need mmsi as two ints,
     * latitude and longitude as 3 ints (1 for upper, 2 for decimal place),
     and
     * then speed and heading as 2 ints (1 for upper, 1 for decimal)
     *
     * Would almost certainly be better / easier to do strings for all.
     */
};

int format_json(char *input_string) {
  int err;

  // printk("Input String %s\n", input_string);
  // printk("Size of %d\n", strlen(input_string));

  // Important to consider command line has max limit of 256 characters.
  // If there is more than 256 characters there should be concern.
  err =
      json_obj_parse(input_string, strlen(input_string), JSON_vessel_data_descr,
                     ARRAY_SIZE(JSON_vessel_data_descr), &rx_struct);
  return err;
}

int message_pos = 0;
char rx_buf[MSG_SIZE];

// message queue for recieving on second uart line
K_MSGQ_DEFINE(recieve_msgq, MSG_SIZE, 10, 4);

// message queue for recieving on second uart line
K_MSGQ_DEFINE(bt_msgq, sizeof(struct vessel_data), 10, 4);
// serial_cb(const struct device *dev, void *user_data)
//-----------------------------------------------------------------
// interrupt handler for second uart
// inputs:
// dev- device node
// user_data - data from uart
void serial_cb(const struct device *dev, void *user_data) {
  uint8_t c;

  if (!uart_irq_update(uart_dev)) {
    // printk("Error initialising");
    return;
  }

  if (!uart_irq_rx_ready(uart_dev)) {
    // printk("Error initialising rx");
    return;
  }
  while (uart_fifo_read(uart_dev, &c, 1) == 1) {
    if (c != '$') {
      rx_buf[message_pos] = c;
      message_pos++;
    } else {
      // finished
      rx_buf[message_pos] = '\0';
      message_pos = 0;
      k_msgq_put(&recieve_msgq, &rx_buf, K_NO_WAIT);
    }
  }
}

// uart_task(void *p1, void *p2, void *p3)
//-------------------------
// Uart task waits on message queue for packet to send
void uart_task(void *p1, void *p2, void *p3) {
  if (!device_is_ready(uart_dev)) {
    // printk("UART device not found!\n");
  }

  char buffer[MSG_SIZE];

  /* configure interrupt and callback to receive data */
  int ret = uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);

  if (ret < 0) {
    if (ret == -ENOTSUP) {
      // printk("Interrupt-driven UART API support not enabled\n");
    } else if (ret == -ENOSYS) {
      // printk("UART device does not support interrupt-driven API\n");
    } else {
      // printk("Error setting UART callback: %d\n", ret);
    }
  }
  uart_irq_rx_enable(uart_dev);
  while (1) {
    // wait indefinitely for message from shell
    k_msgq_get(&recieve_msgq, &buffer, K_FOREVER);
    format_json(buffer);
    // got a struct done
    //   printk(
    //       "Recieved a Struct: MMSI: %s, Vessel Name: %s, Call Sign: %s,
    //       Update " "Time: %s, Latitude: %s, Longitude: %s, Speed: %s,
    //       Heading: %s, " "Position: %s$", rx_struct.mmsi,
    //       rx_struct.vessel_name, rx_struct.call_sign, rx_struct.update_time,
    //       rx_struct.latitude, rx_struct.longitude, rx_struct.speed,
    //       rx_struct.heading, rx_struct.position);
    // }
    k_msgq_put(&bt_msgq, &rx_struct, K_NO_WAIT);
  }
}
