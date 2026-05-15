// PwnDeck camera module — local LAN access for the ESP32-CAM.
//
// Two ways to view the camera from your phone/laptop:
//   1. cam.ap_stream  — ESP32-CAM hosts its own Wi-Fi AP "PwnDeck-CAM".
//                       Connect, open http://192.168.4.1/  → MJPEG live view.
//   2. cam.lan_stream — Joins your existing Wi-Fi (env-configurable creds);
//                       prints the obtained IP on the serial console.
// Both expose:
//   GET /        — single-page HTML viewer
//   GET /stream  — multipart/x-mixed-replace MJPEG stream
//   GET /jpg     — one-shot JPEG snapshot
//
// Press 'q' on the PwnDeck serial console to stop the server and return
// to the menu.

#include <Arduino.h>
#include "../../menu/registry.h"
#include "../../../include/pins.h"

#if defined(HAS_CAMERA) && HAS_CAMERA

#include <WiFi.h>
#include <esp_camera.h>
#include <esp_http_server.h>

// AI-Thinker ESP32-CAM pin map (the board in the user's Drive folder).
#define CAM_PWDN   32
#define CAM_RESET  -1
#define CAM_XCLK    0
#define CAM_SIOD   26
#define CAM_SIOC   27
#define CAM_Y9     35
#define CAM_Y8     34
#define CAM_Y7     39
#define CAM_Y6     36
#define CAM_Y5     21
#define CAM_Y4     19
#define CAM_Y3     18
#define CAM_Y2      5
#define CAM_VSYNC  25
#define CAM_HREF   23
#define CAM_PCLK   22

static const char kIndexHtml[] PROGMEM = R"HTML(<!doctype html><html><head>
<meta name=viewport content="width=device-width,initial-scale=1">
<title>PwnDeck Cam</title>
<style>
 body{background:#111;color:#eee;font-family:system-ui;margin:0;padding:8px}
 img{width:100%;max-width:640px;border:1px solid #333;display:block;margin:auto}
 .row{display:flex;gap:8px;justify-content:center;margin:8px}
 button{background:#1a1;color:#000;border:0;padding:8px 14px;font-weight:bold;border-radius:6px}
</style></head><body>
<h3 style="text-align:center">PwnDeck — ESP32-CAM</h3>
<img id=v src="/stream">
<div class=row>
 <button onclick="document.getElementById('v').src='/stream?'+Date.now()">restart</button>
 <a href="/jpg" download="cam.jpg"><button>snapshot</button></a>
</div>
</body></html>)HTML";

static httpd_handle_t s_httpd = NULL;

static esp_err_t handler_index(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, kIndexHtml, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t handler_jpg(httpd_req_t *req) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) return httpd_resp_send_500(req);
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Cache-Control", "no-store");
  esp_err_t r = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return r;
}

static esp_err_t handler_stream(httpd_req_t *req) {
  static const char *kBoundary = "frame";
  static char kHdr[64];
  httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=frame");
  while (true) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) { delay(50); continue; }
    int n = snprintf(kHdr, sizeof(kHdr),
                     "--%s\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
                     kBoundary, (unsigned)fb->len);
    if (httpd_resp_send_chunk(req, kHdr, n) != ESP_OK ||
        httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len) != ESP_OK ||
        httpd_resp_send_chunk(req, "\r\n", 2) != ESP_OK) {
      esp_camera_fb_return(fb);
      break;
    }
    esp_camera_fb_return(fb);
    if (Serial.available() && Serial.peek() == 'q') break;
  }
  return ESP_OK;
}

