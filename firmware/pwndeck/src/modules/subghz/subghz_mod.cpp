// Sub-GHz radio module. Only built with a CC1101 wired up — that means
// the CYD and DevKit targets. The ESP32-CAM target gets stubs because
// the camera consumes all the SPI-capable pins.

#include <Arduino.h>
#include "../../menu/registry.h"
#include "../../../include/pins.h"

#if defined(PWNDECK_TARGET_CYD) || defined(PWNDECK_TARGET_DEVKIT)

#include <SPI.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>

static bool g_cc_ready = false;

static bool cc1101_init(float mhz = 433.92) {
  ELECHOUSE_cc1101.setSpiPin(CC1101_SCK, CC1101_MISO, CC1101_MOSI, CC1101_CS);
  ELECHOUSE_cc1101.Init();
  if (ELECHOUSE_cc1101.getCC1101() == false) return false;
  ELECHOUSE_cc1101.setMHZ(mhz);
  ELECHOUSE_cc1101.SetRx();
  g_cc_ready = true;
  return true;
}

void subghz_scan(void) {
  if (!g_cc_ready && !cc1101_init()) { Serial.println("[subghz] CC1101 not detected"); return; }
  Serial.println("[subghz.scan] scanning 300-928 MHz coarse");
  const float bands[] = {315.0, 433.92, 868.35, 915.0};
  for (float f : bands) {
    ELECHOUSE_cc1101.setMHZ(f);
    delay(20);
    int rssi = ELECHOUSE_cc1101.getRssi();
    Serial.printf("  %7.2f MHz  rssi=%d dBm  lqi=%d\n", f, rssi, ELECHOUSE_cc1101.getLqi());
  }
}

void subghz_capture(void) {
  if (!g_cc_ready && !cc1101_init()) { Serial.println("[subghz] CC1101 not detected"); return; }
  Serial.println("[subghz.capture] 5s ASK/OOK pulse capture (stub: prints RSSI stream)");
  uint32_t end = millis() + 5000;
  while (millis() < end) {
    Serial.printf("%lu,%d\n", millis(), ELECHOUSE_cc1101.getRssi());
    delay(2);
  }
}

void subghz_replay(void)     { Serial.println("[subghz.replay] stub — needs capture-file format"); }
void subghz_jam_detect(void) { Serial.println("[subghz.jam_detect] stub — long-RSSI heuristic"); }

#else  // CAM target: no CC1101 wiring possible.

void subghz_scan(void)       { Serial.println("[subghz] CC1101 unavailable on this target"); }
void subghz_capture(void)    { Serial.println("[subghz] CC1101 unavailable on this target"); }
void subghz_replay(void)     { Serial.println("[subghz] CC1101 unavailable on this target"); }
void subghz_jam_detect(void) { Serial.println("[subghz] CC1101 unavailable on this target"); }

#endif
