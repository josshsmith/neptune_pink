#include <bt_ahu.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/data/json.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/slist.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys_clock.h>
#include <zephyr/types.h>
#include <zephyr/usb/usb_device.h>

#define BT_PRIORITY 7
#define BT_STACKSIZE 1024
#define ALL_MODULES 4

LOG_MODULE_REGISTER(AHU, ALL_MODULES);

// // begin bt thread
K_THREAD_DEFINE(bt_task_id, BT_STACKSIZE, bt_task, NULL, NULL, NULL,
                BT_PRIORITY, 0, 0);
