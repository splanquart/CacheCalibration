#include "HttpHandler.h"
#define ALPACA_PORT 11111

HttpHandler::HttpHandler(uint16_t port)
: _server(ALPACA_PORT) {
}

void HttpHandler::begin() {
  _server.onNotFound(std::bind(&HttpHandler::handlerNotFound, this)); 
  _server.begin();
}

void HttpHandler::handleClient() {
  _server.handleClient();
}

void HttpHandler::bind(const String& uri, HTTPMethod method, std::function<void()> handler) {
  _server.on(uri.c_str(), method, handler);
}


void HttpHandler::handlerNotFound(){
  Serial.println("----------");
  Serial.print("URL not found: ");
  Serial.println(_server.uri());
  Serial.println("----------");
  _server.send(404, "text/plain", "Not found");
}

String HttpHandler::getHTTPMethodName() {
  switch (_server.method()) {
    case HTTP_GET:     return "GET";
    case HTTP_POST:    return "POST";
    case HTTP_DELETE:  return "DELETE";
    case HTTP_PUT:     return "PUT";
    case HTTP_PATCH:   return "PATCH";
    case HTTP_HEAD:    return "HEAD";
    case HTTP_OPTIONS: return "OPTIONS";
    default:           return "UNKNOWN";
  }
}
void HttpHandler::logRequest(const String& callerName) {
  Serial.print(getHTTPMethodName());
  Serial.print(" ");
  Serial.print(_server.uri());
  if(callerName!=""){
    Serial.print(" -> ");
    Serial.print(callerName);
  }
  Serial.println();
}
void HttpHandler::sendHtml(int httpAnwserCode, String content) {
  _server.send(httpAnwserCode, _content_type_html, content);
}
void HttpHandler::sendContent(String content) {
  _server.sendContent(content);
}
HTTPMethod HttpHandler::method() {
  return _server.method();
}
int HttpHandler::args() {
  return _server.args();
}
String HttpHandler::argName(int i) {
  return _server.argName(i);
}
String HttpHandler::arg(String i) {
  return _server.arg(i);
}
String HttpHandler::arg(int i) {
  return _server.arg(i);
}

void HttpHandler::returnResponse(DynamicJsonDocument val) {
  int clientID = (uint32_t)_server.arg("ClientID").toInt();
  int transID = (uint32_t)_server.arg("ClientTransactionID").toInt();
  for(int i=0;i<_server.args();i++) {
    if (_server.argName(i) == "clientId") {
      clientID = (uint32_t)_server.arg(i).toInt();  
    } else if (_server.argName(i) == "ClientTransactionID") {
      transID = (uint32_t)_server.arg(i).toInt();
    }
  }
  
  DynamicJsonDocument doc(1024);
  doc["Value"] = val;
  doc["ErrorMessage"] = "";
  doc["ErrorNumber"] = 0;
  doc["ClientTransactionID"] = transID;
  doc["ServerTransactionID"] = ++_serverTransactionID;

  String response;
  serializeJson(doc, response);
  _server.send(200, _content_type, response); // envoie la réponse JSON au client Alpaca
  Serial.print("    >>>> ");
  Serial.println(response);
}
void HttpHandler::returnBoolValue(bool val, String errMsg, int errNr) {
  int clientID = (uint32_t)_server.arg("ClientID").toInt();
  int transID = (uint32_t)_server.arg("ClientTransactionID").toInt();
  for(int i=0;i<_server.args();i++) {
    if (_server.argName(i) == "clientId") {
      clientID = (uint32_t)_server.arg(i).toInt();  
    } else if (_server.argName(i) == "ClientTransactionID") {
      transID = (uint32_t)_server.arg(i).toInt();
    }
  }
  
  DynamicJsonDocument doc(1024);
  
  doc["Value"] = val;
  doc["ErrorMessage"] = errMsg;
  doc["ErrorNumber"] = errNr;
  doc["ClientTransactionID"] = transID;
  doc["ServerTransactionID"] = ++_serverTransactionID;
  
  String response;
  serializeJson(doc, response);
  
  _server.send(200, _content_type, response);
  Serial.print("    >>>> ");
  Serial.println(response);
}
void HttpHandler::returnNothing(String errMsg, int errNr) {
  int clientID = (uint32_t)_server.arg("ClientID").toInt();
  int transID = (uint32_t)_server.arg("ClientTransactionID").toInt();
  for(int i=0;i<_server.args();i++) {
    if (_server.argName(i) == "clientId") {
      clientID = (uint32_t)_server.arg(i).toInt();  
    } else if (_server.argName(i) == "ClientTransactionID") {
      transID = (uint32_t)_server.arg(i).toInt();
    }
  }
  
  DynamicJsonDocument doc(1024);
  
  doc["ErrorMessage"] = errMsg;
  doc["ErrorNumber"] = errNr;
  doc["ClientTransactionID"] = transID;
  doc["ServerTransactionID"] = ++_serverTransactionID;
  
  String response;
  serializeJson(doc, response);
  
  _server.send(200, _content_type, response);
  Serial.print("    >>>> ");
  Serial.println(response);
}
void HttpHandler::returnStringValue(String val, String errMsg, int errNr) {
  int clientID = (uint32_t)_server.arg("ClientID").toInt();
  int transID = (uint32_t)_server.arg("ClientTransactionID").toInt();
  for(int i=0;i<_server.args();i++) {
    if (_server.argName(i) == "clientId") {
      clientID = (uint32_t)_server.arg(i).toInt();  
    } else if (_server.argName(i) == "ClientTransactionID") {
      transID = (uint32_t)_server.arg(i).toInt();
    }
  }
  
  DynamicJsonDocument doc(1024);
  
  doc["Value"] = val;
  doc["ErrorMessage"] = errMsg;
  doc["ErrorNumber"] = errNr;
  doc["ClientTransactionID"] = transID;
  doc["ServerTransactionID"] = ++_serverTransactionID;
  
  String response;
  serializeJson(doc, response);
  
  _server.send(200, _content_type, response);
  Serial.print("    >>>> ");
  Serial.println(response);
}
void HttpHandler::returnIntValue(int val, String errMsg, int errNr) {
  int clientID = (uint32_t)_server.arg("ClientID").toInt();
  int transID = (uint32_t)_server.arg("ClientTransactionID").toInt();
  for(int i=0;i<_server.args();i++) {
    if (_server.argName(i) == "clientId") {
      clientID = (uint32_t)_server.arg(i).toInt();  
    } else if (_server.argName(i) == "ClientTransactionID") {
      transID = (uint32_t)_server.arg(i).toInt();
    }
  }

  
  DynamicJsonDocument doc(1024);
  
  doc["Value"] = val;
  doc["ErrorMessage"] = errMsg;
  doc["ErrorNumber"] = errNr;
  doc["ClientTransactionID"] = transID;
  doc["ServerTransactionID"] = ++_serverTransactionID;
  
  String response;
  serializeJson(doc, response);
  
  _server.send(200, _content_type, response);
  Serial.print("    >>>> ");
  Serial.println(response);
}

String HttpHandler::getArgCaseInsensitive(const String& argName) {
    for (uint8_t i = 0; i < _server.args(); i++) {
        if (_server.argName(i).equalsIgnoreCase(argName)) {
            return _server.arg(i);
        }
    }
    return String();  // Retourner une chaîne vide si aucun argument correspondant n'est trouvé
}
