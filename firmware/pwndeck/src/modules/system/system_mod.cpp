#include <Arduino.h>
#include <SPIFFS.h>
#include <Preferences.h>
#include "../../menu/registry.h"

void system_panic_wipe(void) {
  Serial.println("[system.panic_wipe] erasing SPIFFS + preferences");
  if (SPIFFS.begin(true)) SPIFFS.format();
  Preferences p; p.begin("pwndeck", false); p.clear(); p.end();
  Serial.println("  done. reboot recommended.");
}

void system_wiring_wizard(void) {
  Serial.println("[system.wizard] verifying CC1101 SPI link...");
  extern void subghz_scan(void);
  subghz_scan();
  Serial.println("  if you saw RSSI numbers above, wiring is OK.");
}

static const char *kRegions[] = {"US", "EU", "JP", "BR", "AU"};
void system_region_gate(void) {
  Preferences p; p.begin("pwndeck", false);
  String cur = p.getString("region", "");
  Serial.printf("[system.region] current=%s  options: US EU JP BR AU\n",
                cur.length() ? cur.c_str() : "(unset)");
  Serial.println("  send `region=XX` over serial to set");
  p.end();
}

void system_about(void) {
  Serial.printf("PwnDeck %s  build %s %s\n", PWNDECK_VERSION, __DATE__, __TIME__);
  Serial.println("https://github.com/jknutson0000-png/esp32-display-api");
  Serial.println("AUTHORIZED USE ONLY. Operator is responsible for legal compliance.");
}
