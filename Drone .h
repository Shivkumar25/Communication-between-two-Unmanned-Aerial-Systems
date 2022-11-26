#ifndef _DRONE_H_
#define _DRONE_H_

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* DEFAULT_SSID = "TELLO-612781";
const char* DEFAULT_PW = "";
const char* TELLO_IP = "192.168.10.1";
const char* const TELLO_STREAM_URL{"udp://0.0.0.0:11111"};
const int PORT = 8889;

const int INIT = 0;
const int CONNECTED = 1;

class Drone {
  public:
    Drone();
    void init();

    bool takeoff();
    bool land();
    bool getVideo();
    bool goVideo();
    bool up(int x);
    bool down(int x);
    bool left(int x);
    bool right(int x);
    bool forward(int x);
    bool back(int x);
    bool rot_cw(int deg);
    bool rot_ccw(int deg);
  

    bool setSpeed(int x);

    int getBattery();
    int getTemperature();
    int getSpeed();
    int getHeight();
    int getBarometer();

    int battery;
    int temperature;
    int speed;
    int height;
    int barometer;
  

    
  private:
    bool _drone_init;
    WiFiUDP* udp;

    String _sendCmd(String cmd);

    void _wifiEvent(WiFiEvent_t event);
    static Drone * _instance;
    static void isr(WiFiEvent_t event);

    bool _cmd(String cmd, char value);
    bool _cmd(String cmd);
    bool _cmd(String cmd, int value);
    int _get(String cmd);

    
};

Drone::Drone()
{
  pinMode(LED_BUILTIN, OUTPUT);
  _instance = this;
  _drone_init = false;
  udp = new WiFiUDP();
  battery = 0;
}

void Drone::init()
{
  DEBUG_PRINTLN("\n\nConnecting to WiFi network: " + String(DEFAULT_SSID));

  WiFi.disconnect(true);
  WiFi.onEvent(isr);
  WiFi.begin(DEFAULT_SSID, DEFAULT_PW);
  DEBUG_PRINTLN("Waiting for WiFi connection...");
  while (WiFi.status() != WL_CONNECTED) {
    DEBUG_PRINT(".");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }
  while (!_drone_init);

  udp->begin(PORT);
  udp->flush();

  DEBUG_PRINTLN("Waiting for Drone connection...");

  while (!_sendCmd("command").equals("ok"))
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  DEBUG_PRINTLN("Drone connected!");
  battery = getBattery();
}


void Drone::isr(WiFiEvent_t event)
{
  _instance->_wifiEvent(event);
}

Drone * Drone::_instance;

void Drone::_wifiEvent(WiFiEvent_t event)
{
  switch (event)
  {
    case WL_CONNECTED:
      DEBUG_PRINT("\n\nWiFi connected! IP address: ");
      DEBUG_PRINTLN(WiFi.localIP());
      _drone_init = true;
      break;
    case WL_NO_SSID_AVAIL:
      if (_drone_init)
      {
        DEBUG_PRINTLN("Please reset MCU.");
      }
      else
      {
        DEBUG_PRINTLN("Please open drone.");
      }
      break;
  }
}


String Drone::_sendCmd(String cmd)
{
  udp->beginPacket(TELLO_IP, PORT);
  udp->write(cmd.c_str());
  udp->endPacket();

  char buffer[20] = {0};
  while (!udp->parsePacket());
  int n = udp->read(buffer, udp->available());
  if (n > 0)
  {
    buffer[n] = '\0';
  }
  return String(buffer);
}

bool Drone::_cmd(String cmd)
{
  String response = _sendCmd(cmd);
  DEBUG_PRINT("send: ");
  DEBUG_PRINT(cmd);
  DEBUG_PRINT(" response: ");
  DEBUG_PRINTLN(response);
  if (!response.equals("ok"))
  {
    return true;
  }

  return false;
}

bool Drone::_cmd(String cmd, int value)
{
  char buff[10];
  String command = cmd + " " + String(itoa(value, buff, 10));
  String response = _sendCmd(command);
  DEBUG_PRINT("send: ");
  DEBUG_PRINT(command);
  DEBUG_PRINT(" response: ");
  DEBUG_PRINTLN(response);
  if (!response.equals("ok"))
  {
    return true;
  }
  return false;
}

int Drone::_get(String cmd)
{
  int response = _sendCmd(cmd).toInt();
//  DEBUG_PRINT("send: ");
//  DEBUG_PRINT(cmd);
//  DEBUG_PRINT(" response: ");
//  DEBUG_PRINTLN(response);
  return response;
}

bool Drone::takeoff()
{
  return _cmd("takeoff");
}

bool Drone::land()
{
  return _cmd("land");
}

bool Drone::up(int x)
{
  return _cmd("up", x);
}

bool Drone::down(int x)
{
  return _cmd("down", x);
}

bool Drone::left(int x)
{
  return _cmd("left", x);
}

bool Drone::right(int x)
{
  return _cmd("right", x);
}

bool Drone::forward(int x)
{
  return _cmd("forward", x);
}
bool Drone::back(int x) {
  return _cmd("back", x);
}
bool Drone::rot_cw(int deg) {
  return _cmd("cw", deg);
}
bool Drone::rot_ccw(int deg) {
  return _cmd("ccw", deg);
}
bool Drone::setSpeed(int x) {
  x = constrain(x, 10, 100);
  return _cmd("speed", x);
}

int Drone::getBattery()
{
  battery = _get("battery?");
  return battery;
}
int Drone::getTemperature()
{
  temperature = _get("temp?");
  return temperature;
}
bool Drone::getVideo()
{
  return _cmd("streamon");
}
bool Drone::goVideo()
{
  return _cmd("streamoff");
}

int Drone::getSpeed()
{
  speed = _get("speed?");
  return speed;
}

int Drone::getHeight()
{
  height = _get("height?");
  return height;
}

int Drone::getBarometer()
{
  barometer = _get("baro?");
  return barometer;
}

#endif