static bool init_camera(void) {
  camera_config_t c = {};
  c.ledc_channel = LEDC_CHANNEL_0;
  c.ledc_timer = LEDC_TIMER_0;
  c.pin_d0=CAM_Y2; c.pin_d1=CAM_Y3; c.pin_d2=CAM_Y4; c.pin_d3=CAM_Y5;
  c.pin_d4=CAM_Y6; c.pin_d5=CAM_Y7; c.pin_d6=CAM_Y8; c.pin_d7=CAM_Y9;
  c.pin_xclk=CAM_XCLK; c.pin_pclk=CAM_PCLK;
  c.pin_vsync=CAM_VSYNC; c.pin_href=CAM_HREF;
  c.pin_sccb_sda=CAM_SIOD; c.pin_sccb_scl=CAM_SIOC;
  c.pin_pwdn=CAM_PWDN; c.pin_reset=CAM_RESET;
  c.xclk_freq_hz = 20000000;
  c.pixel_format = PIXFORMAT_JPEG;
  c.frame_size = psramFound() ? FRAMESIZE_SVGA : FRAMESIZE_VGA;
  c.jpeg_quality = 12;
  c.fb_count = psramFound() ? 2 : 1;
  c.grab_mode = CAMERA_GRAB_LATEST;
  c.fb_location = psramFound() ? CAMERA_FB_IN_PSRAM : CAMERA_FB_IN_DRAM;
  esp_err_t e = esp_camera_init(&c);
  if (e != ESP_OK) {
    Serial.printf("  camera init failed: 0x%x\n", e);
    return false;
  }
  return true;
}

static void start_httpd(void) {
  httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
  cfg.server_port = 80;
  if (httpd_start(&s_httpd, &cfg) != ESP_OK) {
    Serial.println("  httpd start failed");
    return;
  }
  httpd_uri_t u_idx    = {"/",       HTTP_GET, handler_index,  NULL};
  httpd_uri_t u_jpg    = {"/jpg",    HTTP_GET, handler_jpg,    NULL};
  httpd_uri_t u_stream = {"/stream", HTTP_GET, handler_stream, NULL};
  httpd_register_uri_handler(s_httpd, &u_idx);
  httpd_register_uri_handler(s_httpd, &u_jpg);
  httpd_register_uri_handler(s_httpd, &u_stream);
}

static void stop_httpd(void) {
  if (s_httpd) { httpd_stop(s_httpd); s_httpd = NULL; }
}

static void wait_for_quit(const char *url) {
  Serial.printf("  open %s in a browser  (press 'q' here to stop)\n", url);
  while (true) {
    if (Serial.available() && Serial.read() == 'q') break;
    delay(100);
  }
  stop_httpd();
  WiFi.disconnect(true, true);
  Serial.println("  camera server stopped");
}

void cam_ap_stream(void) {
  Serial.println("[cam.ap_stream] starting SoftAP");
  if (!init_camera()) return;
  WiFi.mode(WIFI_AP);
  WiFi.softAP("PwnDeck-CAM", "pwndeck123");
  IPAddress ip = WiFi.softAPIP();
  start_httpd();
  char url[40];
  snprintf(url, sizeof(url), "http://%s/", ip.toString().c_str());
  wait_for_quit(url);
}

void cam_lan_stream(void) {
  Serial.println("[cam.lan_stream] joining LAN");
  if (!init_camera()) return;
#ifndef PWNDECK_WIFI_SSID
  Serial.println("  build with -DPWNDECK_WIFI_SSID=\\\"name\\\" -DPWNDECK_WIFI_PASS=\\\"pw\\\"");
  return;
#else
  WiFi.mode(WIFI_STA);
  WiFi.begin(PWNDECK_WIFI_SSID, PWNDECK_WIFI_PASS);
  uint32_t deadline = millis() + 15000;
  while (WiFi.status() != WL_CONNECTED && millis() < deadline) delay(250);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("  wifi join failed");
    return;
  }
  start_httpd();
  char url[40];
  snprintf(url, sizeof(url), "http://%s/", WiFi.localIP().toString().c_str());
  wait_for_quit(url);
#endif
}

void cam_snapshot_serial(void) {
  Serial.println("[cam.snapshot] one frame to serial as base64");
  if (!init_camera()) return;
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) { Serial.println("  capture failed"); return; }
  Serial.printf("  jpeg %u bytes  (binary follows after 'BEGIN_JPEG')\n", (unsigned)fb->len);
  Serial.println("BEGIN_JPEG");
  Serial.write(fb->buf, fb->len);
  Serial.println();
  Serial.println("END_JPEG");
  esp_camera_fb_return(fb);
}

#else  // non-camera targets: stubs so the menu still links.

void cam_ap_stream(void)       { Serial.println("[cam] not built for this target"); }
void cam_lan_stream(void)      { Serial.println("[cam] not built for this target"); }
void cam_snapshot_serial(void) { Serial.println("[cam] not built for this target"); }

#endif
