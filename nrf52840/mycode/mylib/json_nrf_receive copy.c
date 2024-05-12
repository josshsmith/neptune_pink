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

/* SHELL Command Header Files */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

/* Logging Header Files */
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/logging/log_output.h>

/* UART Header Files */
#include <zephyr/drivers/uart.h>

#include <zephyr/drivers/gpio.h>

/* JSON Includes */
#include <zephyr/data/json.h>
#include "logging.h"
#include "json_nrf_receive.h"

/* Define the UART_DEVICE_NODE for consistent uart use */
#define UART_DEVICE_NODE DT_NODELABEL(uart0) // Can't print anything if using uart, maybe make it a command instead of uart?
#define MAX_STRING_LEN 256
#define POLL_INTERVAL_MS 20

#define VESSEL_DATA_INIT(_mssi, _name, _call_sign, _time, _latitude, _longitude, _speed, _rssi) \
{ \
    .mssi = (_mssi), \
    .name = (_name), \
    .call_sign = (_call_sign), \
    .time = (_time), \
    .latitude = (_latitude), \
    .longitude = (_longitude), \
    .speed = (_speed), \
    .rssi = (_rssi) \
}

/* Set the UART Dev */
const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* Declare the logging module in the .c files */
LOG_MODULE_DECLARE(LOG_MODULE_NAME, LOG_LEVEL_DBG);

/* Receive Setup */
K_MSGQ_DEFINE(vessel_msgq, sizeof(struct vessel_data), 5, 1);
static char rx_buf[MAX_STRING_LEN];
static int rx_buf_pos;
struct vessel_data rx_struct;

struct vessel_data vessel_data_array[5]; // Use this

// Reference Golioth.io blogs "How To Parse JSON Data in Zephyr"
// Initialise JSON data object
static const struct json_obj_descr JSON_vessel_data_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct vessel_data, mssi, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, name, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, call_sign, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, time, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, latitude, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, longitude, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, speed, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, rssi, JSON_TOK_NUMBER)
  // BELOW MAY NOT BE VALID, DOCUMENTATION SAYS ONLY THE ABOVE 3 ARE VALID
  // JSON_OBJ_DESCR_PRIM(struct vessel_data, fixed_x_cord, JSON_TOK_FLOAT), 
};

// Decode the json buffer
int format_json(char* input_string) {
  int err;
  
  err = json_obj_parse(input_string, sizeof(input_string), 
                        JSON_vessel_data_descr, 
                        ARRAY_SIZE(JSON_vessel_data_descr),
                        &rx_struct);
  
  if (err) {
    // LOG_ERR("JSON Parsing Fault: %d", err);
  }

  return err;
}

/*
 * void read_thread(void)
 *
 * Reads
 *
 * params:
 *      - dev (const struct device *) -> The uart device for reading
 *      - user_data (void *) -> A variable piece of data that has
 *                                been transferred
 * returns:
 *
 * Reference:
 *        - Taken from Zephyr/samples/drivers/uart/echo_bot
 */
static void serial_cb(const struct device *dev, void *user_data) {
  uint8_t c;

  if (!uart_irq_update(uart_dev)) {
    return;
  }

  if (!uart_irq_rx_ready(uart_dev)) {
    return;
  }

  /* read until FIFO empty */
  while (uart_fifo_read(uart_dev, &c, 1) == 1) {
    if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
      /* Terminate string */
      rx_buf[rx_buf_pos] = '\0';

      /* if queue is full, message is silently dropped */
      int err = format_json(rx_buf);

      if (!err) {
        k_msgq_put(&vessel_msgq, &rx_buf, K_NO_WAIT);
      } 

      /* reset the buffer (it was copied to the msgq) */
      rx_buf_pos = 0;
    } else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
      rx_buf[rx_buf_pos++] = c;
    }
  }
}

// Thread to receive and update global data struct
void read_thread(struct vessel_data vessel_data_tx) {
    if (!k_msgq_get(&vessel_msgq, &vessel_data_tx, K_NO_WAIT)) {
        // LOG_DBG("Message Received");
        // Determine index of the vessel data array to position point
        int index = 0;

        // Update data and position in array for some global data struct here
        vessel_data_array[index] = vessel_data_tx;
    };

    // printk("test");
    // Poll to check for messages every 20ms
    k_msleep(POLL_INTERVAL_MS);
}

/*
 * void read_thread(void)
 *
 * Handles the read thread that initialises the queue  and interrupt from the
 * uart and then waits for messages from the GCU. Once a message is received it
 * is read and the command type is identified. If the command type is
 * recognised, the appropriate response will be made. Otherwise an error message
 * will be logged.
 *
 * params:
 * returns:
 *      - 0 if the thread is terminated
 */
int init_read_thread() {
  // Initialise the queue
  struct vessel_data vessel_data_tx = VESSEL_DATA_INIT(1, "No Name", "NA", "2024_11_05", 0, 0, 0, 0);
  
  // Initiate the read interrupt with error checking
  int ret = uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);

  if (ret < 0) {
    if (ret == -ENOTSUP) {
      // LOG_ERR("Interrupt-driven UART API support not enabled\n");
    } else if (ret == -ENOSYS) {
      // LOG_ERR("UART device does not support interrupt-driven API\n");
    } else {
      // LOG_ERR("Error setting UART callback: %d\n", ret);
    }
    return 0;
  }

  // Enable the interrupt
  uart_irq_rx_enable(uart_dev);

  // LOG_DBG("UART init OK");

  while (1) {
    read_thread(vessel_data_tx);
  }

  return 0;
}

#define STACKSIZE 1024
#define PRIORITY 7

K_THREAD_DEFINE(read_thread_id, STACKSIZE, init_read_thread, NULL, NULL,
                  NULL, PRIORITY, 0, 0);