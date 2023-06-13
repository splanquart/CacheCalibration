#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoJson.h>
#include <functional>
#include <vector>

class HttpHandler {
public:
  HttpHandler(uint16_t port);

  void begin();
  void handleClient();
  void bind(const String& uri, HTTPMethod method, std::function<void()> handler);

  void returnResponse(DynamicJsonDocument val);
  void returnBoolValue(bool val, String errMsg, int errNr);
  void returnStringValue(String val, String errMsg, int errNr);
  void returnIntValue(int val, String errMsg, int errNr);
  void returnNothing(String errMsg, int errNr);
  String getArgCaseInsensitive(const String& argName);
  String getHTTPMethodName();
  void logRequest(const String& callerName);
  void sendHtml(int httpAnwserCode, String content);
  void sendContent(String content);
  HTTPMethod method();
  int args();
  String argName(int i);
  String arg(int i);
  String arg(String i);

protected:
  void handlerNotFound();
    
private:
  ESP8266WebServer _server;
  uint32_t _serverTransactionID = 0;
  const char* _content_type = "application/json";
  const char* _content_type_html = "text/html; charset=UTF-8";

};

#endif // HTTPHANDLER_H
 
