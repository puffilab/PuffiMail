#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_VL53L0X.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include "esp_sleep.h"

#define SWITCH_PIN 20
#define SDA_PIN 6
#define SCL_PIN 7
#define BUTTON_PIN 5
#define MAGNETIC_PIN 3
#define LED_PIN 4
#define BATTERY_ADC_PIN 2
#define DEFAULT_SLEEP_TIME 3600000000ULL
#define ADC_MAX_VAL 4095  
#define REF_VOLTAGE 3.3f  

Preferences preferences;
AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_VL53L0X vl = Adafruit_VL53L0X();


int baseDist = 0;
volatile bool buttonPressed = false;
volatile bool magneticTriggered = false;
bool sensorOk = false;
String mqtt_user;
String mqtt_password;

int measureDistance();
void sendMQTTData(int current, int baseline);
void setupWakeupSource();
bool connectWiFiAndMQTT();
void setupAPMode();
float readBatteryVoltage();
void checkBatteryAndNotify();
void handleWakeButtonActions();
bool detectLongPress(uint8_t pin, uint16_t durationMs);
bool detectTripleClick(uint8_t pin, uint16_t intervalMs);

void IRAM_ATTR buttonISR() { buttonPressed = true; }
void IRAM_ATTR magneticISR() { magneticTriggered = true; }

void setupWakeupSource() {
  uint64_t mask = (1ULL << BUTTON_PIN) | (1ULL << MAGNETIC_PIN);
  esp_deep_sleep_enable_gpio_wakeup(mask, ESP_GPIO_WAKEUP_GPIO_LOW);
  esp_sleep_enable_timer_wakeup(DEFAULT_SLEEP_TIME);
}



void handleWakeButtonActions() {
  const unsigned long window = 20000; 
  const unsigned long longPressTime = 10000;
  unsigned long start = millis();
  int clickCount = 0;

  Serial.println("[BUTTON] Enter button detection window (20 seconds)...");

  while (millis() - start < window) {

    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);


    if (digitalRead(BUTTON_PIN) == LOW) {
      unsigned long pressStart = millis();

      while (digitalRead(BUTTON_PIN) == LOW) {
        if (millis() - pressStart > longPressTime) {
          Serial.println("[BUTTON] Long press detected for 10 seconds → Restore factory settings");
          preferences.begin("settings", false);
          preferences.clear();
          preferences.end();
          delay(500);
          ESP.restart();
        }
        delay(10);
      }

      unsigned long pressDuration = millis() - pressStart;
      if (pressDuration < 1000) {
        clickCount++;
        Serial.printf("[BUTTON] Short press #%d\n", clickCount);
        if (clickCount >= 4) {
          Serial.println("[BUTTON] Detected 4 consecutive presses → Enter AP mode");
          setupAPMode();
          return;
        }
      }
    }
  }

  Serial.println("\[BUTTON] Window timed out with no operation, re-entering Deep Sleep");
  esp_deep_sleep_start();
}


bool detectLongPress(uint8_t pin, uint16_t durationMs) {
  if (digitalRead(pin) == LOW) {
    unsigned long pressedTime = millis();
    while (digitalRead(pin) == LOW) {
      if (millis() - pressedTime > durationMs) return true;
      delay(10);
    }
  }
  return false;
}

bool detectTripleClick(uint8_t pin, uint16_t intervalMs) {
  int clickCount = 0;
  unsigned long lastClick = 0;
  while (millis() - lastClick < intervalMs * 3) {
    if (digitalRead(pin) == LOW) {
      unsigned long pressTime = millis();
      while (digitalRead(pin) == LOW);
      if (millis() - pressTime < 800) {
        clickCount++;
        lastClick = millis();
        Serial.printf("[BUTTON] Click detected #%d\n", clickCount);
        if (clickCount >= 3) return true;
      }
    }
    delay(10);
  }
  return false;
}


bool connectWiFiAndMQTT() {
  preferences.begin("settings", false);
  String ssid = preferences.getString("wifi_ssid", "");
  String password = preferences.getString("wifi_password", "");
  String mqtt_server = preferences.getString("mqtt_server", "");
  mqtt_user = preferences.getString("mqtt_user", "");
  mqtt_password = preferences.getString("mqtt_password", "");
  preferences.end();

  WiFi.setHostname("SmartMailbox Sender");
  WiFi.begin(ssid.c_str(), password.c_str());
  for (int retry = 0; retry < 10 && WiFi.status() != WL_CONNECTED; retry++) {
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n❌ WiFi connection failed");
    return false;
  }
  Serial.println("\n✅ WiFi connection successful");

  client.setServer(mqtt_server.c_str(), 1883);
  if (client.connect("ESP32C3_Client", mqtt_user.c_str(), mqtt_password.c_str())) {
    Serial.println("✅ MQTT connection successful");
    client.publish("mailbox-sensor/availability", "online", true); 
    publishAutoDiscovery(); 
    return true;
  } else {
    Serial.println("❌ MQTT connection failed");
    return false;
  }
}


