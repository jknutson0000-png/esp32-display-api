#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "../../menu/registry.h"

static void log_line(const char *s) { Serial.println(s); }

void wifi_scan(void) {
  log_line("[wifi.scan] starting");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++) {
    Serial.printf("  %2d) %-32s ch=%2d rssi=%d %s\n", i,
                  WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i),
                  WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "OPEN" : "SEC");
  }
  WiFi.scanDelete();
}

// Promiscuous deauth-frame detector. Counts type=0 subtype=12 frames.
static volatile uint32_t s_deauth_count = 0;
static void promisc_cb(void *buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT) return;
  const uint8_t *p = ((wifi_promiscuous_pkt_t *)buf)->payload;
  uint8_t subtype = (p[0] & 0xF0) >> 4;
  if (subtype == 0x0C || subtype == 0x0A) s_deauth_count++;
}
void wifi_deauth_detect(void) {
  log_line("[wifi.deauth_detect] 15s window");
  s_deauth_count = 0;
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&promisc_cb);
  uint32_t end = millis() + 15000;
  while (millis() < end) delay(50);
  esp_wifi_set_promiscuous(false);
  Serial.printf("  observed %u deauth/disassoc frames\n", s_deauth_count);
}

void wifi_evil_portal(void)  { log_line("[wifi.evil_portal] stub — captive portal AP+DNS"); }
void wifi_beacon_spam(void)  { log_line("[wifi.beacon_spam] stub — needs raw 802.11 TX"); }
