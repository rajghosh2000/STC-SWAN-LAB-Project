/******************************************************************************
 * PROJECT      : ESP32 LDR Auto-Light
 * DESCRIPTION  : Reads ambient light level from an LDR on GPIO34 (ADC input).
 *                When darkness is detected (value ≤ DARK_THRESHOLD), the LED
 *                on GPIO2 switches ON. Bright environment → LED OFF.
 *
 *                LDR wiring (voltage divider):
 *                  3.3V → LDR → GPIO34 → 10kΩ → GND
 *                As light increases, resistance drops → voltage at GPIO34 rises.
 *
 * NOTE         : On ESP32, use GPIO34–39 for ADC inputs — they are input-only
 *                pins and work best for analog reads. Avoid GPIO36 (VP) if
 *                using WiFi simultaneously (causes ADC noise).
 *
 * HARDWARE     : ESP32 DevKit
 *                LDR + 10kΩ pull-down on GPIO34
 *                LED + 220Ω resistor on GPIO2
 *
 * CODED BY     : SWAN LAB, IIT Kharagpur
 ******************************************************************************/


/* ─── PIN CONFIGURATION ──────────────────────────────────────────────────── */

const int LDR_PIN  = 34;    // GPIO34 — ADC1_CH6, input-only, no pull-up
const int LED_PIN  = 2;     // GPIO2  — indicator LED


/* ─── THRESHOLD ──────────────────────────────────────────────────────────── *
 *  ESP32 ADC is 12-bit → range 0–4095                                        *
 *  Open Serial Monitor at 115200, cover / expose LDR, note values,           *
 *  then set threshold between your dark and bright readings.                 *
 * ─────────────────────────────────────────────────────────────────────────── */

const int DARK_THRESHOLD = 20;  // Below = dark → LED ON  (tune for your LDR)


/* ─────────────────────────────────────────────────────────────────────────── *
 *  SETUP                                                                      *
 * ─────────────────────────────────────────────────────────────────────────── */
void setup()
{
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    /* ESP32 ADC configuration                                                */
    analogReadResolution(12);         // 12-bit resolution: 0–4095
    analogSetAttenuation(ADC_11db);   // Full range: 0–3.3V input

    Serial.println("SWAN LAB — ESP32 LDR Auto-Light ready");
    Serial.println("Open plotter or monitor to calibrate DARK_THRESHOLD");
}


/* ─────────────────────────────────────────────────────────────────────────── *
 *  LOOP                                                                       *
 * ─────────────────────────────────────────────────────────────────────────── */
void loop()
{
    /* ── Read LDR ────────────────────────────────────────────────────────── */
    int ldrValue = analogRead(LDR_PIN);   // 0 (dark) → 4095 (bright)

    /* ── Serial output (use Arduino Serial Plotter to visualise) ─────────── */
    Serial.print("LDR:");
    Serial.print(ldrValue);
    Serial.print("  Threshold:");
    Serial.println(DARK_THRESHOLD);       // Flat reference line in plotter

    /* ── LED control ─────────────────────────────────────────────────────── */
    if (ldrValue <= DARK_THRESHOLD)
    {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("State: DARK  → LED ON");
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
        Serial.println("State: LIGHT → LED OFF");
    }

    delay(300);   // 300 ms polling interval — smooth without flooding serial
}