void setupAPMode() {
  Wire.begin(SDA_PIN, SCL_PIN);
  sensorOk = vl.begin();
  Serial.println(sensorOk ? "[SENSOR] VL53L0X initialized successfully (AP mode)" : "[SENSOR] Initialization failed (AP mode)");

  WiFi.softAP("Smartmailbox_Sender", "12345678");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html",
    "<!DOCTYPE html><html><head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<style>"
    "body {"
    "  font-family: sans-serif;"
    "  background: #f2f2f2;"
    "  margin: 0;"
    "  padding: 20px;"
    "  min-height: 100vh;"
    "}"
    ".container {"
    "  max-width: 500px;"
    "  margin: 0 auto;"
    "}"
    ".config-section {"
    "  background: white;"
    "  padding: 25px;"
    "  border-radius: 12px;"
    "  box-shadow: 0 2px 8px rgba(0,0,0,0.1);"
    "  margin-bottom: 25px;"
    "}"
    "h2 {"
    "  color: #2c3e50;"
    "  margin: 0 0 15px;"
    "  font-size: 1.5em;"
    "}"
    ".lang-select {"
    "  width: 100%;"
    "  padding: 10px;"
    "  margin-bottom: 15px;"
    "  border: 1px solid #ddd;"
    "  border-radius: 6px;"
    "  background: #f8f9fa;"
    "}"
    "input, button {"
    "  width: 100%;"
    "  padding: 12px;"
    "  margin: 8px 0;"
    "  border: 1px solid #ddd;"
    "  border-radius: 6px;"
    "  box-sizing: border-box;"
    "}"
    "button {"
    "  background: #3498db;"
    "  color: white;"
    "  border: none;"
    "  font-weight: bold;"
    "  cursor: pointer;"
    "  transition: background 0.2s;"
    "}"
    "button:hover {"
    "  background: #2980b9;"
    "}"
    ".ota-section {"
    "  margin-top: 25px;"
    "  padding-top: 25px;"
    "  border-top: 2px solid #eee;"
    "}"
    "progress {"
    "  width: 100%;"
    "  height: 20px;"
    "  border-radius: 10px;"
    "  margin: 15px 0;"
    "}"
    "progress::-webkit-progress-bar { background: #ecf0f1; }"
    "progress::-webkit-progress-value { background: #27ae60; }"
    "#otaStatus {"
    "  text-align: center;"
    "  color: #7f8c8d;"
    "  font-size: 0.9em;"
    "}"
    "</style>"
    "<script>"
    "const translations = {"
    "  'en': {"
    "    'title': 'Device Setup',"
    "    'wifi_ssid': 'WiFi SSID',"
    "    'wifi_pwd': 'WiFi Password',"
    "    'mqtt_server': 'MQTT Server',"
    "    'mqtt_user': 'MQTT Username',"
    "    'mqtt_pwd': 'MQTT Password',"
    "    'save_btn': 'Save Settings',"
    "    'measure_btn': 'Measure Distance',"
    "    'current_dist': 'Current Distance: ',"
    "    'baseline_label': 'Baseline Distance:',"
    "    'ota_title': 'Firmware Update',"
    "    'ota_btn': 'Upload Firmware',"
    "    'uploading': 'Uploading: '"
    "  },"
    "  'zh': {"
    "    'title': '设备配置',"
    "    'wifi_ssid': 'WiFi名称',"
    "    'wifi_pwd': 'WiFi密码',"
    "    'mqtt_server': 'MQTT服务器',"
    "    'mqtt_user': 'MQTT用户名',"
    "    'mqtt_pwd': 'MQTT密码',"
    "    'save_btn': '保存设置',"
    "    'measure_btn': '测量距离',"
    "    'current_dist': '当前距离: ',"
    "    'baseline_label': '基准距离:',"
    "    'ota_title': '固件更新',"
    "    'ota_btn': '上传固件',"
    "    'uploading': '上传进度: '"
    "  },"
    "  'ja': {"
    "    'title': '設定',"
    "    'wifi_ssid': 'WiFi SSID',"
    "    'wifi_pwd': 'WiFiパスワード',"
    "    'mqtt_server': 'MQTTサーバー',"
    "    'mqtt_user': 'MQTTユーザー名',"
    "    'mqtt_pwd': 'MQTTパスワード',"
    "    'save_btn': '設定保存',"
    "    'measure_btn': '距離を測定',"
    "    'current_dist': '現在の距離: ',"
    "    'baseline_label': '基準距離:',"
    "    'ota_title': 'ファームウェア更新',"
    "    'ota_btn': 'ファームウェアをアップロード',"
    "    'uploading': 'アップロード中: '"
    "  }"
    "};"

    "let currentLang = 'en';"
    "function updateLanguage(lang) {"
    "  currentLang = lang;"
    "  document.querySelectorAll('[data-trans]').forEach(el => {"
    "    const key = el.getAttribute('data-trans');"
    "    el.textContent = translations[lang][key];"
    "  });"
    "  document.title = translations[lang]['title'];"
    "  localStorage.setItem('lang', lang);"
    "}"
    "function uploadFirmware() {"
    "  const formData = new FormData(document.getElementById('otaForm'));"
    "  const xhr = new XMLHttpRequest();"
    "  xhr.open('POST', '/update');"
    "  xhr.upload.onprogress = function(e) {"
    "    if(e.lengthComputable) {"
    "      const percent = Math.round((e.loaded / e.total) * 100);"
    "      document.getElementById('progress').value = percent;"
    "      document.getElementById('otaStatus').textContent = "
    "        translations[currentLang]['uploading'] + percent + '%';"
    "    }"
    "  };"
    "  xhr.onload = function() {"
    "    if(xhr.status === 200) {"
    "      alert('Update Success! Rebooting...');"
    "      setTimeout(() => location.reload(), 2000);"
    "    } else {"
    "      alert('Error: ' + xhr.responseText);"
    "      document.getElementById('progress').value = 0;"
    "      document.getElementById('otaStatus').textContent = '';"
    "    }"
    "  };"
    "  xhr.send(formData);"
    "  return false;"
    "}"
    "document.addEventListener('DOMContentLoaded', () => {"
    "  const savedLang = localStorage.getItem('lang') || 'en';"
    "  document.getElementById('langSelect').value = savedLang;"
    "  updateLanguage(savedLang);"
    "});"

    "function measureDistance() {"
    "  fetch('/measure')"
    "    .then(response => response.text())"
    "    .then(data => {"
    "      document.getElementById('distanceValue').textContent = data + ' mm';"
    "      document.getElementById('baseline').value = data;"
    "    })"
    "    .catch(err => {"
    "      alert('Distance measurement failed.');"
    "      console.error(err);"
    "    });"
    "}"    


    "</script>"
    "</head><body>"
    "<div class='container'>"

    "  <div class='config-section'>"

    "    <h2 data-trans='title'></h2>"
    "    <select id='langSelect' class='lang-select' onchange='updateLanguage(this.value)'>"
    "      <option value='en'>English</option>"
    "      <option value='zh'>中文</option>"
    "      <option value='ja'>日本語</option>"
    "    </select>"

    "    <button onclick='measureDistance()' data-trans='measure_btn'></button>"
    "    <div style='margin:15px 0'>"
    "      <span data-trans='current_dist'></span>"
    "      <strong id='distanceValue'>-</strong>"
    "    </div>"
    "    <div>"
    "      <label data-trans='baseline_label'></label>"
    "      <input type='number' id='baseline' readonly>"
    "    </div>"
    "  </div>"

    "  <div class='config-section'>"
    "    <form action='/save' method='POST'>"
    "      <input type='text' name='ssid' placeholder='WiFi SSID' required>"
    "      <input type='password' name='password' placeholder='Password' required>"
    "      <input type='text' name='mqtt_server' placeholder='MQTT Server Address' required>"
    "      <input type='text' name='mqtt_user' placeholder='MQTT Username'>"
    "      <input type='password' name='mqtt_password' placeholder='MQTT Password'>"
    "      <button type='submit' data-trans='save_btn'></button>"
    "    </form>"
    "  </div>"



    "  <div class='config-section ota-section'>"
    "    <h3 data-trans='ota_title'></h3>"
    "    <form id='otaForm' onsubmit='return uploadFirmware()'>"
    "      <input type='file' name='firmware' accept='.bin' required>"
    "      <button type='submit' data-trans='ota_btn'></button>"
    "      <progress id='progress' value='0' max='100'></progress>"
    "      <div id='otaStatus'></div>"
    "    </form>"
    "  </div>"
    "</div>"
    "</body></html>");
  });


  server.on("/measure", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!sensorOk) {
      request->send(503, "text/plain", "Sensor Error");
      return;
    }
    int distance = measureDistance();
    if(distance >= 0 && !isnan(distance)) {
      preferences.begin("settings", false);
      preferences.putInt("baseDist", distance);
      preferences.end();
      baseDist = distance;
      Serial.printf("New baseline saved: %d\n", distance);
    }
    request->send(200, "text/plain", String(distance));
  });

  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
    preferences.begin("settings", false);
    if(request->hasArg("ssid")){
      preferences.putString("wifi_ssid", request->arg("ssid"));
    }
    if(request->hasArg("password")){
      preferences.putString("wifi_password", request->arg("password"));
    }
    if(request->hasArg("mqtt_server")){
      preferences.putString("mqtt_server", request->arg("mqtt_server"));
    }
    if(request->hasArg("mqtt_user")){
      preferences.putString("mqtt_user", request->arg("mqtt_user"));
    }
    if(request->hasArg("mqtt_password")){
      preferences.putString("mqtt_password", request->arg("mqtt_password"));
    }
    preferences.end();
    request->send(200, "text/html", "<h3>Saved. Restarting...</h3>");
    delay(1500);
    ESP.restart();
  });

  // OTA 上传
  server.on("/update", HTTP_POST,
    [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "Update Complete!");
      ESP.restart();
    },
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      if(!index){
        Serial.printf("OTA Start: %s\n", filename.c_str());
        Update.begin(UPDATE_SIZE_UNKNOWN);
      }
      if(!Update.write(data, len)){
        Serial.println("Write Error");
      }
      if(final){
        if(Update.end(true)){
          Serial.printf("OTA Success: %uB\n", index+len);
        } else {
          Update.printError(Serial);
        }
      }
    }
  );

  server.begin();
  Serial.println("\[AP] AP mode started");


  while (true) {
    ledBreathing();
  }
}

