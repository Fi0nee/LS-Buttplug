#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bluetooth_service.h"
#include "muse.h"
#include "lovense.h"

static const char* TAG = "main";

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }

  bluetooth_service_init();

  lovense_init();
  muse_init();
  muse_start();

  bluetooth_service_start();

  ESP_LOGI(TAG, "SETUP DONE.");
}

void loop() {
    if (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        line.trim();
        if (line.length() > 0) {
            int level = line.toInt();
            float intensity = level / 7.0f; 
            muse_set_intensity(intensity);
        }
    }
    delay(10); // короткая пауза
}

