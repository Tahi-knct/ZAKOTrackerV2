#include "config.h"
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <espnow.h>
#include <utility/imumaths.h>

#if Mode == Mode_Sender
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x70);

const long refreshRate = 60;
unsigned long lastTime = 0;
unsigned long timerDelay = 1000 / refreshRate;

uint8_t QuatDataArray[4];

// You need fill out broadcastAddress!!!
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void GetSensorQuaternion() {
    imu::Quaternion quat = bno.getQuat();
    QuatDataArray[0] = quat.x();
    QuatDataArray[1] = quat.y();
    QuatDataArray[2] = quat.z();
    QuatDataArray[3] = quat.w();
}

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    Serial.print("Last Packet Status: ");
    if (sendStatus == 0) {
        Serial.println("Success");
    } else {
        Serial.println("Fail");
    }
}

#elif Mode == Mode_Reciver
void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {
    Serial.print("s\t");
    for (int i = 0; i < len; i++) {
        Serial.print(incomingData[i]);
        Serial.print("\t");
    }
    Serial.print("\n");
}

#else
    #error you need select Mode
#endif

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != 0) {
        Serial.println("Error initialize ESP-NOW");
        return;
    }

#if Mode == Mode_Sender
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(OnDataSent);
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    if (!bno.begin(bno.OPERATION_MODE_IMUPLUS)) {
        Serial.print(" not detected\n");
        while (1)
            ;
    }

#elif Mode == Mode_Reciver
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(OnDataRecv);

#else
    #error you need select Mode
#endif
}

void loop() {
#if Mode == Mode_Sender
    GetSensorQuaternion();
    if ((millis() - lastTime) > timerDelay) {
        esp_now_send(broadcastAddress, (uint8_t *)&QuatDataArray, sizeof(QuatDataArray));
        lastTime = millis();
    }

#else
    #error you need select Mode
#endif
}