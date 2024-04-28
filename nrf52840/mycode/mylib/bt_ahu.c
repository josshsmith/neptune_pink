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
 * void device_found(const bt_addr_le_t* addr, int8_t rssi, uint8_t type,
                  struct net_buf_simple* ad)
  --------------------------------------------------
  Interrupt driven BLE reciever call back
  Filters incoming transmissions and extracts approriate information
*/
void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
                  struct net_buf_simple *ad) {
  // extract data and put on queue
}

/**
 * int observer_start(void)
  --------------------------------------------------
  Initalise call back for BLE reciever
*/
//.options = BT_LE_SCAN_OPT_FILTER_DUPLICATE,
// BT_LE_SCAN_OPT_FILTER_ACCEPT_LIST
int start_scanning(void) {
  struct bt_le_scan_param scan_param = {
      .type = BT_LE_SCAN_TYPE_PASSIVE,
      .options = BT_LE_SCAN_OPT_FILTER_ACCEPT_LIST,
      .interval = BT_GAP_SCAN_FAST_INTERVAL,
      .window = BT_GAP_SCAN_FAST_WINDOW,
  };
  // add raspberry pi address to the accept list

  int err = bt_le_scan_start(&scan_param, device_found);
  if (err) {
    return err;
  }
  printk("Started scanning...\n");

  return 0;
}

/**
 * void bt_task(void* p1, void* p2, void* p3)
  -------------------------------------------
  Initialise bt and wait for shell commands
*/
void bt_scan_ahu(void *p1, void *p2, void *p3) {
  int err;

  /* Initialize the Bluetooth Subsystem */
  err = bt_enable(NULL);
  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
  }
  int error;
  error = start_scanning();
  if (error) {
    printk("Error is %d", error);
  }

  while (1) {
    // process the bluetooth data
  }
  return;
}