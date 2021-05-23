#ifndef EASYAUTOMATE_NETWORK_H
#define EASYAUTOMATE_NETWORK_H

#include "Arduino.h"
#include <urlencode.h>
#include <string.h>
#include <ArduinoJson.h>
#include "easyautomate_network.h"
#include <time.h>
#include "ca_cert.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

static const char* host = "www.easyautomate.me";
static const int httpsPort = 443;

class EasyautomateNetwork
{
private:
  String name;
  String key;
  WiFiClientSecure client;

public:
  EasyautomateNetwork(String device_name, String api_key, WiFiClientSecure client);
  void setCACert();
  DynamicJsonDocument getSettings();
  DynamicJsonDocument sendReports(String reports);
};

#endif
