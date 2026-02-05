#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <Arduino_JSON.h>

// --- KONFIGURASI WIFI & MQTT ---
const char* ssid = "jco&family";
const char* password = "super.garpit1";
const char* mqtt_server = "broker.emqx.io"; 

// --- PIN ---
const int PIN_LED = 5;
const int PIN_BUTTON = 4;

// --- GLOBAL ---
WiFiClient espClient;
PubSubClient client(espClient);
Preferences pref;
volatile bool buttonTriggered = false;
int brightness = 0; 
int targetBrightness = 0;

// --- TOPIC MQTT ---
const char* topic_publish = "esp32/smartlamp/data";
const char* topic_subscribe = "esp32/smartlamp/control";

// --- ISR: Interupsi Tombol ---
void IRAM_ATTR handleButton() {
    buttonTriggered = true;
}

// --- CALLBACK: Menerima Perintah dari HP/MQTTX ---
void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.println("\n[MQTT] Pesan Diterima di Topik: " + String(topic));
    Serial.println("[MQTT] Isi Pesan: " + message);

    int val = message.toInt();
    if (val >= 0 && val <= 255) {
        targetBrightness = val;
        brightness = val; 
        pref.putInt("last_bright", targetBrightness);
        Serial.printf("[SYSTEM] Brightness diatur via Remote: %d\n", targetBrightness);
    }
}

// --- TASK 1: Kontrol LED & Tombol (Core 1) ---
void taskHardware(void *pvParameters) {
    for (;;) {
        if (buttonTriggered) {
            vTaskDelay(50 / portTICK_PERIOD_MS); 
            if (digitalRead(PIN_BUTTON) == LOW) { 
                buttonTriggered = false;
                
                targetBrightness += 51; 
                if (targetBrightness > 255) targetBrightness = 0;
                
                brightness = targetBrightness; 
                pref.putInt("last_bright", targetBrightness);
                
                Serial.println("\n[BUTTON] Tombol Ditekan!");
                Serial.printf("[SYSTEM] Target Brightness Baru: %d\n", targetBrightness);
            } else {
                buttonTriggered = false; 
            }
        }

        static int currentPWM = 0; 
        if (currentPWM < targetBrightness) {
            currentPWM++;
            ledcWrite(PIN_LED, currentPWM);
            vTaskDelay(5 / portTICK_PERIOD_MS);
        } else if (currentPWM > targetBrightness) {
            currentPWM--;
            ledcWrite(PIN_LED, currentPWM);
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS); 
    }
}

// --- TASK 2: WiFi & MQTT (Core 0) ---
void taskNetwork(void *pvParameters) {
    for (;;) {
        // 1. Cek Koneksi WiFi
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("\n[!] WIFI TERPUTUS");
            Serial.print("[WIFI] Menghubungkan kembali ke: "); Serial.println(ssid);
            
            WiFi.disconnect(); 
            vTaskDelay(100 / portTICK_PERIOD_MS);
            WiFi.begin(ssid, password);

            int retry = 0;
            while (WiFi.status() != WL_CONNECTED && retry < 20) {
                vTaskDelay(500 / portTICK_PERIOD_MS);
                Serial.print("."); 
                retry++;
            }

            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("\n\n>>> WIFI CONNECTED INFO <<<");
                Serial.print("SSID      : "); Serial.println(WiFi.SSID());
                Serial.print("IP Address: "); Serial.println(WiFi.localIP());
                Serial.print("MAC Addr  : "); Serial.println(WiFi.macAddress());
                Serial.print("Signal    : "); Serial.print(WiFi.RSSI()); Serial.println(" dBm");
                Serial.println("---------------------------\n");
            }
        }

        // 2. Logika Reconnect MQTT
        if (WiFi.status() == WL_CONNECTED && !client.connected()) {
            Serial.println("[MQTT] Status: Disconnected. Reconnecting...");
            String clientId = "ESP32_Farhan_" + String(random(0, 9999));
            
            if (client.connect(clientId.c_str())) {
                Serial.println(">>> MQTT BROKER CONNECTED <<<");
                Serial.print("Broker    : "); Serial.println(mqtt_server);
                Serial.print("Client ID : "); Serial.println(clientId);
                client.subscribe(topic_subscribe); 
                Serial.print("Subscribed: "); Serial.println(topic_subscribe);
                Serial.println("---------------------------\n");
            } else {
                Serial.print("[ERROR] MQTT Gagal, rc="); Serial.println(client.state());
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                continue; 
            }
        }

        if (client.connected()) {
            client.loop(); 
            JSONVar data;
            data["status"] = (brightness > 0) ? "ON" : "OFF";
            data["val"] = brightness;
            client.publish(topic_publish, JSON.stringify(data).c_str());
        }
        
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000); 
    
    Serial.println("\n\n==============================================");
    Serial.println("       IOT SMART LAMP - MIKROPROSESOR        ");
    Serial.println("  DEVELOPED BY: FARHANDHIKA NURROHMAN (ESP32)  ");
    Serial.println("==============================================");
    
    // Info Memori & Chip (Tambahan Keren)
    Serial.print("[SYSTEM] Chip Model: "); Serial.println(ESP.getChipModel());
    Serial.print("[SYSTEM] CPU Cores : "); Serial.println(ESP.getChipCores());
    Serial.print("[SYSTEM] Free Heap : "); Serial.print(ESP.getFreeHeap() / 1024); Serial.println(" KB");

    pref.begin("lamp_data", false);
    targetBrightness = pref.getInt("last_bright", 0);
    brightness = targetBrightness;
    Serial.print("[MEMORI] Last Brightness: "); Serial.println(brightness);

    ledcAttach(PIN_LED, 5000, 8); 
    ledcWrite(PIN_LED, brightness);

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), handleButton, FALLING);
    Serial.println("[INPUT] External Interrupt Initialized.");

    WiFi.begin(ssid, password);
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    xTaskCreatePinnedToCore(taskHardware, "HW_Task", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskNetwork, "Net_Task", 4096, NULL, 1, NULL, 0);
    
    Serial.println("[SYSTEM] Multitasking Kernel Running...");
    Serial.println("==============================================\n");
}

void loop() { 
}