#include <bt_ahu.h>
#include <stddef.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys_clock.h>
#include <zephyr/types.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/*
 * Set Advertisement data. Based on the Eddystone specification:
 * https://github.com/google/eddystone/blob/master/protocol-specification.md
 * https://github.com/google/eddystone/tree/master/eddystone-url
 */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xaa, 0xfe),
    BT_DATA_BYTES(BT_DATA_SVC_DATA16, 0xaa, 0xfe, /* Eddystone UUID */
                  0x10,                           /* Eddystone-URL frame type */
                  0x00, /* Calibrated Tx power at 0m */
                  0x00, /* URL Scheme Prefix http://www. */
                  'z', 'e', 'p', 'h', 'y', 'r', 'p', 'r', 'o', 'j', 'e', 'c',
                  't', 0x08) /* .org */
};

static void bt_ready(int err) {
  char addr_s[BT_ADDR_LE_STR_LEN];
  bt_addr_le_t addr = {0};
  size_t count = 1;

  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
    return;
  }

  printk("Bluetooth initialized\n");

  /* Start advertising */
  err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
  if (err) {
    printk("Advertising failed to start (err %d)\n", err);
    return;
  }

  /* For connectable advertising you would use
   * bt_le_oob_get_local().  For non-connectable non-identity
   * advertising an non-resolvable private address is used;
   * there is no API to retrieve that.
   */

  bt_id_get(&addr, &count);
  bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));

  printk("Beacon started, advertising as %s\n", addr_s);
}

/**
 * void bt_thingy_task(void* p1, void* p2, void* p3)
  --------------------------------------------------
  Send sensor data through ibeacon protocol
*/
void bt_task(void* p1, void* p2, void* p3) {
  int err;

  printk("Starting Beacon Demo\n");

  /* Initialize the Bluetooth Subsystem */
  err = bt_enable(bt_ready);
  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
  }

  while (1) {
    // send data over bluetooth
  }

  return;
}
