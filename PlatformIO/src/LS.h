#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <NimBLEDevice.h>
#include <cmath>

static constexpr uint16_t MANUFACTURER_ID = 0xFFF0;
static constexpr size_t MANUFACTURER_DATA_LENGTH = 11;
static constexpr uint8_t MANUFACTURER_DATA_PREFIX[8] = {0x6D, 0xB6, 0x43, 0xCE, 0x97, 0xFE, 0x42, 0x7C};

static uint8_t _intensity_value = 0;
static uint8_t _last_intensity_value = 255;
static bool _stopping = false;

// ---------------- Channel values ----------------
static constexpr uint32_t CHANNELS[] = {
    0xE50000, // Stop
    0xF40000, // L1
    0xF70000, // L2
    0xF60000, // L3
    0xF10000, // L4
    0xF30000, // L5
    0xE70000, // L6
    0xE60000, // L7
};

// ---------------- Set manufacturer data ----------------
inline void set_manufacturer_data(uint8_t index) {
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->stop();

    uint8_t data[MANUFACTURER_DATA_LENGTH];
    memcpy(data, MANUFACTURER_DATA_PREFIX, sizeof(MANUFACTURER_DATA_PREFIX));

    uint32_t channel = CHANNELS[index];
    data[8]  = (channel >> 16) & 0xFF;
    data[9]  = (channel >> 8) & 0xFF;
    data[10] = channel & 0xFF;

    pAdvertising->setManufacturerData(std::string((char*)&MANUFACTURER_ID, 2) +
                                      std::string((char*)data, MANUFACTURER_DATA_LENGTH));

    pAdvertising->start();
}

// ---------------- BLE advertising task ----------------
inline void muse_advertising_task(void* pvParameters) {
    while (!_stopping) {
        if (_last_intensity_value != _intensity_value) {
            set_manufacturer_data(_intensity_value);
            _last_intensity_value = _intensity_value;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    set_manufacturer_data(0); // stop
    vTaskDelete(NULL);
}

// ---------------- Set intensity ----------------
inline void muse_set_intensity(float intensity_percent) {
    if (isnan(intensity_percent) || intensity_percent < 0.0f) intensity_percent = 0.0f;
    else if (intensity_percent > 1.0f) intensity_percent = 1.0f;

    _intensity_value = static_cast<uint8_t>(round(intensity_percent * (sizeof(CHANNELS)/sizeof(CHANNELS[0]) - 1)));
}

// ---------------- Start/Stop ----------------
inline void muse_start() {_stopping = false; xTaskCreatePinnedToCore(muse_advertising_task, "muse_advertising_task", 4096, nullptr, 2, nullptr, 0);}
inline void muse_stop() {_stopping = true;}

// ---------------- Init ----------------
inline void muse_init() {}
