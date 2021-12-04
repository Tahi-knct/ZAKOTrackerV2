#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <espnow.h>
#include <utility/imumaths.h>

void GetSensorQuaternion();

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x70);

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

unsigned long lastTime = 0;
unsigned long timerDelay = 2000;

float QuatDataArray[4];

void OnDataSent(uint8_t* mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Status: ");
  if (sendStatus == 0) {
    Serial.println("Success");
  } else {
    Serial.println("Fail");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initialize ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  if (!bno.begin(bno.OPERATION_MODE_IMUPLUS)) {
    Serial.print(" not detected\n");
    while (1)
      ;
  }
}

void loop() {
  GetSensorQuaternion();
  if ((millis() - lastTime) > timerDelay) {
    esp_now_send(broadcastAddress, (uint8_t*)&QuatDataArray, sizeof(QuatDataArray));
    lastTime = millis();
  }
}

void GetSensorQuaternion() {
  imu::Quaternion quat = bno.getQuat();
  QuatDataArray[0] = quat.x();
  QuatDataArray[1] = quat.y();
  QuatDataArray[2] = quat.z();
  QuatDataArray[3] = quat.w();
}