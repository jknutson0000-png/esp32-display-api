#pragma once
// PwnDeck pin map. CC1101 wiring is identical across all targets; the
// CYD already uses HSPI for the TFT, so CC1101 lives on VSPI.

#if defined(PWNDECK_TARGET_CYD)
  // VSPI free pins on the ESP32-2432S028R header (CN1 + P3).
  #define CC1101_SCK   18
  #define CC1101_MISO  19
  #define CC1101_MOSI  23
  #define CC1101_CS    22
  #define CC1101_GDO0  35   // input-only OK
  #define CC1101_GDO2  27
  #define HAS_DISPLAY   1
  #define HAS_TOUCH     1
  #define HAS_SD        1
  #define SD_CS         5
#elif defined(PWNDECK_TARGET_DEVKIT)
  #define CC1101_SCK   18
  #define CC1101_MISO  19
  #define CC1101_MOSI  23
  #define CC1101_CS     5
  #define CC1101_GDO0  16
  #define CC1101_GDO2  17
  #define HAS_DISPLAY   0
  #define HAS_TOUCH     0
  #define HAS_SD        0
#elif defined(PWNDECK_TARGET_CAM)
  // ESP32-CAM has very few free pins; sub-GHz disabled by default.
  #define HAS_DISPLAY   0
  #define HAS_TOUCH     0
  #define HAS_SD        1
  #define SD_CS         5
  #define HAS_CAMERA    1
#else
  #error "Define one of PWNDECK_TARGET_CYD / _DEVKIT / _CAM"
#endif
