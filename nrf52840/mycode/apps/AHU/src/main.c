#include <bt_ahu.h>
#include <stdlib.h>
#include <string.h>
#include <uart_rec.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/data/json.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/slist.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys_clock.h>
#include <zephyr/types.h>
#include <zephyr/usb/usb_device.h>

#define PRIORITY 7
#define STACKSIZE 2048
#define ALL_MODULES 4
LOG_MODULE_REGISTER(AHU, 4);

// begin uart thread
K_THREAD_DEFINE(uart_task_id, STACKSIZE, uart_task, NULL, NULL, NULL, PRIORITY,
                0, 0);
// begin bt thread
K_THREAD_DEFINE(bt_task_id, STACKSIZE, bt_task, NULL, NULL, NULL, PRIORITY, 0,
                0);

K_THREAD_DEFINE(alter_data_task_id, STACKSIZE, alter_data_task, NULL, NULL,
                NULL, PRIORITY, 0, 0);