void ledBreathing() {
  static int brightness = 0;
  static int fadeAmount = 5;
  analogWrite(LED_PIN, brightness);
  brightness += fadeAmount;
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  delay(20);
}

int measureDistance() {
  if (!sensorOk) return -999;

  const int numSamples = 5;
  int samples[numSamples];
  int count = 0;

  for (int i = 0; i < numSamples; i++) {
    VL53L0X_RangingMeasurementData_t measure;
    vl.rangingTest(&measure, false);

    if (measure.RangeStatus == 0) {
      int dist = measure.RangeMilliMeter;
      if (dist >= 30 && dist <= 1000) {
        samples[count++] = dist;
      }
    }
    delay(10);
  }

  if (count == 0) {
    Serial.println("Distance measurement failed: no valid samples");
    return -1;
  }


  for (int i = 0; i < count - 1; i++) {
    for (int j = 0; j < count - i - 1; j++) {
      if (samples[j] > samples[j + 1]) {
        int tmp = samples[j];
        samples[j] = samples[j + 1];
        samples[j + 1] = tmp;
      }
    }
  }

  int median;
  if (count % 2 == 1) {
    median = samples[count / 2];
  } else {
    median = (samples[count / 2 - 1] + samples[count / 2]) / 2;
  }
  return median;
}


