/******************************************************************************
 * PROJECT      : Automatic Light Sensor (LDR + 10 A Relay)
 * DESCRIPTION  : Reads ambient light via an LDR on A0. When darkness falls
 *                below DARK_THRESHOLD the 5 V relay module on Pin 8 closes
 *                its NO contact, switching a high-power load (bulb, fan,
 *                pump…) ON. An indicator LED on Pin 12 mirrors relay state.
 *                The relay module is ACTIVE LOW:
 *                  LOW  → relay energised  (load ON)
 *                  HIGH → relay released   (load OFF)
 *
 * HARDWARE     : Arduino Uno / Nano
 *                LDR + 10 kΩ pull-down  → A0
 *                5 V 1-Ch Relay Module  → Pin 8  (IN / VCC / GND)
 *                LED + 220 Ω resistor   → Pin 12
 *
 * CODED BY     : SWAN LAB, IIT Kharagpur
 ******************************************************************************/


/* ─── PIN CONFIGURATION ──────────────────────────────────────────────────── */

const int LDR_PIN   = A0;   // Analog input  – LDR voltage divider output
const int RELAY_PIN = 8;    // Digital output – relay module IN  (active LOW)
const int LED_PIN   = 12;   // Digital output – indicator LED


/* ─── THRESHOLD ───────────────────────────────────────────────────────────── *
 *  Open Serial Monitor at 9600 baud, cover / uncover the LDR, and note the  *
 *  printed values. Set DARK_THRESHOLD somewhere between the lit and dark     *
 *  readings to get reliable switching in your environment.                   *
 * ─────────────────────────────────────────────────────────────────────────── */

const int DARK_THRESHOLD = 5;  // 0 – 1023 · below = dark · above = bright


/* ─── ACTIVE LOW HELPERS ─────────────────────────────────────────────────── *
 *  Using named macros means the code reads exactly as you think it:         *
 *  "turn the relay ON" — no need to remember which logic level does what.   *
 * ─────────────────────────────────────────────────────────────────────────── */

#define RELAY_ON  LOW    // LOW  energises the coil on this module
#define RELAY_OFF HIGH   // HIGH releases  the coil on this module


/* ─────────────────────────────────────────────────────────────────────────── *
 *  SETUP — runs once on power-up / reset                                     *
 *  All pins configured; relay and LED forced OFF so the load never           *
 *  gets an accidental power spike during start-up.                           *
 * ─────────────────────────────────────────────────────────────────────────── */
void setup()
{
    Serial.begin(9600);               // Open serial port for live LDR monitoring

    pinMode(LDR_PIN,   INPUT);        // A0  reads the LDR voltage divider
    pinMode(RELAY_PIN, OUTPUT);       // Pin 8 drives the relay coil
    pinMode(LED_PIN,   OUTPUT);       // Pin 12 drives the indicator LED

    digitalWrite(RELAY_PIN, RELAY_OFF);  // Safe default – load stays OFF at boot
    digitalWrite(LED_PIN,   LOW);        // Indicator OFF at boot
}


/* ─────────────────────────────────────────────────────────────────────────── *
 *  LOOP — runs continuously                                                   *
 *                                                                             *
 *  Step 1 → Read LDR (0 = pitch dark, 1023 = full brightness)               *
 *  Step 2 → Print value to Serial Monitor for easy calibration               *
 *  Step 3 → Compare with DARK_THRESHOLD and act:                             *
 *             DARK   (≤ threshold) → Relay ON  + LED ON                      *
 *             BRIGHT (> threshold) → Relay OFF + LED OFF                     *
 * ─────────────────────────────────────────────────────────────────────────── */
void loop()
{
    /* ── Step 1 : Read LDR ───────────────────────────────────────────────── */
    int ldrStatus = analogRead(LDR_PIN);   // Sample analog voltage (10-bit, 0–1023)

    /* ── Step 2 : Serial Debug ───────────────────────────────────────────── */
    Serial.print("LDR Value: ");
    Serial.println(ldrStatus);             // Watch this live to tune DARK_THRESHOLD

    /* ── Step 3 : Control Relay + LED ───────────────────────────────────── */
    if (ldrStatus <= DARK_THRESHOLD)
    {
        digitalWrite(RELAY_PIN, RELAY_ON);   // Energise coil → NO contact closes → load ON
        digitalWrite(LED_PIN,   HIGH);        // Indicator LED confirms relay is active
        Serial.println("State: DARK  → Relay ON");
    }
    else
    {
        digitalWrite(RELAY_PIN, RELAY_OFF);  // Release coil → NO contact opens → load OFF
        digitalWrite(LED_PIN,   LOW);         // Indicator LED off
        Serial.println("State: LIGHT → Relay OFF");
    }

    /* Loop restarts immediately — sensor polled as fast as Arduino allows */
}
