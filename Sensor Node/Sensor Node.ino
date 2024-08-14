#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define CE_PIN   9
#define CSN_PIN  10

RF24 radio(CE_PIN, CSN_PIN);

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

const int sensorPin = A0; // Pin analog untuk sensor tegangan
const float R1 = 30000.0;  // Nilai resistor R1 (ohm)
const float R2 = 7500.0;   // Nilai resistor R2 (ohm)

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!accel.begin()) {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    while (1);
  }

  radio.begin();
  radio.openWritingPipe(0xF0F0F0F0E1LL); // 
  radio.setPALevel(RF24_PA_HIGH);        // 
  radio.stopListening();

  Serial.println("Pengirim Siap");
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  // Membaca nilai akselerometer untuk sumbu X, Y, Z
  float xAccel = event.acceleration.x;
  float yAccel = event.acceleration.y;
  float zAccel = event.acceleration.z;

  // Menghitung sudut X dan sudut Y
  float xAngle = atan2(yAccel, zAccel) * 180.0 / PI;
  float yAngle = atan2(-xAccel, sqrt(yAccel * yAccel + zAccel * zAccel)) * 180.0 / PI;

  // Membaca tegangan
  float voltage = readVoltage(); 

  // Mengirimkan hanya sudut X, sudut Y, dan tegangan melalui radio
  float data[3] = {xAngle, yAngle, voltage};
  radio.write(&data, sizeof(data));

  // Menampilkan nilai akselerometer, sudut, dan tegangan
  Serial.print("Data Terkirim: ");
  Serial.print("Sudut X: "); Serial.print(xAngle);
  Serial.print("\tSudut Y: "); Serial.print(yAngle);
  Serial.print("\tTegangan: "); Serial.println(voltage);

  delay(1000);
}

float readVoltage() {
  int sensorValue = analogRead(sensorPin);
  float vout = (sensorValue * 4.95) / 1023.0; // 
  float voltage = vout / (R2 / (R1 + R2)); // 
  return voltage;
}
