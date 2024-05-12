/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* General Header Files */
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

/* Bluetooth files */
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/net/buf.h>

/* SHELL Command Header Files */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

/* Logging Header Files */
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/logging/log_output.h>

/* Custom Logging Files */
#include "logging.h"
#include "ble_nrf_advertising.h"
#include "json_nrf_receive.h"

/* Thread Defines */
#define STACKSIZE 1024
#define PRIORITY 7
#define SLEEP_TIME_MAIN_AHU 2000

/* Logging Module Registration */
LOG_MODULE_REGISTER(LOG_MODULE_NAME, LOG_LEVEL_DBG);

/*
 * int main(void)
 *
 * Initialise all of the commands for the shell for prac 2
 *
 * params:
 * returns:
 *      - 0 if successful
 */
int main(void) {
  ble_nrf_adv_thread();
  return 0;
}