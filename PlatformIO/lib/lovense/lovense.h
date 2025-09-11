#pragma once

#include <NimBLEDevice.h>

// Initialize the lovense service
void lovense_init();

std::string get_device_info();
std::string get_device_status();
std::string get_battery_level();

std::string set_vibration_speed(const std::string& command);
std::string set_auto_switch_options(const std::string& command);
