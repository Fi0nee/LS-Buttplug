#pragma once

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>

void muse_start();
void muse_stop();
void muse_init();

void muse_set_intensity(float intensity_percent);
