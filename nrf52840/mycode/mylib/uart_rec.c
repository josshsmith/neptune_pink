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
#include <zephyr/logging/log.h>
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

/* JSON Includes */
#include <zephyr/data/json.h>

// setting up pc uart connection
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

struct vessel_data rx_struct;
LOG_MODULE_DECLARE(AHU, ALL_MODULES);

// struct filled with string representation of ais data
static const struct json_obj_descr JSON_vessel_data_descr[] = {
    JSON_OBJ_DESCR_PRIM(struct vessel_data, mmsi, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, vessel_name, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, call_sign, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, update_time, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, latitude, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, longitude, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, speed, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, heading, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct vessel_data, position, JSON_TOK_STRING)};

/**
 * int format_json(char *input_string)
 * fill json struct with strings from json string
 * input: json string
 * output: return value from json_obj_parse
 */
int format_json(char *input_string) {
  int err;
  err =
      json_obj_parse(input_string, strlen(input_string), JSON_vessel_data_descr,
                     ARRAY_SIZE(JSON_vessel_data_descr), &rx_struct);
  return err;
}

// message position when recieving on uart connection
int message_pos = 0;
// buffer to store input json string
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
  // initialising the uart
  if (!uart_irq_update(uart_dev)) {
    LOG_ERR("Error initialising");
    return;
  }

  if (!uart_irq_rx_ready(uart_dev)) {
    LOG_ERR("Error initialising rx");
    return;
  }
  while (uart_fifo_read(uart_dev, &c, 1) == 1) {
    if (c != '$') {  // add character to string buffer
      rx_buf[message_pos] = c;
      message_pos++;
    } else {
      // finished
      rx_buf[message_pos] = '\0';
      message_pos = 0;
      // put on queue to uart thread
      k_msgq_put(&recieve_msgq, &rx_buf, K_NO_WAIT);
    }
  }
}

// uart_task(void *p1, void *p2, void *p3)
//-------------------------
// Uart task waits on message queue for packet to send
void uart_task(void *p1, void *p2, void *p3) {
  if (!device_is_ready(uart_dev)) {
    LOG_ERR("UART device not found!\n");
  }

  char buffer[MSG_SIZE];

  /* configure interrupt and callback to receive data */
  int ret = uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);

  if (ret < 0) {
    if (ret == -ENOTSUP) {
      LOG_ERR("Interrupt-driven UART API support not enabled\n");
    } else if (ret == -ENOSYS) {
      LOG_ERR("UART device does not support interrupt-driven API\n");
    } else {
      LOG_ERR("Error setting UART callback: %d\n", ret);
    }
    return;
  }
  uart_irq_rx_enable(uart_dev);
  while (1) {
    // wait indefinitely for message from pc
    ret = k_msgq_get(&recieve_msgq, &buffer, K_FOREVER);
    LOG_INF("GETTING HERE!");
    if (ret) {
      LOG_WRN("Error reciving information on uart queue: %d", ret);
    }
    ret = format_json(buffer);  // format struct
    if (ret < 0) {
      LOG_WRN("Error formatting json: %d", ret);
    }
    ret = k_msgq_put(&bt_msgq, &rx_struct, K_NO_WAIT);
    if (ret) {
      LOG_WRN("Error putting information on uart queue: %d", ret);
    }
  }
}
