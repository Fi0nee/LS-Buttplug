#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lovense.h"
#include "LS.h"
#include "nvs_flash.h"

void setup() {
    Serial.begin(115200);

    // Полная очистка NVS перед инициализацией BLE
    nvs_flash_erase();
    nvs_flash_init();
    delay(10);

    bluetooth_service_init();
    lovense_init();
    muse_init();
    muse_start();

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName(DEVICE_NAME);

    bluetooth_service_start();
}

void loop() {
    if (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        line.trim();

        if (line.length() > 0) {
            if (line.startsWith("Vibrate:") && line.endsWith(";")) {
                int colonPos = line.indexOf(':');
                int semicolonPos = line.indexOf(';');
                String valueStr = line.substring(colonPos + 1, semicolonPos);
                int level = valueStr.toInt();

                if (level < 0) level = 0;
                if (level > 20) level = 20;

                float intensity = level / 20.0f;
                muse_set_intensity(intensity);
            }
        }
    }

    delay(10);
}