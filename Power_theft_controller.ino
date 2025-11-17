#define BLYNK_TEMPLATE_ID "Provide template here"
#define BLYNK_TEMPLATE_NAME "IOTpower"
#define BLYNK_AUTH_TOKEN "Provide Auth Token Here"

#include <BlynkSimpleStream.h> // Blynk library for Serial communication
// Blynk Auth Token
char auth[] = "BLYNK_AUTH_TOKEN";

// Pin definitions
int tamperSensor = 2;      // Tamper sensor input pin
int buzzer = 3;            // Buzzer pin
int relay = 4;             // Relay control pin
int currentSensor = A0;    // Current sensor analog input pin
int voltageSensor = A1;    // Voltage sensor analog input pin

// Variables for measurements
float voltage = 0.0;       // Variable to store voltage value
float current = 0.0;       // Variable to store current value

// Calibration constants (update based on your sensors)
const float currentSensorSensitivity = 0.185;  // For ACS712-5A: 185mV/A
const float voltageSensorMultiplier = 5.0;     // Voltage divider ratio 

// Center voltage for current sensor (for ACS712, typically 2.5V at 0A)
const float centerVoltage = 2.5;

// Blynk virtual pin mappings
#define VIRTUAL_PIN_RELAY V0
#define VIRTUAL_PIN_VOLTAGE V1
#define VIRTUAL_PIN_CURRENT V2

void setup() {
  // Initialize Serial communication (for COMPIM and Blynk)
  Serial.begin(9600);
  Blynk.begin(Serial, auth);

  // Pin mode setup
  pinMode(tamperSensor, INPUT);    // Set tamper sensor pin as input
  pinMode(buzzer, OUTPUT);         // Set buzzer pin as output
  pinMode(relay, OUTPUT);          // Set relay pin as output

  // Ensure relay and buzzer are off initially
  digitalWrite(relay, LOW);
  digitalWrite(buzzer, LOW);
}

void loop() {
  // Read voltage and current
  voltage = analogRead(voltageSensor) * (5.0 / 1023.0) * voltageSensorMultiplier;
  float rawCurrent = analogRead(currentSensor) * (5.0 / 1023.0); 
  current = (rawCurrent - centerVoltage) / currentSensorSensitivity;

  // Update Blynk virtual pins
  Blynk.virtualWrite(VIRTUAL_PIN_VOLTAGE, voltage);  // Send voltage to Blynk app
  Blynk.virtualWrite(VIRTUAL_PIN_CURRENT, current);  // Send current to Blynk app

  // Check for tampering
  if (digitalRead(tamperSensor) == HIGH) {
    Serial.println("Tampering detected!");

    // Activate relay to disconnect power and turn on the buzzer
    digitalWrite(relay, HIGH);    // Trigger relay
    digitalWrite(buzzer, HIGH);   // Turn on buzzer
  } else {
    // Ensure relay and buzzer are off when no tampering
    digitalWrite(relay, LOW);
    digitalWrite(buzzer, LOW);
  }

  // Allow Blynk to handle app interactions
  Blynk.run();
}

// Blynk app relay control (V0)
BLYNK_WRITE(VIRTUAL_PIN_RELAY) {
  int relayState = param.asInt(); // Get relay state from Blynk app
  digitalWrite(relay, relayState); // Set relay based on app input
}
