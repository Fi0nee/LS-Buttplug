#include "bluetooth_service.h"

static const char* TAG = "bluetooth_service";

void bluetooth_service_init() {
  // Initialize the NimBLE device
  NimBLEDevice::init("LVS-Gateway01");

  // Create a new server
  NimBLEServer *pServer = NimBLEDevice::createServer();

  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->setMinPreferred(0x02);
}

void bluetooth_service_start() {
  NimBLEDevice::startAdvertising();
}

void bluetooth_service_stop() {
  NimBLEDevice::stopAdvertising();
}
