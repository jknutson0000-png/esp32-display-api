require 'sinatra'
require 'json'
require 'fileutils'

# Initialize data storage
$esp32_data = {
  temp1: "425",
  temp2: "380",
  status: "RUNNING",
  custom: "000"
}

# Data persistence file
DATA_FILE = 'esp32_data.json'

# Load existing data on startup
if File.exist?(DATA_FILE)
  $esp32_data = JSON.parse(File.read(DATA_FILE), symbolize_names: true)
end

class ESP32Server < Sinatra::Base
  set :bind, '0.0.0.0'
  set :port, 3000
  
  # Enable CORS for ESP32 requests
  before do
    headers['Access-Control-Allow-Origin'] = '*'
    headers['Access-Control-Allow-Methods'] = 'GET, POST, OPTIONS'
    headers['Access-Control-Allow-Headers'] = 'Content-Type'
    headers['Content-Type'] = 'application/json'
  end
  
  # Handle OPTIONS requests (preflight)
  options '/api/*' do
    204
  end
  
  # GET endpoint for ESP32
  get '/api/esp32-data' do
    $esp32_data.to_json
  end
  
  # POST endpoint to update data
  post '/api/esp32-data' do
    begin
      request_data = JSON.parse(request.body.read, symbolize_names: true)
      
      # Update only provided fields
      request_data.each do |key, value|
        if $esp32_data.key?(key.to_sym)
          $esp32_data[key.to_sym] = value.to_s
        end
      end
      
      # Persist to file
      File.write(DATA_FILE, $esp32_data.to_json)
      
      { success: true, data: $esp32_data }.to_json
    rescue => e
      status 400
      { success: false, error: e.message }.to_json
    end
  end
  
  # Serve web interface
  get '/' do
    send_file File.expand_path('public/index.html')
  end
  
  # Serve static files
  get '/style.css' do
    send_file File.expand_path('public/style.css')
  end
  
  get '/app.js' do
    send_file File.expand_path('public/app.js')
  end
end

run ESP32Server
