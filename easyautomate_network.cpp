#include "Arduino.h"
#include "easyautomate_network.h"

X509List cert(trustRoot);

EasyautomateNetwork::EasyautomateNetwork (String device_name, String api_key, WiFiClientSecure client) {
  name = device_name;
  key = api_key;
  client = client;
}

void EasyautomateNetwork::setCACert() {
  // Synchronize time useing SNTP. This is necessary to verify that
  // the TLS certificates offered by the server are currently valid.
  Serial.print("Setting time using SNTP, with google.com");
  configTime(8 * 3600, 0, "time.google.com", "pool.ntp.org", "time.nist.gov");
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

  client.setTrustAnchors(&cert);
}

DynamicJsonDocument EasyautomateNetwork::getSettings() {
  String url = "/api/v1/devices/" + urlencode(name);
  String payload = String("GET ") + url + "?current_settings=true" + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "User-Agent: ESP8266Device\r\n" +
                  "Api-Key: " + key + "\r\n" +
                  "Connection: close\r\n\r\n";
  return handleRequest(payload);
}

DynamicJsonDocument EasyautomateNetwork::sendReports(String reports) {
  String url = "/api/v1/measurements";
  String payload = String("POST ") + url + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "User-Agent: ESP8266Device\r\n" +
                  "Accept: application/json" + "\r\n" +
                  "Content-Type: application/json" + "\r\n" +
                  "Content-Length: " + String(reports.length()) + "\r\n" +
                  "Api-Key: " + key + "\r\n" +
                  "\r\n" + reports;
  return handleRequest(payload);
}

DynamicJsonDocument EasyautomateNetwork::handleRequest(String payload) {
  DynamicJsonDocument jsonDocument(1024);
  boolean status_ok = false;

  if (!client.connect(host, httpsPort)) {
    char buf[256];
    client.getLastSSLError(buf,256);
    Serial.print("WiFiClientSecure SSL error: ");
    Serial.println(buf);
    jsonDocument["error"] = "connection failed";
    return jsonDocument;
  }

  client.print(payload);

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








