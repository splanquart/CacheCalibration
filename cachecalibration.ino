#include "RelayController.h"

#include <WiFiManager.h>
#include <DNSServer.h>

const int maxSwitch = 4;
RelayController *relay;
AlpacaDriver *driver;
ESP8266WebServer server(80);

void handleReset() {
  WiFi.disconnect(true); // Reset WiFi credentials
  server.send(200, "text/plain", "WiFi reset done. The device will now reboot."); 
  delay(1000);
  ESP.restart(); // Reboot the device
}


void setup() {
  
  Serial.begin(115200);
  delay(10);
  WiFiManager wifiManager;
  // Démarrer le WiFiManager
  // Si la connexion échoue, le point d'accès "AutoConnectAP" sera créé
  // Vous pouvez utiliser votre téléphone pour vous connecter à ce point d'accès et configurer le SSID et le mot de passe
  if (!wifiManager.autoConnect("Cache_Calibration")) {
    Serial.println("Échec de la connexion, redémarrage...");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  
  // Si vous arrivez ici, vous êtes connecté au WiFi choisi
  Serial.println("Connecté !");

  server.on("/reset", handleReset); // Register reset handler
  server.begin();
  
  relay = new RelayController(maxSwitch);
  driver = new AlpacaDriver(relay, 0);

  driver->begin();
}

void loop() {
  server.handleClient(); // Handle client requests
  driver->handleDiscovery();
  driver->handleClient();
}
