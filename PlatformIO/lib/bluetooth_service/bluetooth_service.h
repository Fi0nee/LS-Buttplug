#pragma once

#include <NimBLEDevice.h>

void bluetooth_service_init();
void bluetooth_service_start();
void bluetooth_service_stop();
void bluetooth_service_set_battery_level(float battery_voltage);

void add_user_events_characteristic(NimBLEService *pService);
