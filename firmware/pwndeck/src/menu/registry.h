#pragma once
#include <Arduino.h>

// One source of truth for every menu action. Each MENU_ENTRY declares
// an id (used by UI + serial JSON control) and a handler symbol that
// must be defined in src/modules/<area>/. scripts/verify.sh enforces
// that every id here has a matching handler implementation.

typedef void (*PwnHandler)(void);

struct MenuEntry {
  const char *id;
  const char *label;
  const char *area;     // "wifi" | "ble" | "subghz" | "system"
  PwnHandler handler;
};

// Handler forward decls — kept here so the linker catches missing ones.
void wifi_scan(void);
void wifi_deauth_detect(void);
void wifi_evil_portal(void);
void wifi_beacon_spam(void);
void ble_scan(void);
void ble_spam_apple(void);
void ble_spoof_airtag(void);
void subghz_scan(void);
void subghz_capture(void);
void subghz_replay(void);
void subghz_jam_detect(void);
void system_panic_wipe(void);
void system_wiring_wizard(void);
void system_region_gate(void);
void system_about(void);

extern const MenuEntry kMenu[];
extern const size_t kMenuCount;
