#pragma once

#include <NimBLEDevice.h>

// Инициализация Bluetooth
inline void bluetooth_service_init() {
    // Initialize the NimBLE device
    NimBLEDevice::init("LVS-Lush11");

    // Create a new server
    NimBLEServer *pServer = NimBLEDevice::createServer();

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->setMinPreferred(0x02);
}

// Старт рекламы
inline void bluetooth_service_start() {
    NimBLEDevice::startAdvertising();
}

// Остановка рекламы
inline void bluetooth_service_stop() {
    NimBLEDevice::stopAdvertising();
}


inline void bluetooth_service_set_battery_level(float battery_voltage) {
}

inline void add_user_events_characteristic(NimBLEService *pService) {
}
