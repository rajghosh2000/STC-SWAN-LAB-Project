/******************************************************************************
 * PROJECT      : ESP32 LDR → MQTT Publisher
 * DESCRIPTION  : Extends Part 2 with WiFi + MQTT. Reads the LDR every 500 ms,
 *                controls the LED locally, and publishes a JSON payload to an
 *                MQTT broker. A Python script subscribes and plots data live.
 *
 * MQTT TOPICS  :
 *   swan/ldr/data   → {"ldr": 2048, "state": "DARK",  "timestamp": 12345}
 *   swan/ldr/status → "online" / "offline"  (LWT message)
 *
 * DEPENDENCIES : Install via Arduino Library Manager:
 *   - PubSubClient  by Nick O'Leary   (MQTT)
 *   - ArduinoJson   by Benoit Blanchon (JSON)
 *
 * BROKER       : Use a free public broker for testing:
 *                broker.hivemq.com  port 1883  (no auth)
 *                Or run Mosquitto locally: pip install mosquitto
 *
 * HARDWARE     : ESP32 DevKit
 *                LDR + 10kΩ pull-down on GPIO34
 *                LED + 220Ω resistor on GPIO2
 *
 * CODED BY     : SWAN LAB, IIT Kharagpur
 ******************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


/* ─── WIFI CREDENTIALS — change these ───────────────────────────────────── */

const char* WIFI_SSID     = "REMPIFY";
const char* WIFI_PASSWORD = "12345689";


/* ─── MQTT BROKER SETTINGS ───────────────────────────────────────────────── */

const char* MQTT_BROKER   = "broker.hivemq.com";  // Free public broker
const int   MQTT_PORT     = 1883;
const char* MQTT_CLIENT   = "SWAN_LAB_ESP32";      // Must be unique per device
const char* TOPIC_DATA    = "swan/ldr/data";        // LDR readings go here
const char* TOPIC_STATUS  = "swan/ldr/status";      // LWT (last-will) topic


/* ─── PIN & THRESHOLD CONFIGURATION ─────────────────────────────────────── */

const int LDR_PIN        = 34;
const int LED_PIN        = 2;
const int DARK_THRESHOLD = 15;   // 0–4095 · tune for your environment
const int PUBLISH_MS     = 500;    // Publish interval in milliseconds


/* ─── GLOBALS ────────────────────────────────────────────────────────────── */

WiFiClient   wifiClient;
PubSubClient mqttClient(wifiClient);
unsigned long lastPublish = 0;


/* ─── WIFI CONNECT ───────────────────────────────────────────────────────── */
void connectWiFi()
{
    Serial.printf("\nConnecting to WiFi: %s", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.printf("\nWiFi connected — IP: %s\n", WiFi.localIP().toString().c_str());
}


/* ─── MQTT CONNECT ───────────────────────────────────────────────────────── */
void connectMQTT()
{
    while (!mqttClient.connected())
    {
        Serial.print("Connecting to MQTT broker...");

        /* Last-Will message: broker sends "offline" if ESP32 drops          */
        bool connected = mqttClient.connect(
            MQTT_CLIENT,
            nullptr, nullptr,            // No username / password
            TOPIC_STATUS, 0, true,       // LWT: topic, QoS, retain
            "offline"
        );

        if (connected)
        {
            Serial.println(" connected!");
            mqttClient.publish(TOPIC_STATUS, "online", true);  // Announce online
        }
        else
        {
            Serial.printf(" failed (rc=%d) — retry in 3s\n", mqttClient.state());
            delay(3000);
        }
    }
}


/* ─────────────────────────────────────────────────────────────────────────── *
 *  SETUP                                                                      *
 * ─────────────────────────────────────────────────────────────────────────── */
void setup()
{
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);

    connectWiFi();

    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setBufferSize(256);   // Enough for our JSON payload

    Serial.println("SWAN LAB — ESP32 LDR MQTT Publisher ready");
}


/* ─────────────────────────────────────────────────────────────────────────── *
 *  LOOP                                                                       *
 * ─────────────────────────────────────────────────────────────────────────── */
void loop()
{
    /* ── Maintain connections ────────────────────────────────────────────── */
    if (WiFi.status() != WL_CONNECTED)  connectWiFi();
    if (!mqttClient.connected())        connectMQTT();
    mqttClient.loop();   // Must be called regularly to keep MQTT alive

    /* ── Read sensor ─────────────────────────────────────────────────────── */
    int ldrValue = analogRead(LDR_PIN);
    bool isDark  = (ldrValue <= DARK_THRESHOLD);

    /* ── Control LED ─────────────────────────────────────────────────────── */
    digitalWrite(LED_PIN, isDark ? HIGH : LOW);

    /* ── Publish at interval ─────────────────────────────────────────────── */
    unsigned long now = millis();
    if (now - lastPublish >= PUBLISH_MS)
    {
        lastPublish = now;

        /* Build JSON payload                                                 */
        StaticJsonDocument<128> doc;
        doc["ldr"]       = ldrValue;
        doc["state"]     = isDark ? "DARK" : "LIGHT";
        doc["threshold"] = DARK_THRESHOLD;
        doc["uptime_ms"] = now;

        char payload[128];
        serializeJson(doc, payload);

        /* Publish and print                                                  */
        mqttClient.publish(TOPIC_DATA, payload);
        Serial.printf("[MQTT] %s → %s\n", TOPIC_DATA, payload);
    }
}