void sendMQTTData(int current, int baseline) {
  if (abs(current - baseline) < 1) return;

  char payload[64];
  snprintf(payload, sizeof(payload), "{\"current\":%d,\"baseline\":%d}", current, baseline);

  if (client.publish("mailbox-sensor/distance", payload)) {
    Serial.println("📡 Data successfully published to MQTT server.");
  } else {
    Serial.println("⚠️ MQTT publish failed.");
  }

  
  const char* status;
  if (current < baseline - 1) {
    status = "new_mail";
  } else if (abs(current - baseline) <= 1) {
    status = "empty";
  } else {
    status = "unknown";
  }

  if (client.publish("mailbox-sensor/mail_status", status)) {
    Serial.printf("📡 Publish status: %s\n", status);
  } else {
    Serial.println("⚠️ Failed to publish status message.");
  }
}



float readBatteryVoltage() {

  uint32_t sumMv = 0;
  const int samples = 16;
  for(int i=0;i<samples;i++){
    sumMv += analogReadMilliVolts(BATTERY_ADC_PIN); 
  }
  float avgMv = (float)sumMv/samples;  
  float batteryVoltage = (avgMv * 2.0f)/1000.0f;  
  return batteryVoltage;
}

void checkBatteryAndNotify() {
  float voltage = readBatteryVoltage();
  Serial.printf("Battery = %.2fV\n", voltage);

  const char* status = (voltage < 3.2f) ? "low_battery" : "normal";
  if (client.publish("mailbox-sensor/battery", status)) {
    Serial.printf("[BAT] Battery status sent: %s\n", status);
  } else {
    Serial.println("[BAT] Failed to publish battery status");
  }
}

