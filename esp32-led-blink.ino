/******************************************************************************
 * PROJECT      : ESP32 LED Blink
 * DESCRIPTION  : Blinks the onboard LED (GPIO2) on and off every 1 second.
 *                The simplest possible ESP32 firmware — a "Hello, World!"
 *                for embedded systems.
 *
 * HARDWARE     : ESP32 DevKit
 *                LED + 220Ω resistor on GPIO2 → GND  (or use onboard LED)
 *
 * CODED BY     : SWAN LAB, IIT Kharagpur
 ******************************************************************************/


/* ─── PIN CONFIGURATION ──────────────────────────────────────────────────── */

const int LED_PIN    = 2;     // GPIO2 — onboard LED on most ESP32 DevKit boards
const int BLINK_MS   = 1000;  // ON / OFF duration in milliseconds


/* ─────────────────────────────────────────────────────────────────────────── *
 *  SETUP — runs once on power-up / reset                                     *
 * ─────────────────────────────────────────────────────────────────────────── */
void setup()
{
    Serial.begin(115200);             // ESP32 default baud rate
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);       // Start with LED off
    Serial.println("SWAN LAB — ESP32 Blink ready");
}


/* ─────────────────────────────────────────────────────────────────────────── *
 *  LOOP — runs continuously                                                   *
 * ─────────────────────────────────────────────────────────────────────────── */
void loop()
{
    digitalWrite(LED_PIN, HIGH);      // LED ON
    Serial.println("LED: ON");
    delay(BLINK_MS);

    digitalWrite(LED_PIN, LOW);       // LED OFF
    Serial.println("LED: OFF");
    delay(BLINK_MS);
}
