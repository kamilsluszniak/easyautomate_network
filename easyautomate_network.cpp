
#include "Arduino.h"
#include "easyautomate_network.h"

EasyautomateNetwork::EasyautomateNetwork(String device_name, String api_key, WiFiClientSecure client) {
  name = device_name;
  key = api_key;
  client = client;
}

void EasyautomateNetwork::setCACert() {
  // Synchronize time useing SNTP. This is necessary to verify that
  // the TLS certificates offered by the server are currently valid.
  Serial.print("Setting time using SNTP");
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 1000) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  // Load root certificate in DER format into WiFiClientSecure object
  bool res = client.setCACert(caCert, caCertLen);
  if (!res) {
    Serial.println("Failed to load root CA certificate!");
    while (true) {
      yield();
    }
  }
}

DynamicJsonDocument EasyautomateNetwork::getSettings() {
  DynamicJsonDocument jsonDocument(1024);
  boolean status_ok = false;

  if (!client.connect(host, httpsPort)) {
    jsonDocument["error"] = "connection failed";
    return jsonDocument;
  }

  // Verify validity of server's certificate
  if (!client.verifyCertChain(host)) {
    jsonDocument["error"] = "certificate verification failed";
    return jsonDocument;
  }

  String url = "/api/v1/devices/" + urlencode(name);
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + "?current_settings=true" + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Api-Key: " + key + "\r\n" +
               "Connection: close\r\n\r\n");


  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      client.stop();
      jsonDocument["error"] = "Client Timeout !";
      return jsonDocument;
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println(line);
    if (line.substring(9, 15) == "200 OK") {
      status_ok = true;
    }
    else if (line.substring(9, 25) == "401 Unauthorized") {
      status_ok = false;
      jsonDocument["error"] = "401 Unauthorized";
      return jsonDocument;
    }

    DeserializationError error = deserializeJson(jsonDocument, line);

    if (!error && line.toInt() == 0) {
      return jsonDocument;
    }
  }
  Serial.println("error");
  jsonDocument["error"] = "No JSON data";
  return jsonDocument;
}
