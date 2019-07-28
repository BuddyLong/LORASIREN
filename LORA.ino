#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

void onReceive(int packetSize);
void LoRaSend(String message);
void ISHandleInterruptR();

// LORA RADIO PINS
#define LORA_BAND    868
#define SCK     15    // GPIO5  -- SX1278's SCK
#define MISO    14   // GPIO19 -- SX1278's MISO
#define MOSI    16   // GPIO27 -- SX1278's MOSI
#define CS      8   // GPIO18 -- SX1278's CS
#define RST     4   // GPIO14 -- SX1278's RESET
#define DI0     7   // GPIO26 -- SX1278's IRQ(Interrupt Request)

// Alarm side pins
#define SIRENALARM   18 // INPUT
#define ACFAIL 19 // OUTPUT
#define TAMPER 20 // OUTPUT
#define ALARM 21 // OUTPUT
#define PANIC 22 // OUTPUT

// bool AlarmState = 0;
bool SirenState = 0;
bool ACState = 0;
bool TamperState = 0;
bool AlarmState = 0;
bool PanicState = 0;

void setup() {
  //Pin setup
  pinMode(SIRENALARM,INPUT);
  pinMode(ACFAIL,OUTPUT);
  pinMode(TAMPER,OUTPUT);
  pinMode(ALARM,OUTPUT);
  pinMode(PANIC, OUTPUT);

  // Pin Interrupt Setup
  // attachInterrupt(digitalPinToInterrupt(SIRENALARM), HandleInterrupt, CHANGE);
  // Serial Setup
  Serial.begin(9600);   //9600 Baud
  while (!Serial);
  Serial.println("Serial Success");


  // Configure LORA Radio
  SPI.begin();
  LoRa.setPins(CS, RST, DI0);
  LoRa.setTxPower(14);
  LoRa.setSpreadingFactor(6);
  LoRa.onReceive(onReceive);
  if (!LoRa.begin(LORA_BAND * 1E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.idle();
  Serial.println("Initialize succeeded");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (ACState == 0) {
    digitalWrite(ACFAIL,LOW);
  }
  if (ACState == 1) {
    digitalWrite(ACFAIL,HIGH);
  }

  if (TamperState == 0) {
    digitalWrite(TAMPER,LOW);
  }
  if (TamperState == 1) {
    digitalWrite(TAMPER,HIGH);
  }

  if (AlarmState == 0) {
    digitalWrite(ALARM, LOW);
  }
  if (AlarmState == 1) {
    digitalWrite(ALARM, HIGH);
    delay(2000);
    digitalWrite(ALARM, LOW);
  }

  if (PanicState == 0) {
    digitalWrite(PANIC, LOW);
  }
  if (PanicState == 1) {
    digitalWrite(PANIC, HIGH);
    delay(2000);
    digitalWrite(PANIC, LOW);
  }


  SirenState = digitalRead(SIRENALARM);
  LoRaSend("W" + String(SirenState));
  Serial.println("W" + String(SirenState));
  delay(1000);
}

void LoRaSend(String message){
  LoRa.idle();
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
  LoRa.receive();
}

void onReceive(int packetSize) {
  String message = "";
  while (LoRa.available()) {
    message += (char)LoRa.read();
  }
  Serial.print("Node Recieve: ");
  Serial.println(message);

  if (message.charAt(0) == 'W') {
    if (message.charAt(1) == '0') {
      ACState = 0;
    }
    if (message.charAt(1) == '1') {
      ACState = 1;
    }

    if (message.charAt(2) == '0') {
      TamperState = 0;
    }
    if (message.charAt(2) == '1') {
      TamperState = 1;
    }

    if (message.charAt(3) == '0') {
      AlarmState = 0;
    }
    if (message.charAt(3) == '1') {
      AlarmState = 1;
    }

    if (message.charAt(4) == '0') {
      PanicState = 0;
    }
    if (message.charAt(4) == '1') {
      PanicState = 1;
    }
      Serial.println(String(PanicState));
    Serial.println(String(TamperState));
    Serial.println(String(ACState));
    Serial.println(String(AlarmState));
    }

    
  }
  
// }


