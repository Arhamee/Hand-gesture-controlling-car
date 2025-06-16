#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Create an instance of the ADXL345 sensor
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(0x53); 

int outMessage[3];

RF24 radio(9, 10); // CE, CSN pins
const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL;

void setup() {
  Serial.begin(9600); 

  // Initialize the ADXL345
  if (!accel.begin()) {
    Serial.println("Could not find ADXL345 sensor!");
    while (1); // Stop if the sensor is not found
  }

  radio.begin();
  radio.openWritingPipe(my_radio_pipe);
  radio.setPALevel(RF24_PA_MIN); 
  radio.stopListening(); // Set as transmitter
}

void loop() {
  // Read accelerometer data
  sensors_event_t event;
  accel.getEvent(&event);

  // Store acceleration values in an array
  float outMessage[3] = {event.acceleration.x, event.acceleration.y, event.acceleration.z};

  // Print the acceleration values to Serial Monitor (for debugging)
  Serial.print("X: ");
  Serial.print(outMessage[0]);
  Serial.print(", Y: ");
  Serial.print(outMessage[1]);
  Serial.print(", Z: ");
  Serial.println(outMessage[2]);

  // Send data via NRF24L01
  bool status = radio.write(&outMessage, sizeof(outMessage));
  if (status) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.println("Failed to send data.");
  }
  delay(50); // Small delay for stability
}