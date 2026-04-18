/******************************************************************************
 * PROJECT      : Automatic Light Sensor (LDR-Controlled LED)
 * DESCRIPTION  : Reads ambient light levels using an LDR (Light Dependent
 *                Resistor) connected to analog pin A0. When the light level
 *                drops below a set threshold (dark environment), the LED on
 *                pin 12 is automatically switched ON. When sufficient light
 *                is detected, the LED switches OFF.
 *
 *                Ideal for automatic night lamps, street light simulations,
 *                and light-sensitive trigger systems.
 *
 * HARDWARE     : Arduino Uno / Nano (or compatible)
 *                LDR connected between A0 and GND (with pull-up resistor)
 *                LED + 1KΩ resistor connected between Pin 12 and GND
 *
 CODED BY     : SWAN LAB, IIT Kharagpur
 *                & Contributors
 *
 * WEBSITE      : https://cse.iitkgp.ac.in/~smisra/swan/
 ******************************************************************************/


/* ─── PIN CONFIGURATION ──────────────────────────────────────────────────── */

const int LED_PIN = 12;    // Digital output pin where the LED is connected
const int LDR_PIN = A0;    // Analog input pin where the LDR is connected


/* ─── THRESHOLD CONFIGURATION ────────────────────────────────────────────── */

const int DARK_THRESHOLD = 5;  // Analog readings at or below this value (0–1023)
                                // indicate darkness → LED should turn ON


/* ─────────────────────────────────────────────────────────────────────────── *
 *  SETUP                                                                      *
 *  Runs once on power-up or reset.                                            *
 *  Initialises serial communication for live monitoring and configures        *
 *  the LED pin as OUTPUT and the LDR pin as INPUT.                            *
 * ─────────────────────────────────────────────────────────────────────────── */
void setup()
{
    Serial.begin(9600);             // Start serial monitor at 9600 baud for debugging

    pinMode(LED_PIN, OUTPUT);       // LED pin drives current → must be OUTPUT
    pinMode(LDR_PIN, INPUT);        // LDR pin only reads voltage → must be INPUT
}


/* ─────────────────────────────────────────────────────────────────────────── *
 *  LOOP                                                                       *
 *  Runs continuously after setup() completes.                                 *
 *  Each iteration:                                                            *
 *    1. Reads the analog voltage from the LDR (range: 0 – 1023)              *
 *    2. Prints the raw value to Serial Monitor for live debugging             *
 *    3. Compares the value against DARK_THRESHOLD:                            *
 *         • At or below threshold → environment is DARK  → LED ON            *
 *         • Above threshold       → environment is BRIGHT → LED OFF           *
 * ─────────────────────────────────────────────────────────────────────────── */
void loop()
{
    /* ── Step 1 : Read LDR ───────────────────────────────────────────────── */
    int ldrStatus = analogRead(LDR_PIN);  // Sample analog voltage (0 = dark, 1023 = bright)

    /* ── Step 2 : Debug Output ───────────────────────────────────────────── */
    Serial.println(ldrStatus);            // Print raw LDR value to Serial Monitor

    /* ── Step 3 : Light / Dark Decision ─────────────────────────────────── */
    if (ldrStatus <= DARK_THRESHOLD)
    {
        digitalWrite(LED_PIN, HIGH);      // Dark detected → switch LED ON
    }
    else
    {
        digitalWrite(LED_PIN, LOW);       // Light detected → switch LED OFF
    }

    /* Loop repeats immediately — sensor is polled as fast as Arduino allows */
}
