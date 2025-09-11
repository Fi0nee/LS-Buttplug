#include "lovense.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "muse.h"
#include <NimBLEDevice.h>

#define DEVICE_NAME     "LVS-ESP"
#define DEVICE_SERVICE  "45440001-0023-4BD4-BBD5-A6920E4C5653"
#define DEVICE_CHAR_RX  "45440002-0023-4BD4-BBD5-A6920E4C5653"
#define DEVICE_CHAR_TX  "45440003-0023-4BD4-BBD5-A6920E4C5653"

static NimBLECharacteristic* blecRX;
static NimBLECharacteristic* blecTX;
static NimBLEServer* server;
static NimBLEService* service;

// ---------------- AutoSwitch variables ----------------
static bool autoSwitchTurnOff = true;
static bool autoSwitchLastLevel = true;

// ---------------- Forward declarations ----------------
std::string generate_response(const std::string& command);
std::string get_device_info();
std::string get_battery_level();
std::string get_device_status();
std::string get_auto_switch_options();
std::string set_auto_switch_options(const std::string& command);
std::string set_vibration_speed(const std::string& command);

// ---------------- Server callbacks ----------------
class MyServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) override {}
    void onDisconnect(NimBLEServer* pServer) override {NimBLEDevice::getAdvertising()->start();}
};

// ---------------- Characteristic callbacks ----------------
class MyCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* c) override {
        std::string message = c->getValue();

        if (message.empty()) return;

        std::string response = generate_response(message);

        if (!response.empty()) {
            blecTX->setValue(response);
            blecTX->notify();
        }
    }
};

// ---------------- Lovense logic ----------------
std::string generate_response(const std::string& command) {
    if (command == "DeviceType;") return get_device_info();
    else if (command == "Battery;") return get_battery_level();
    else if (command == "Status:1;") return get_device_status();
    else if (command.find("AutoSwitch:") == 0) return set_auto_switch_options(command);
    else if (command.find("Vibrate:") == 0) return set_vibration_speed(command);
    else if (command == "PowerOff;") return set_vibration_speed("Vibrate:0;");
    else return "OK;";
}

std::string get_device_info() {
    std::string firmwareVersion = "01";
    std::string deviceType = "G";

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char macAddressStr[18];
    sprintf(macAddressStr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return deviceType + ":" + firmwareVersion + ":" + macAddressStr + ";";
}

std::string get_battery_level() { return "100;"; }
std::string get_device_status() { return "2;"; }

std::string get_auto_switch_options() {
    return "AutoSwitch:" + std::string(autoSwitchTurnOff ? "1:" : "0:") + (autoSwitchLastLevel ? "1;" : "0;");
}

std::string set_auto_switch_options(const std::string& command) {
    if (command == "AutoSwitch:On:Off;") { autoSwitchTurnOff = true; autoSwitchLastLevel = false; }
    else if (command == "AutoSwitch:On:On;") { autoSwitchTurnOff = true; autoSwitchLastLevel = true; }
    else if (command == "AutoSwitch:Off:On;") { autoSwitchTurnOff = false; autoSwitchLastLevel = true; }
    else if (command == "AutoSwitch:Off:Off;") { autoSwitchTurnOff = false; autoSwitchLastLevel = false; }
    return "OK;";
}

std::string set_vibration_speed(const std::string& command) {
    size_t startPos = command.find(":") + 1;
    size_t endPos = command.find(";");
    if (startPos == std::string::npos || endPos == std::string::npos || endPos <= startPos) return "Invalid command;";

    int speedInt = std::stoi(command.substr(startPos, endPos - startPos));
    float speed = static_cast<float>(speedInt) / 20.0f;

    muse_set_intensity(speed);
    return "OK;";
}

// ---------------- Lovense init ----------------
void lovense_init() {
    NimBLEDevice::init(DEVICE_NAME);

    server = NimBLEDevice::createServer();
    server->setCallbacks(new MyServerCallbacks());

    service = server->createService(DEVICE_SERVICE);

    blecRX = service->createCharacteristic(DEVICE_CHAR_RX, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
    blecTX = service->createCharacteristic(DEVICE_CHAR_TX, NIMBLE_PROPERTY::NOTIFY);

    blecRX->setCallbacks(new MyCharacteristicCallbacks());

    service->start();

    NimBLEAdvertising* advert = NimBLEDevice::getAdvertising();
    advert->addServiceUUID(DEVICE_SERVICE);
    advert->start();
}
