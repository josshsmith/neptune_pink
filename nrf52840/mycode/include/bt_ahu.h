#ifndef BT_AHU_H
#define BT_AHU_H

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys_clock.h>
#include <zephyr/types.h>

#define STACKSIZE 1024
#define PRIORITY 7

#define DATA_TYPE 0
#define SELECTION_TYPE 1

extern struct k_poll_signal bt_signal;

extern struct k_msgq uart_msgq;

extern struct k_sem data_sem;

void bt_task(void* p1, void* p2, void* p3);
int observer_start(void);
void device_found(const bt_addr_le_t* addr, int8_t rssi, uint8_t type,
                  struct net_buf_simple* ad);
uint8_t uuid_matches(uint8_t* recieve);

typedef struct Bt_Uart {
  uint8_t type;
  uint8_t instruction;
  uint8_t mode;
  uint8_t node_id;
  double data;

} Bt_Uart;

#endif