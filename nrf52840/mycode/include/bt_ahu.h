#ifndef BT_AHU_H
#define BT_AHU_H

#include <zephyr/bluetooth/bluetooth.h>

void bt_task(void* p1, void* p2, void* p3);
void alter_data_task(void* p1, void* p2, void* p3);

#define SEND_PERIOD 500
#define MAX_NUMBER_OF_VESSELS 4
#define MAX_DATA 25

#endif