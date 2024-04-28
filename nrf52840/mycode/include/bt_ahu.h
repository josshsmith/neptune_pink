#ifndef BT_AHU_H
#define BT_AHU_H

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys_clock.h>
#include <zephyr/types.h>

#define DATA_TYPE 0
#define SELECTION_TYPE 1

void bt_task(void* p1, void* p2, void* p3);
void device_found(const bt_addr_le_t* addr, int8_t rssi, uint8_t type,
                  struct net_buf_simple* ad);

#endif