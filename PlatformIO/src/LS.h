#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <LS.h>
#include <NimBLEDevice.h>
#include <cmath>

static uint16_t MANUFACTURER_ID = 0xFFF0;

#define MANUFACTURER_DATA_LENGTH 11
#define MANUFACTURER_DATA_PREFIX 0x6D, 0xB6, 0x43, 0xCE, 0x97, 0xFE, 0x42, 0x7C

static uint8_t _intensity_value = 0;
static uint8_t _last_intensity_value = 255;
static bool _stopping = false;

// ---------------- HEX ----------------
static uint8_t manufacturerDataList[][MANUFACTURER_DATA_LENGTH] = {
    {MANUFACTURER_DATA_PREFIX, 0xE5, 0x00, 0x00}, // Stop
    {MANUFACTURER_DATA_PREFIX, 0xF4, 0x00, 0x00}, // L1
    {MANUFACTURER_DATA_PREFIX, 0xF7, 0x00, 0x00}, // L2
    {MANUFACTURER_DATA_PREFIX, 0xF6, 0x00, 0x00}, // L3
    {MANUFACTURER_DATA_PREFIX, 0xF1, 0x00, 0x00}, // L4
    {MANUFACTURER_DATA_PREFIX, 0xF3, 0x00, 0x00}, // L5
    {MANUFACTURER_DATA_PREFIX, 0xE7, 0x00, 0x00}, // L6
    {MANUFACTURER_DATA_PREFIX, 0xE6, 0x00, 0x00}, // L7
};

// ---------------- Установка manufacturer data и реклама ----------------
inline void set_manufacturer_data(uint8_t index) {
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->stop();

    uint8_t *manufacturerData = manufacturerDataList[index];
    pAdvertising->setManufacturerData(
        std::string((char *)&MANUFACTURER_ID, 2) +
        std::string((char *)manufacturerData, MANUFACTURER_DATA_LENGTH)
    );

    pAdvertising->start();
}

// ---------------- Задача BLE рекламы ----------------
inline void muse_advertising_task(void *pvParameters) {
    while (!_stopping) {
        if (_last_intensity_value != _intensity_value) {
            set_manufacturer_data(_intensity_value);
            _last_intensity_value = _intensity_value;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    // один раз останавливаем все каналы
    set_manufacturer_data(0);
    vTaskDelete(NULL);
}

// ---------------- Установка интенсивности ----------------
inline void muse_set_intensity(float intensity_percent) {
    if (isnan(intensity_percent) || intensity_percent < 0.0f) intensity_percent = 0.0f;
    else if (intensity_percent > 1.0f) intensity_percent = 1.0f;

    _intensity_value = static_cast<uint8_t>(round(intensity_percent * 7.0f));
}

// ---------------- Запуск/остановка ----------------
inline void muse_start() {
    _stopping = false;
    xTaskCreatePinnedToCore(muse_advertising_task, "muse_advertising_task", 4096, nullptr, 2, nullptr, 0);
}

inline void muse_stop() {
    _stopping = true;
}

// ---------------- Инициализация ----------------
inline void muse_init() {
}
