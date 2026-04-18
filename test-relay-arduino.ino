/******************************************************************************
 * PROJECT      : Test Relay
 * CODED BY     : SWAN LAB, IIT Kharagpur
 *                & Contributors
 *
 * WEBSITE      : https://cse.iitkgp.ac.in/~smisra/swan/
 ******************************************************************************/
// Quick relay test — watch the onboard LED and listen for the click
void setup() {
    pinMode(8, OUTPUT);
    digitalWrite(8, LOW);   // Try LOW first — relay should click ON
    delay(2000);
    digitalWrite(8, HIGH);  // Relay should click OFF
    delay(2000);
}
void loop() {}
