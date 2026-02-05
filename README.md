# esp32-display-api

ESP32 Display API with web control panel - REST endpoints for temperature monitoring and status updates with CORS support.

## Features

- **GET `/api/esp32-data`** - Retrieve current ESP32 display values (JSON)
- **POST `/api/esp32-data`** - Update ESP32 display values
- **CORS Enabled** - Full cross-origin support for ESP32 requests
- **Web Control Panel** - Beautiful UI to manage values in real-time
- **Persistent Storage** - Values persist in `esp32_data.json`
- **Real-time Polling** - Perfect for 5-second ESP32 fetch cycles

## Project Structure

```
esp32-display-api/
├── config.ru              # Main Scarlet/Sinatra application
├── public/
│   ├── index.html        # Web control panel
│   ├── style.css         # Styling
│   └── app.js            # Frontend logic
├── esp32_data.json       # Data persistence file (auto-created)
└── README.md
```

## API Endpoints

### GET /api/esp32-data

Retrieve current values for your ESP32 display.

**Request:**
```bash
curl http://your-server:3000/api/esp32-data
```

**Response:**
```json
{
  "temp1": "425",
  "temp2": "380",
  "status": "RUNNING",
  "custom": "000"
}
```

### POST /api/esp32-data

Update one or more values. All fields are optional.

**Request:**
```bash
curl -X POST http://your-server:3000/api/esp32-data \
  -H "Content-Type: application/json" \
  -d '{
    "temp1": "425",
    "temp2": "380",
    "status": "RUNNING",
    "custom": "000"
  }'
```

**Response:**
```json
{
  "success": true,
  "data": {
    "temp1": "425",
    "temp2": "380",
    "status": "RUNNING",
    "custom": "000"
  }
}
```

## Installation & Setup

### Prerequisites

- Ruby 2.5+
- Bundler

### Step 1: Clone or Create Project

```bash
git clone https://github.com/jknutson0000-png/esp32-display-api.git
cd esp32-display-api
```

### Step 2: Install Dependencies

Create a `Gemfile`:

```ruby
source 'https://rubygems.org'

gem 'sinatra'
gem 'json'
```

Then run:

```bash
bundle install
```

### Step 3: Start the Server

```bash
bundle exec rackup config.ru -p 3000
```

The server will start on `http://localhost:3000`

## Usage

### Accessing Web Panel

Visit `http://your-server-ip:3000` in your browser to see the control panel.

### ESP32 Arduino Code Example

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* serverName = "http://192.168.1.100:3000/api/esp32-data";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName);
      int httpResponseCode = http.GET();
      
      if (httpResponseCode > 0) {
        String payload = http.getString();
        
        StaticJsonDocument<256> doc;
        deserializeJson(doc, payload);
        
        const char* temp1 = doc["temp1"];
        const char* temp2 = doc["temp2"];
        const char* status = doc["status"];
        const char* custom = doc["custom"];
        
        Serial.printf("Temp1: %s, Temp2: %s, Status: %s, Custom: %s\n", 
                      temp1, temp2, status, custom);
        
        // Update your display here
      }
      http.end();
    }
    lastTime = millis();
  }
}
```

## Deployment

### Local Network

1. Find your server IP: `ipconfig` (Windows) or `ifconfig` (Linux/Mac)
2. Update ESP32 code with your server IP
3. Start the API server

### Cloud Deployment (Optional)

**Heroku:**

```bash
heroku create your-app-name
git push heroku main
heroku open
```

**Railway/Render/Fly.io:** Similar process with their CLI tools.

## Configuration

### Change Port

Edit `config.ru`:

```ruby
set :port, 8080  # Change from 3000 to 8080
```

### Modify Default Values

Edit the `$esp32_data` hash in `config.ru`:

```ruby
$esp32_data = {
  temp1: "425",
  temp2: "380",
  status: "RUNNING",
  custom: "000"
}
```

### Database (SQLite Optional)

For production, you can replace the JSON file with SQLite:

```ruby
require 'sqlite3'

DB = SQLite3::Database.new 'esp32_data.db'
DB.execute <<-SQL
  CREATE TABLE IF NOT EXISTS esp32_data (
    id INTEGER PRIMARY KEY,
    temp1 TEXT,
    temp2 TEXT,
    status TEXT,
    custom TEXT
  );
SQL
```

## Troubleshooting

### CORS Errors

The API has CORS headers enabled. If still getting errors:
- Check your ESP32's request headers
- Ensure server is accessible from ESP32's network
- Verify port 3000 is not blocked by firewall

### Values Not Persisting

- Check write permissions on `esp32_data.json`
- Ensure `public/` directory exists

### Connection Refused

- Verify server is running: `curl http://localhost:3000`
- Check firewall settings
- Confirm IP address and port are correct

## License

MIT License - Feel free to use and modify as needed.

## Support

For issues or questions, open an issue on GitHub or check the API documentation above.
