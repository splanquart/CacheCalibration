#include "RelayController.h"

#include <WiFiManager.h>
#include <DNSServer.h>

const int maxSwitch = 4;
RelayController *relay = new RelayController(maxSwitch);
AlpacaDriver driver(relay, 0);
void setup() {
  
  Serial.begin(115200);
  delay(10);


  WiFiManager wifiManager;
  // Démarrer le WiFiManager
  // Si la connexion échoue, le point d'accès "AutoConnectAP" sera créé
  // Vous pouvez utiliser votre téléphone pour vous connecter à ce point d'accès et configurer le SSID et le mot de passe
  if (!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("Échec de la connexion, redémarrage...");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  
  // Si vous arrivez ici, vous êtes connecté au WiFi choisi
  Serial.println("Connecté !");
  driver.begin();
}

void loop() {
  driver.handleDiscovery();
  driver.handleClient();

}
