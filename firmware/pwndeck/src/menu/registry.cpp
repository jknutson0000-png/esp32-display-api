#include "registry.h"

const MenuEntry kMenu[] = {
  {"wifi.scan",          "Wi-Fi: Scan APs",         "wifi",   wifi_scan},
  {"wifi.deauth_detect", "Wi-Fi: Deauth Detector",  "wifi",   wifi_deauth_detect},
  {"wifi.evil_portal",   "Wi-Fi: Captive Portal",   "wifi",   wifi_evil_portal},
  {"wifi.beacon_spam",   "Wi-Fi: Beacon Spam",      "wifi",   wifi_beacon_spam},
  {"ble.scan",           "BLE: Scan",               "ble",    ble_scan},
  {"ble.spam_apple",     "BLE: Apple Adv Spam",     "ble",    ble_spam_apple},
  {"ble.spoof_airtag",   "BLE: AirTag Spoof",       "ble",    ble_spoof_airtag},
  {"subghz.scan",        "Sub-GHz: Scan",           "subghz", subghz_scan},
  {"subghz.capture",     "Sub-GHz: Capture",        "subghz", subghz_capture},
  {"subghz.replay",      "Sub-GHz: Replay",         "subghz", subghz_replay},
  {"subghz.jam_detect",  "Sub-GHz: Jam Detect",     "subghz", subghz_jam_detect},
  {"system.panic_wipe",  "System: Panic Wipe",      "system", system_panic_wipe},
  {"system.wizard",      "System: Wiring Wizard",   "system", system_wiring_wizard},
  {"system.region",      "System: Region Gate",     "system", system_region_gate},
  {"system.about",       "System: About",           "system", system_about},
  {"cam.ap_stream",      "Cam: AP + Live View",     "camera", cam_ap_stream},
  {"cam.lan_stream",     "Cam: Join LAN + Stream",  "camera", cam_lan_stream},
  {"cam.snapshot",       "Cam: Snapshot Serial",    "camera", cam_snapshot_serial},
};
const size_t kMenuCount = sizeof(kMenu) / sizeof(kMenu[0]);
