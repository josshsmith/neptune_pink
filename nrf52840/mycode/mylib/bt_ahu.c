#include <bt_ahu.h>
#include <hci.h>
#include <shell_acu.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys_clock.h>
#include <zephyr/types.h>

/**
 * void bt_thingy_task(void* p1, void* p2, void* p3)
  --------------------------------------------------
  Send sensor data through ibeacon protocol
*/
void bt_task(void* p1, void* p2, void* p3) {
  /* Initialize the Bluetooth Subsystem */

  err = bt_enable(bt_ready);

  while (1) {
    // send data over bluetooth
  }

  return;
}

/**
 * void bt_ready(int err)
  --------------------------------------------------
  Initalise bt
*/
void bt_ready(int err) {
  /* Start advertising */
  err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
}
