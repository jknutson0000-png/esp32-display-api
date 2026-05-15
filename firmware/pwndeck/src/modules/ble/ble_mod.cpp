#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "../../menu/registry.h"

void ble_scan(void) {
  Serial.println("[ble.scan] 10s active scan");
  BLEDevice::init("");
  BLEScan *s = BLEDevice::getScan();
  s->setActiveScan(true);
  BLEScanResults r = s->start(10, false);
  for (int i = 0; i < r.getCount(); i++) {
    BLEAdvertisedDevice d = r.getDevice(i);
    Serial.printf("  %s rssi=%d name=%s\n",
                  d.getAddress().toString().c_str(), d.getRSSI(),
                  d.getName().c_str());
  }
  s->clearResults();
}

void ble_spam_apple(void)   { Serial.println("[ble.spam_apple] stub — Apple Continuity adv"); }
void ble_spoof_airtag(void) { Serial.println("[ble.spoof_airtag] stub — AirTag-style adv"); }
