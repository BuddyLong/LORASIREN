#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

void onReceive(int packetSize);
void LoRaSend(String message);

// LORA RADIO PINS
#define LORA_BAND    868
#define SCK     15    // GPIO5  -- SX1278's SCK
#define MISO    14   // GPIO19 -- SX1278's MISO
#define MOSI    16   // GPIO27 -- SX1278's MOSI
#define CS      8   // GPIO18 -- SX1278's CS
#define RST     4   // GPIO14 -- SX1278's RESET
#define DI0     7   // GPIO26 -- SX1278's IRQ(Interrupt Request)

// Siren side pins
#define SIRENALARM   2 // OUTPUT
#define ACFAIL 19 // INPUT
#define TAMPER 20 // INPUT
#define ALARM 21 // INPUT
#define PANIC 22 // INPUT


// State VARS
bool SirenState = 0;
bool ACState = 0;
bool TamperState = 0;
bool AlarmState = 0;
bool PanicState = 0;
bool SendRequest = 0;
String stateMessage = "";

// ALARM VARS
bool AlarmButtonFlag = 0;
unsigned long AlarmStartmillis = 0; 
unsigned long currentAlarmMillis = 0;
#define TRIGTIME 2000

// Panic VARS
bool PanicButtonFlag = 0;
unsigned long PanicStartmillis = 0; 
unsigned long currentPanicMillis = 0;

void setup() {
  // Pin Setup
  pinMode(SIRENALARM,OUTPUT);
  pinMode(ACFAIL,INPUT);
  pinMode(TAMPER,INPUT);
  pinMode(ALARM,INPUT);
  pinMode(PANIC, INPUT);

  Serial.begin(9600);   //9600 Baud
  while (!Serial);
  Serial.println("Serial Success");


  // Configure LORA Radio
//  SPI.begin(SCK, MISO, MOSI, CS);
  SPI.begin();
  LoRa.setPins(CS, RST, DI0);
  LoRa.setTxPower(14);
  LoRa.setSpreadingFactor(6);
  LoRa.onReceive(onReceive);

  if (!LoRa.begin(LORA_BAND * 1E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("Initialize succeeded");
  LoRa.receive();
}

void loop() {
  // Get states
  stateMessage = "";
    //ALARM
    if (digitalRead(ALARM) == 1)        {
        if (AlarmButtonFlag == 0) {
            AlarmButtonFlag = 1;
            AlarmStartmillis = millis();
            Serial.println("Start" + String(AlarmStartmillis)); 
        }
        
        // Serial.println(String(currentMillis - AlarmStartmillis));
        currentAlarmMillis = millis();
        if ((currentAlarmMillis - AlarmStartmillis >= TRIGTIME) and (AlarmButtonFlag == 1)) {
            Serial.println("Stop" + String(currentAlarmMillis));
            Serial.println("Alarm Triggered");
            AlarmState = 1;
            AlarmButtonFlag = 0;
        }
    }

    if (digitalRead(ALARM) == 0) {
      AlarmButtonFlag = 0;
    }


    //Panic
    if (digitalRead(PANIC) == 1)        {
        if (PanicButtonFlag == 0) {
            PanicButtonFlag = 1;
            PanicStartmillis = millis();
            Serial.println("Start" + String(PanicStartmillis)); 
        }
        
        // Serial.println(String(currentMillis - AlarmStartmillis));
        currentPanicMillis = millis();
        if ((currentPanicMillis - PanicStartmillis >= TRIGTIME) and (PanicButtonFlag == 1)) {
            Serial.println("Stop" + String(currentPanicMillis));
            Serial.println("Panic Triggered");
            PanicState = 1;
            PanicButtonFlag = 0;
        }
    }

    if (digitalRead(PANIC) == 0) {
      PanicButtonFlag = 0;
    }
    
    


  if (SendRequest == 1) {
    ACState = digitalRead(ACFAIL);
    TamperState = digitalRead(TAMPER);
    // AlarmState = digitalRead(ALARM);
    // PanicState = digitalRead(PANIC);
    stateMessage = "W" + String(ACState) + String(TamperState) + String(AlarmState) + String(PanicState);
    LoRaSend(stateMessage);
    SendRequest = 0;
    AlarmState = 0;
    PanicState = 0;
  }
//   Serial.println(String(SirenState));
  if (SirenState == 1) {
      digitalWrite(SIRENALARM, HIGH);
  }
  if (SirenState == 0) {
      digitalWrite(SIRENALARM, LOW);
  }
  
  
  
  
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
  // Serial.print("Node Recieve: ");
  // Serial.println(message);

  if (message.charAt(0) == 'W' ){
    SendRequest = 1;
    if (message.charAt(1) == '1') {
        SirenState = 1;
    }else if (message.charAt(1) == '0')
    {
        SirenState = 0;
    }
    
  }
  
}

