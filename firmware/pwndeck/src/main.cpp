#include <Arduino.h>
#include "menu/registry.h"
#include "../include/pins.h"

#if HAS_DISPLAY
  #include <TFT_eSPI.h>
  static TFT_eSPI tft;
#endif

static int g_cursor = 0;

static void draw_menu(void) {
#if HAS_DISPLAY
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(2);
  tft.setCursor(4, 4);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.printf("PwnDeck %s\n", PWNDECK_VERSION);
  for (size_t i = 0; i < kMenuCount; i++) {
    tft.setTextColor(i == (size_t)g_cursor ? TFT_BLACK : TFT_WHITE,
                     i == (size_t)g_cursor ? TFT_GREEN : TFT_BLACK);
    tft.setCursor(4, 24 + (int)i * 16);
    tft.printf(" %s ", kMenu[i].label);
  }
#endif
  Serial.println("--- PwnDeck menu ---");
  for (size_t i = 0; i < kMenuCount; i++)
    Serial.printf(" %c %2u) %s\n", i == (size_t)g_cursor ? '>' : ' ',
                  (unsigned)i, kMenu[i].label);
  Serial.println("type number to run, n/p to move, j for JSON list");
}

static void emit_json_menu(void) {
  Serial.print("{\"menu\":[");
  for (size_t i = 0; i < kMenuCount; i++) {
    Serial.printf("%s{\"id\":\"%s\",\"label\":\"%s\",\"area\":\"%s\"}",
                  i ? "," : "", kMenu[i].id, kMenu[i].label, kMenu[i].area);
  }
  Serial.println("]}");
}

void setup(void) {
  Serial.begin(115200);
  delay(200);
#if HAS_DISPLAY
  tft.init();
  tft.setRotation(1);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif
  draw_menu();
}

static String buf;
void loop(void) {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      buf.trim();
      if (buf.length()) {
        if (buf == "j") { emit_json_menu(); }
        else if (buf == "n") { g_cursor = (g_cursor + 1) % kMenuCount; draw_menu(); }
        else if (buf == "p") { g_cursor = (g_cursor + kMenuCount - 1) % kMenuCount; draw_menu(); }
        else {
          int idx = buf.toInt();
          if (idx >= 0 && (size_t)idx < kMenuCount) {
            Serial.printf(">> %s\n", kMenu[idx].label);
            kMenu[idx].handler();
            draw_menu();
          }
        }
      }
      buf = "";
    } else if (buf.length() < 32) buf += c;
  }
  delay(10);
}
