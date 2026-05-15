# PwnDeck

A user-friendly, menu-driven ESP32 red-team / pentest firmware. Inspired
by Bruce and M5Stick-Launcher, but tailored to the hardware in the
project's `Esp32` Drive folder.

## Hardware supported

| Photo                       | Board                            | Role in PwnDeck                          |
| --------------------------- | -------------------------------- | ---------------------------------------- |
| `20260515_012757.jpg`       | ESP32-CAM (AI-Thinker, ESP-32S)  | secondary: camera + SD logger           |
| `20260515_012808.jpg`       | CC1101 433 MHz V2.0 module       | sub-GHz radio (SPI)                      |
| `20260515_012822.jpg`       | Ebyte E07-M1101D (CC1101 433MHz) | sub-GHz radio alternate                  |
| `20260515_012827.jpg`       | ESP32 DevKit V1 (38-pin)         | headless target (`env:devkit`)           |
| `20260515_012831.jpg`       | ESP32 DevKit (USB-C)             | headless target (`env:devkit`)           |
| `20260515_012852.jpg`       | ESP32-2432S028R "CYD"            | **primary** target (`env:cyd`)           |

## What's new vs Bruce

- **Wiring Wizard** — first-boot guided check that the CC1101 is wired
  correctly *before* any sub-GHz menu is enabled.
- **Panic Wipe** — single action clears all captures + prefs from SPIFFS.
- **Region Gate** — operator picks region on first boot; firmware
  refuses sub-GHz TX outside that region's ISM bands.
- **JSON-over-serial** control mode (`j` lists menu as JSON) so a
  desktop GUI can drive it without screen-scraping.
- **Single menu registry** — every action declared once in
  `src/menu/registry.cpp`; `scripts/verify.sh` enforces a handler exists.

## Build

```
pio run -e cyd          # ESP32-2432S028 with TFT + touch
pio run -e devkit       # ESP32 DevKit V1, serial-menu only
pio run -e cam          # ESP32-CAM (limited; subghz disabled)
```

## Wiring (CC1101 → ESP32)

| CC1101 | CYD pin | DevKit pin |
| ------ | ------- | ---------- |
| VCC    | 3V3     | 3V3        |
| GND    | GND     | GND        |
| CS     | 22      | 5          |
| SCK    | 18      | 18         |
| MOSI   | 23      | 23         |
| MISO   | 19      | 19         |
| GDO0   | 35      | 16         |
| GDO2   | 27      | 17         |

## Legal

Authorized security testing only. The operator is responsible for all
applicable laws including FCC Part 15 / RED / radio-licensing rules.
PwnDeck enforces a region gate for TX features but you are still
responsible for verifying compliance.
