#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <RF24.h>

char auth[] = "Your Token"; // 
char ssid[] = "";        // 
char pass[] = "";    // 

#define CE_PIN   D3
#define CSN_PIN  D4

RF24 radio(CE_PIN, CSN_PIN);

const int sensorPin = A0; // Pin analog untuk sensor tegangan
const float R1 = 30000.0;  // Nilai resistor R1 (ohm)
const float R2 = 7500.0;   // Nilai resistor R2 (ohm)


const float teganganReferensiPengirim = 12.28;
const float teganganReferensiPenerima = 12.53;
const float toleransi = 1.0; // Toleransi 1V

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Blynk.begin(auth, ssid, pass);
  
  radio.begin();
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL); 
  radio.setPALevel(RF24_PA_HIGH);          
  radio.startListening();
  
  Serial.println("ESP8266 Terhubung ke Blynk");
}

void loop() {
  Blynk.run();

  for (int i = 0; i < 5; i++) {
    if (radio.available()) {
      float data[3];
      radio.read(&data, sizeof(data));

      float xAngle = data[0];
      float yAngle = data[1];
      float voltage = data[2];


      float localVoltage = readVoltage();


      bool pengirimValid = fabs(voltage - teganganReferensiPengirim) <= toleransi;

      bool penerimaValid = fabs(localVoltage - teganganReferensiPenerima) <= toleransi;

      
      Serial.print("Pengujian ke-"); Serial.print(i + 1);
      Serial.print(": Sudut X: "); Serial.print(xAngle);
      Serial.print("\tSudut Y: "); Serial.print(yAngle);
      Serial.print("\tTegangan Arduino: "); Serial.print(voltage);
      Serial.print(pengirimValid ? " (Valid)" : " (Tidak Valid)");
      Serial.print("\tTegangan ESP8266: "); Serial.print(localVoltage);
      Serial.println(penerimaValid ? " (Valid)" : " (Tidak Valid)");


      Blynk.virtualWrite(V1, xAngle);
      Blynk.virtualWrite(V2, yAngle);
      Blynk.virtualWrite(V3, voltage);
      Blynk.virtualWrite(V4, localVoltage);

      delay(1000);
    }
  }
}

float readVoltage() {
  int sensorValue = analogRead(sensorPin);
  float vout = (sensorValue * 3.18) / 1023.0; // 
  float voltage = vout / (R2 / (R1 + R2)); // 
  return voltage;
}
