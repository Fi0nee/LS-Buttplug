#pragma once

#include <NimBLEDevice.h>

inline void bluetooth_service_init() {

    NimBLEDevice::init("LVS-Lush11");

    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->setMinPreferred(0x02);
}

inline void bluetooth_service_start() {NimBLEDevice::startAdvertising();}
inline void bluetooth_service_stop() {NimBLEDevice::stopAdvertising();}
inline void bluetooth_service_set_battery_level(float battery_voltage) {}
inline void add_user_events_characteristic(NimBLEService *pService) {}
