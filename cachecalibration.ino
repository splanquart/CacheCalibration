#include "RelayController.h"
#include "AlpacaDriver.h"
#include "HttpHandler.h"
#include "AlpacaSwitchDevice.h"
#include "RelayAdapter.h"
#include "AlpacaCoverCalibratorDevice.h"

#include <WiFiManager.h>
#include <DNSServer.h>

#define ALPACA_PORT 11111


const int maxSwitch = 4;
RelayController *relay;
RelayAdapter* relayAdapter;
AlpacaDriver *driver;
ESP8266WebServer server(80);

HttpHandler httpHandler(ALPACA_PORT);
AlpacaSwitchDevice *alpacaSwitch;
AlpacaCoverCalibratorDevice *alpacaCover;


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
  // Si la connexion échoue, le point d'accès "Cache_Calibration" sera créé
  // Vous pouvez utiliser votre téléphone pour vous connecter à ce point d'accès et configurer le SSID et le mot de passe
  if (!wifiManager.autoConnect("Cache_Calibration")) {
    Serial.println("Échec de la connexion, redémarrage...");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  
  // Si vous arrivez ici, vous êtes connecté au WiFi choisi
  Serial.println("Connecté !");

  server.begin();
  
  relay = new RelayController(maxSwitch);
  driver = new AlpacaDriver(httpHandler);
  alpacaSwitch = new AlpacaSwitchDevice(httpHandler, 0, new RelayAdapter(relay, {0,1,2,3}));
  alpacaCover = new AlpacaCoverCalibratorDevice(httpHandler, 0, new RelayAdapter(relay, {3}), new RelayAdapter(relay, {2}));
  driver->addDevice(alpacaSwitch);
  driver->addDevice(alpacaCover);


  server.on("/reset", handleReset);
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Location", driver->getSetupUrl(), true);
    server.send(302, "text/plain");
  });

  driver->begin();
  alpacaSwitch->begin();
  alpacaCover->begin();

}

void loop() {
  server.handleClient(); // Handle client requests
  driver->handleDiscovery();
  driver->handleClient();
}
