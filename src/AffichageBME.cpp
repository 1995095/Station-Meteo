#include "AffichageBME.h"
#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

AffichageBME::AffichageBME(Adafruit_BME280 &p_bme)
    : m_bme(p_bme)
{
   ;
}

float AffichageBME::lireTemperature()
{
   return this->m_bme.readTemperature();
}

float AffichageBME::lireHumidite()
{
   return this->m_bme.readHumidity();
}

float AffichageBME::lirePression()
{
   return this->m_bme.readPressure() / 100.0F;
}

float AffichageBME::lireAltitude()
{
   float SEALEVELPRESSURE_HPA = 1013.25;

   return this->m_bme.readAltitude(SEALEVELPRESSURE_HPA);
}
