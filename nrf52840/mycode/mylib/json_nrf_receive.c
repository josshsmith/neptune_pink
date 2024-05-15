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

#define VESSEL_DATA_INIT(_mmsi, _name, _call_sign, _time, _latitude, _longitude, _speed, _heading) \
{ \
    .mmsi = (_mmsi), \
    .vessel_name = (_name), \
    .call_sign = (_call_sign), \
    .update_time = (_time), \
    .latitude = (_latitude), \
    .longitude = (_longitude), \
    .speed = (_speed), \
    .heading = (_heading) \
}

/* The devicetree node identifier for the "led0" alias. */
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

/* Set the UART Dev */
const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* Declare the logging module in the .c files */
LOG_MODULE_DECLARE(LOG_MODULE_NAME, LOG_LEVEL_DBG);

/* Receive Setup */
K_MSGQ_DEFINE(vessel_msgq, sizeof(struct vessel_data), 5, 1);
static char rx_buf[MAX_STRING_LEN];
struct vessel_data rx_struct;

struct vessel_data vessel_data_array[5]; // Use this

struct test_data rx_struct_test;

// Reference Golioth.io blogs "How To Parse JSON Data in Zephyr"
// Initialise JSON data object
static const struct json_obj_descr JSON_vessel_data_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct vessel_data, mmsi, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, vessel_name, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, call_sign, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, update_time, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, latitude, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, longitude, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, speed, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct vessel_data, heading, JSON_TOK_NUMBER) 
  // Can't use floats
  
  /* Maximum value is 32767 for int and 2 147 483 647 for two ints,
   * Would need mmsi as two ints,
   * latitude and longitude as 3 ints (1 for upper, 2 for decimal place), and then
   * speed and heading as 2 ints (1 for upper, 1 for decimal)
   * 
   * Would almost certainly be better / easier to do strings for all.
   */
};

static const struct json_obj_descr JSON_test_data_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct vessel_data, mmsi, JSON_TOK_NUMBER),
  // JSON_OBJ_DESCR_PRIM(struct vessel_data, vessel_name, JSON_TOK_STRING),
  // JSON_OBJ_DESCR_PRIM(struct vessel_data, call_sign, JSON_TOK_STRING),
  // JSON_OBJ_DESCR_PRIM(struct vessel_data, update_time, JSON_TOK_STRING),
  // JSON_OBJ_DESCR_PRIM(struct vessel_data, latitude, JSON_TOK_NUMBER),
  // JSON_OBJ_DESCR_PRIM(struct vessel_data, longitude, JSON_TOK_NUMBER),
  // JSON_OBJ_DESCR_PRIM(struct vessel_data, speed, JSON_TOK_NUMBER),
  // JSON_OBJ_DESCR_PRIM(struct vessel_data, heading, JSON_TOK_NUMBER)
  // BELOW MAY NOT BE VALID, DOCUMENTATION SAYS ONLY THE ABOVE 3 ARE VALID
  // JSON_OBJ_DESCR_PRIM(struct vessel_data, fixed_x_cord, JSON_TOK_FLOAT), 
};

// Decode the json buffer
int format_json(char* input_string) {
  int err;
  printk("Input String %s\n", input_string);
  printk("Size of %d\n", strlen(input_string));
  
  // Important to consider command line has max limit of 256 characters.
  // If there is more than 256 characters there should be concern.

  err = json_obj_parse(input_string, strlen(input_string), 
                        JSON_vessel_data_descr, 
                        ARRAY_SIZE(JSON_vessel_data_descr),
                        &rx_struct);

  // err = json_obj_parse(input_string, strlen(input_string), 
  //                       JSON_test_data_descr, 
  //                       ARRAY_SIZE(JSON_test_data_descr),
  //                       &rx_struct_test);
  
  if (err < 0) {
    LOG_ERR("JSON Parsing Fault: %d", err);
    gpio_pin_toggle_dt(&led1);
  } else {
    LOG_INF("%d\n", rx_struct.mmsi);
    LOG_INF("%s\n", rx_struct.vessel_name);
    LOG_INF("%s\n", rx_struct.call_sign);
    LOG_INF("%s\n", rx_struct.update_time);
    LOG_INF("%d\n", rx_struct.latitude);
    LOG_INF("%d\n", rx_struct.longitude);
    LOG_INF("%d\n", rx_struct.speed);
    LOG_INF("%d\n", rx_struct.heading);
  }

  gpio_pin_toggle_dt(&led2);
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
int cmd_parse_json(const struct shell *sh, size_t argc, char **argv) {
  int err = 0;
  char string_copy[MAX_STRING_LEN];

  if (argc > 1) {
    int err = format_json(strcpy(string_copy, argv[1]));

    if (!err) {
      k_msgq_put(&vessel_msgq, &rx_buf, K_NO_WAIT);
    } 
  }

  return err;
}

// Thread to receive and update global data struct
void read_thread(struct vessel_data vessel_data_tx) {
    if (!k_msgq_get(&vessel_msgq, &vessel_data_tx, K_NO_WAIT)) {
        LOG_DBG("Message Received");
        // Determine index of the vessel data array to position point
        int index = 0;

        // Toggle pin
        gpio_pin_toggle_dt(&led2);

        // Update data and position in array for some global data struct here
        vessel_data_array[index] = vessel_data_tx;
    };

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

  /* Initialise and toggle the LED */
  if (!gpio_is_ready_dt(&led1)) {
    return 0;
  }

  gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
  gpio_pin_toggle_dt(&led1);

  if (!gpio_is_ready_dt(&led2)) {
    return 0;
  }

  gpio_pin_configure_dt(&led2, GPIO_OUTPUT_ACTIVE);
  gpio_pin_toggle_dt(&led2);

  while (1) {
    read_thread(vessel_data_tx);
  }

  return 0;
}


void init_ibeacon_add_cmd(void) {
  SHELL_CMD_ARG_REGISTER(json_send, NULL, "Pass new data",
                         cmd_parse_json, 2, 0);
}

#define STACKSIZE 1024
#define PRIORITY 7

K_THREAD_DEFINE(read_thread_id, STACKSIZE, init_read_thread, NULL, NULL,
                  NULL, PRIORITY, 0, 0);