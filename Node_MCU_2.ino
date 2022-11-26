
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "Drone.h"
Drone drone;

uint8_t broadcastAddress1[] = {0x50, 0x02, 0x91, 0xDF, 0x4C, 0x5A};
uint8_t broadcastAddress2[] = {0x50, 0x02, 0x91, 0xDE, 0xE2, 0xD1};

// Structure to send data
// Must match the receiver structure
typedef struct struct_message_n2 {
  //char a[32];
  int battery_n2;
  int temperature_n2;
  int speed_n2;
  int height_n2;
  int barometer_n2;
  //float c;
  String d;
  //bool e;
} struct_message_n2;

// Create a struct_message called myData
struct_message_n2 myData_n2;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Node MCU 2 - Delivery success");
  }
  else{
    Serial.println("Node MCU 2 - Delivery fail");
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA); 

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress1, ESP_NOW_ROLE_SLAVE, 5, NULL, 0);
  esp_now_add_peer(broadcastAddress2, ESP_NOW_ROLE_SLAVE, 8, NULL, 0);

  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  // Set values to send
  //strcpy(myData_n2.a, "THIS IS A CHAR");
  //drone.init();
  //myData_n2.d = "Drone 2";
  //myData_n2.battery_n2 = drone.getBattery();
  //myData_n2.c = 1.2;

  //myData_n2.e = false;
 
}
 
void loop() {
    drone.init();
    myData_n2.d = "Drone 2";
    myData_n2.battery_n2 = drone.getBattery();
    myData_n2.temperature_n2 = drone.getTemperature();
    myData_n2.speed_n2 = drone.getSpeed();
    myData_n2.height_n2 = drone.getHeight();
    myData_n2.barometer_n2 = drone.getBarometer();
    drone.takeoff();
    delay(2000);
    esp_now_send(broadcastAddress1, (uint8_t *) &myData_n2, sizeof(myData_n2));
    drone.up(30);
    delay(2000);
    esp_now_send(broadcastAddress2, (uint8_t *) &myData_n2, sizeof(myData_n2));
    drone.rot_cw(360);
    delay(2000);
    esp_now_send(broadcastAddress1, (uint8_t *) &myData_n2, sizeof(myData_n2));
    drone.down(30);
    delay(2000);
    esp_now_send(broadcastAddress2, (uint8_t *) &myData_n2, sizeof(myData_n2));
    drone.land();
  
}

#include <ESP8266WiFi.h>
#include <espnow.h>

// Structure to receive data
// Must match the sender structure
typedef struct struct_message_n1 {
    //char a[32];
    int battery_n1;
    int temperature_n1;
    int speed_n1;
    int height_n1;
    int barometer_n2;
    //float c;
    String d;
    //bool e;
} struct_message_n1;

// Create a struct_message called myData
struct_message_n1 myData_n1;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData_n1, incomingData, sizeof(myData_n1));
  Serial.print("Bytes received: ");
  Serial.println(len);
  //Serial.print("Char: ");
  //Serial.println(myData_n1.a);
  Serial.println(myData_n1.d);
  Serial.print("Drone 1 Battery Value: ");
  Serial.println(myData_n1.battery_n1);
  Serial.print("Drone2 Temperature Value: ");
  Serial.println(myData_n1.temperature_n1);
  Serial.print("Drone2 Speed Value: ");
  Serial.println(myData_n1.speed_n1);
  
  //Serial.print("Float: ");
  //Serial.println(myData_n1.c);
  //Serial.print("Bool: ");
  //Serial.println(myData_n1.e);
  //Serial.println();
}
