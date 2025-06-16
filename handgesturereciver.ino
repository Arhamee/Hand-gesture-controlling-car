#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Initialize NRF24L01
RF24 radio(9, 10); // CE, CSN pins
const uint64_t PipeIn = 0xE8E8F0F0E1LL; // Same as transmitter

// Motor pins
#define in1 3
#define in2 4
#define in3 7
#define in4 8
#define enA 5
#define enB 6

// Variables for signal loss detection
unsigned long lastSignalTime = 0;
const unsigned long signalLossTimeout = 1000; // 1 second timeout

// Thresholds for movement
const float forwardThreshold = 3.0;  
const float backwardThreshold = -3.0; 
const float leftThreshold = -4.0;   
const float rightThreshold = 4.0;   

void setup() {
  Serial.begin(9600);

  // Initialize NRF24L01
  radio.begin();
  radio.openReadingPipe(1, PipeIn);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  // Set motor pins as output
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  // Initialize motors to stop
  moveStop();
}

void loop() {
  if (radio.available()) {
    float data[3];
    radio.read(&data, sizeof(data)); // Read the data from the transmitter

    // Update the last signal time
    lastSignalTime = millis();

    // Extract X and Y values (ignore Z)
    float x = data[1];
    float y = data[0];

    // Print the received values for debugging
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(", Y: ");
    Serial.println(y);

    if (y > forwardThreshold) {
      moveForward(); 
    } else if (y < backwardThreshold) {
      moveBackward(); 
    } else if (x < leftThreshold) {
      turnLeft(); 
    } else if (x > rightThreshold) {
      turnRight(); 
    } else {
      moveStop(); 
    }
  } else {
    // Check for signal loss
    if (millis() - lastSignalTime > signalLossTimeout) {
      moveStop(); // Stop motors if signal is lost for 1 second
      Serial.println("Signal lost! Motors stopped.");
    }
  }
}

void moveStop() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void moveForward() {
  analogWrite(enA, 250);
  analogWrite(enB, 250);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void moveBackward() {
  analogWrite(enA, 250);
  analogWrite(enB, 250);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void turnRight() {
  analogWrite(enA, 200); // Left motor speed
  analogWrite(enB, 200); // Right motor speed
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); // Right motor backward 
  digitalWrite(in4, LOW);
}

void turnLeft() {
  analogWrite(enA, 200);
  analogWrite(enB, 200);
  digitalWrite(in1, HIGH); // Left motor backward 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}