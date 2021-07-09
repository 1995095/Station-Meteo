#include <Arduino.h>
//#include <WiFi.h>
//#include <HTTPClient.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h>
#include "AffichageBME.h"
#include "LCD1602.h"
#include <ArduinoJson.h>


WiFiManager wm;
bool resultat;
#define TESTADRESSES
#define TESTBME280
void testerAdresses();
Adafruit_BME280 bme;
AffichageBME BME(bme);
unsigned long delayTime = 5000;
float humidite;
float temperature;
float pression;
float altitude;
#define mqtt_server "192.168.23.196" 
#define mqtt_server "192.168.1.133" 
#define mqtt_port "1883"
#define mqtt_user "station_meteo_01"
#define mqtt_pass "qwerty123."
WiFiClient espClient;
PubSubClient client(espClient);
#define mqttHumidite "/stationMeteo/humidite"
#define mqttTemperature "/stationMeteo/temperature"
#define mqttPression "/stationMeteo/pression"
#define mqttAltitude "/stationMeteo/altitude"
LiquidCrystal_I2C lcd(0x27, 16, 2);
LCD1602 LCD(lcd);
bool shouldSaveConfig = false;
void saveConfigCallback () {  Serial.println("Should save config");  shouldSaveConfig = true; }
void saveParamsCallback ();
char const* SSIDPortail = "ConfigurationESP32";
char const* motPasseAPPortail = "Bonjour123";
IPAddress adresseIPPortail(192, 168, 23, 1);
IPAddress passerellePortail(192, 168, 23, 1);
IPAddress masqueReseauPortail(255, 255, 255, 0);
WiFiManagerParameter custom_mqtt_server("server","mqtt server","mqtt_server", 40);
WiFiManagerParameter custom_mqtt_port("port", "mqtt port", "mqtt_port", 6);
WiFiManagerParameter custom_mqtt_user("user", "mqtt user", "mqtt_user", 20);
WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", "mqtt_pass", 20);
void DeserialiserJSON(const char* fichierConfiguration ,const char*p_mqttserver ,const char* p_port ,const char* p_user ,const char* p_password);
void SerialiserJSON(const char*p_mqttserver ,const char* p_port ,const char* p_user ,const char* p_password);
void GestionDuTemps(unsigned long tempsAccorder );

void setup()
{
lcd.init();
lcd.backlight();
LCD.definirPositionCurseur(0, 0);
LCD.afficher("Station Meteo");
Serial.begin(115200);
wm.setDebugOutput(true); 
wm.setAPCallback([](WiFiManager* p_wiFiManager) {
    Serial.println("Connexion au réseau WiFi échouée, on lance le portail !"); });
wm.setConfigPortalTimeout(180);
wm.setSaveParamsCallback([&custom_mqtt_server,&custom_mqtt_port,&custom_mqtt_user,&custom_mqtt_pass]() {
    strcpy(mqtt_server, custom_mqtt_server.getValue()); 
    strcpy(mqtt_port, custom_mqtt_port.getValue()); 
    strcpy(mqtt_user, custom_mqtt_user.getValue()); 
    strcpy(mqtt_pass, custom_mqtt_pass.getValue());   
       
  });
wm.setSaveConfigCallback(saveConfigCallback);
wm.addParameter(&custom_mqtt_server);
wm.addParameter(&custom_mqtt_port);
wm.addParameter(&custom_mqtt_user);
wm.addParameter(&custom_mqtt_pass);
wm.setAPStaticIPConfig(adresseIPPortail, passerellePortail,masqueReseauPortail);
wm.setParamsPage(true);  
//wm.erase();
 wm.autoConnect(SSIDPortail, motPasseAPPortail);
  if (WiFi.isConnected()) {   
    Serial.println("Connecté au réseau : " + WiFi.SSID() + " avec l'adresse : " + WiFi.localIP().toString());
    LCD.definirPositionCurseur(0, 1);
    LCD.afficher("Connexion reussie");  
if (shouldSaveConfig) {
  SerialiserJSON(mqtt_server ,mqtt_port ,mqtt_user ,mqtt_pass);
    }
  }
unsigned status = BME.m_bme.begin(0x76);
 if (!status)
  {
    Serial.println(
        "Could not find a valid BME280 sensor, check wiring, address, sensor "
        "ID!");
  }
  Serial.println();
  client.setServer(mqtt_server, 1883);
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("arduinoClient", "station_meteo_01", "qwerty123."))
    {
      Serial.println("connected");
      client.publish("outTopic", "hello world");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      GestionDuTemps(5000);
    }
  } 
}

void loop()
{

#ifdef TESTADRESSES
  testerAdresses();
#endif

#ifdef TESTBME280

  temperature = BME.lireTemperature();
  humidite = BME.lireHumidite();
  pression = BME.lirePression();
  altitude = BME.lireAltitude();
LCD.effacer();
LCD.afficher("Temperature :");
LCD.definirPositionCurseur(0, 1);
LCD.afficher(String(temperature).c_str());
GestionDuTemps(delayTime);
LCD.effacer();
LCD.afficher("Humidite :");
LCD.definirPositionCurseur(0, 1);
LCD.afficher(String(humidite).c_str());
#endif
  GestionDuTemps(delayTime);
  client.publish(mqttHumidite, String(humidite).c_str(), true);
  client.publish(mqttTemperature, String(temperature).c_str(), true);
  client.publish(mqttPression, String(pression).c_str(), true);
  client.publish(mqttAltitude, String(altitude).c_str(), true);
  GestionDuTemps(1000);
} 

void testerAdresses()
{
  byte error, address; 
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
  {
    Serial.println("No I2C devices found\n");
  }
  else
  {
    Serial.println("done\n");
  }
}

void DeserialiserJSON(const char* fichierConfiguration ,const char*p_mqttserver ,const char* p_port ,const char* p_user ,const char* p_password) { 
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc,fichierConfiguration);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  JsonObject configuration = doc["configuration"];  
  p_mqttserver =configuration["mqtt_server"] ;
  p_port =configuration["mqtt_port"];
  p_user= configuration["mqtt_user"] ;
  p_password=  configuration["mqtt_pass"] ;

}  
void SerialiserJSON(const char*p_mqttserver ,const char* p_port ,const char* p_user ,const char* p_password){
DynamicJsonDocument doc(1024);
    JsonObject configuration = doc.createNestedObject("configuration");    
    configuration["mqtt_server"] = mqtt_server;
    configuration["mqtt_port"] = mqtt_port;
    configuration["mqtt_user"] = mqtt_user;
    configuration["mqtt_pass"] = mqtt_pass;
    char fichierDeConfiguration[1024];
   serializeJson(doc, fichierDeConfiguration);
   Serial.println(fichierDeConfiguration);
}
void GestionDuTemps(unsigned long tempsAccorder ) {
   unsigned long tempsActuel=millis();
   unsigned long compteurDuTemps=0;
   while (compteurDuTemps<tempsAccorder){
   compteurDuTemps=millis()-tempsActuel;
  }
}