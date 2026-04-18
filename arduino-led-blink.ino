/********************************************************************************
 * PROJECT      : Blinking LED
 * DESCRIPTION  : Blinks an LED connected to pin 9 — turns it ON for user-defined 
 *                "X' seconds, then OFF for 1.5 seconds, repeating indefinitely.
 *                A classic embedded systems "Hello, World!" demonstrating
 *                basic digital output and timing control on Arduino.
 *
 * HARDWARE     : Arduino Uno / Nano (or compatible)
 *                LED + 1KΩ resistor connected between Pin 9 and GND
 *
 * CODED BY     : SWAN LAB, IIT Kharagpur
 *                & Contributors
 *
 * WEBSITE      : https://cse.iitkgp.ac.in/~smisra/swan/
 ********************************************************************************/


/* ─── PIN CONFIGURATION ──────────────────────────────────────────────────── */

const int LED_PIN = 9;       // Digital output pin where the LED is connected

/* ─── TIMING CONFIGURATION ───────────────────────────────────────────────── */

const int BLINK_DELAY = 1500; // Duration (in milliseconds) for each ON / OFF phase


/* ─────────────────────────────────────────────────────────────────────────── *
 *  SETUP                                                                      *
 *  Runs once when the board powers on or resets.                              *
 *  Configures LED_PIN as a digital output so we can drive it HIGH or LOW.     *
 * ─────────────────────────────────────────────────────────────────────────── */
void setup()
{
    pinMode(LED_PIN, OUTPUT);   // Set LED pin as OUTPUT to control voltage level
}


/* ─────────────────────────────────────────────────────────────────────────── *
 *  LOOP                                                                        *
 *  Runs continuously after setup() completes.                                 *
 *  Each iteration performs one full blink cycle:                              *
 *    1. Drive the pin HIGH  → LED turns ON                                    *
 *    2. Wait BLINK_DELAY ms → LED stays ON  for some "X" s                        *
 *    3. Drive the pin LOW   → LED turns OFF                                   *
 *    4. Wait BLINK_DELAY ms → LED stays OFF for some "X" s                        *
 * ─────────────────────────────────────────────────────────────────────────── */
void loop()
{
    /* ── Phase 1 : LED ON ─────────────────────────────────────────────────── */
    digitalWrite(LED_PIN, HIGH);  // Apply 5 V to the pin → current flows → LED lights up
    delay(BLINK_DELAY);           // Hold the ON state for BLINK_DELAY milliseconds

    /* ── Phase 2 : LED OFF ────────────────────────────────────────────────── */
    digitalWrite(LED_PIN, LOW);   // Pull pin to 0 V → no current → LED goes dark
    delay(BLINK_DELAY);           // Hold the OFF state for BLINK_DELAY milliseconds

    /* Loop returns to the top automatically — blinking continues forever */
}
