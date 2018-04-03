/**
   simple MH-Z14A CO2 level monitor
   ESP32 for NodeNCU or DevkitC
   View serial plotter to moniter CO2(ppm) time chart
   Author: coniferconifer
   License: Apache License v2
   April 3,2018

*/
// References
// http://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z14a_co2-manual-v1_01.pdf
// http://www.winsen-sensor.com/d/files/PDF/Infrared%20Gas%20Sensor/NDIR%20CO2%20SENSOR/MH-Z14%20CO2%20V2.4.pdf

HardwareSerial Serial2(2); //RX as GPIO 16 TX as GPIO 17
#define LED 2 // on board LED for NodeNCU ESP32 board

void setup() {
  pinMode(LED, OUTPUT);   
  digitalWrite(LED, LOW);
  Serial.begin(115200);
  Serial2.begin(9600); // communication for MH-Z14A
  initCO2(); // read out garbage (i have no idea why but this is a must have for mobile battery operation)
#ifdef PREHEATING
  int i;
  for (i = 0; i < 180; i++) {
    Serial.print("."); delay(1000);
  }
#endif
}

void loop() {
  Serial.println(getCO2());
  delay(3000);
  digitalWrite(LED, HIGH);
  delay(2000);
  digitalWrite(LED, LOW);
}

uint8_t command[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
// uint8_t reset[] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

void initCO2() {
  uint8_t c;
  while (Serial2.available()) {
    c = Serial2.read();
  }
}
int getCO2() {
  int returnnum = 0;
  uint16_t co2 = 0;
  uint8_t readBuffer[9] = {};
  int i; uint8_t checksum = 0;
  initCO2();

  Serial2.write(command, sizeof(command));
  Serial2.setTimeout(300);//set 300msec timeout
  returnnum = Serial2.readBytes(readBuffer, sizeof(readBuffer));

  if ( (readBuffer[0] == 0xFF) && (readBuffer[1] == 0x86)) {
    for ( i = 1; i < 8; i++)
    {
      checksum += readBuffer[i];
    }
    checksum = 0xff - checksum;
    checksum += 1;
#ifdef DEBUG
    Serial.println(checksum, HEX);
    for (i = 0; i < sizeof(readBuffer); i++) {
      Serial.print(readBuffer[i], HEX);
      Serial.print(" ");
    }
#endif
    if (readBuffer[8] != checksum) {
      Serial.println("check sum error");
      return (6000); // 6000 indicates check sum error
    }
    co2 = (uint16_t)readBuffer[2] << 8;
    co2 += readBuffer[3];
//    Serial.print("CO2 level(ppm):" ); 
    Serial.println(co2);
    return (co2);
  }
  // in case of anormally , indicate by flashing LED
  digitalWrite(LED, HIGH); delay(50); digitalWrite(LED, LOW); delay(500);
  digitalWrite(LED, HIGH); delay(50); digitalWrite(LED, LOW); delay(500);

  return (returnnum);
}

