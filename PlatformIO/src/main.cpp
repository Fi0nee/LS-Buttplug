#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ble.h"
#include "LS.h"
#include "lovense.h"

void setup() {
    Serial.begin(115200);
    bluetooth_service_init();
    lovense_init();
    muse_init();
    muse_start();
    bluetooth_service_start();
}

void loop() {
    if (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        line.trim();

        if (line.length() > 0) {
            // Проверяем, начинается ли команда с "Vibrate:"
            if (line.startsWith("Vibrate:") && line.endsWith(";")) {
                // Извлекаем число между ": " и ";"
                int colonPos = line.indexOf(':');
                int semicolonPos = line.indexOf(';');
                String valueStr = line.substring(colonPos + 1, semicolonPos);
                int level = valueStr.toInt();

                // Ограничиваем диапазон 0–20
                if (level < 0) level = 0;
                if (level > 20) level = 20;

                // Преобразуем в float 0.0–1.0
                float intensity = level / 20.0f;
                muse_set_intensity(intensity);
            }
        }
    }

    delay(10); // короткая пауза
}