void publishAutoDiscovery() {
  String node_id = "smartmailbox_sender";


  String topic1 = "homeassistant/sensor/" + node_id + "/distance/config";
  String payload1 = 
    "{"
      "\"name\": \"Mailbox Distance\","
      "\"state_topic\": \"mailbox-sensor/distance\","
      "\"unit_of_measurement\": \"mm\","
      "\"value_template\": \"{{ value_json.current }}\","
      "\"json_attributes_topic\": \"mailbox-sensor/distance\","
      "\"unique_id\": \"smartmailbox_distance\","
      "\"device\": {"
        "\"identifiers\": [\"" + node_id + "\"],"
        "\"name\": \"PuffiMail SmartMailbox\","
        "\"model\": \"PuffiMail V1.4\","
        "\"manufacturer\": \"FabLabDesigns\""
      "}"
    "}";

  client.publish(topic1.c_str(), payload1.c_str(), true);


  String topic2 = "homeassistant/sensor/" + node_id + "/mail_status/config";
  String payload2 = 
    "{"
      "\"name\": \"Mailbox Status\","
      "\"state_topic\": \"mailbox-sensor/mail_status\","
      "\"unique_id\": \"smartmailbox_mailstatus\","
      "\"device_class\": \"enum\","
      "\"options\": [\"new_mail\", \"empty\", \"unknown\"],"
      "\"device\": {"
        "\"identifiers\": [\"" + node_id + "\"]"
      "}"
    "}";

  client.publish(topic2.c_str(), payload2.c_str(), true);


  String topic3 = "homeassistant/binary_sensor/" + node_id + "/battery/config";
  String payload3 = 
    "{"
      "\"name\": \"Mailbox Battery\","
      "\"state_topic\": \"mailbox-sensor/battery\","
      "\"device_class\": \"battery\","
      "\"payload_on\": \"low_battery\","
      "\"payload_off\": \"normal\","
      "\"unique_id\": \"smartmailbox_battery\","
      "\"device\": {"
        "\"identifiers\": [\"" + node_id + "\"]"
      "}"
    "}";

  client.publish(topic3.c_str(), payload3.c_str(), true);
}



void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  sensorOk = vl.begin();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(MAGNETIC_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(MAGNETIC_PIN), magneticISR, FALLING);

  preferences.begin("settings", true);
  baseDist = preferences.getInt("baseDist", 1000);
  preferences.end();
  Serial.printf("Loaded baseDist: %d\n", baseDist);

  setupWakeupSource();

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

  if (cause == ESP_SLEEP_WAKEUP_GPIO) {
 
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("\[WAKE] Button wakeup");
      handleWakeButtonActions();
      return;
    }


    bool reverseMagnetic = (digitalRead(SWITCH_PIN) == LOW);
    bool triggerState = reverseMagnetic ? HIGH : LOW;

    int magneticLevel = digitalRead(MAGNETIC_PIN);
    Serial.printf("\[DEBUG] Current state of magnetic GPIO: %d\n", magneticLevel);

    if (magneticLevel == triggerState) {


      Serial.println("[WAKE] Start measuring");
      if (connectWiFiAndMQTT()) {
        int current = measureDistance();
        sendMQTTData(current, baseDist);
        checkBatteryAndNotify();
      }

      delay(1000);
      Serial.println("[SLEEP] Measurement completed → Deep Sleep");
      esp_deep_sleep_start();
    } else {
      Serial.println("[WAKE] GPIO wakeup, but magnetic sensor not triggered → Deep Sleep");
      esp_deep_sleep_start();
    }
    return;
  }

 
  if (cause == ESP_SLEEP_WAKEUP_UNDEFINED || cause == ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("[WAKE] First power-on or timer wakeup → Attempting to connect to Wi-Fi");
    if (connectWiFiAndMQTT()) {
      int current = measureDistance();
      sendMQTTData(current, baseDist);
      checkBatteryAndNotify();
      delay(1000);
      Serial.println("[SLEEP] Execution complete, entering Deep Sleep");
      esp_deep_sleep_start();
    } else {
      Serial.println("[AP] Wi-Fi/MQTT connection failed → Entering AP mode");
      setupAPMode();
      while (true) delay(1000);
    }
    return;
  }


  Serial.printf("[WAKE] Unknown wakeup type (%d)，Entering sleep\n", cause);
  esp_deep_sleep_start();
}



void loop() {

}

