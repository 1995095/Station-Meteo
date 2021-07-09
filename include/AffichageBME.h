#pragma once
#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>


class AffichageBME
{
public:
    AffichageBME(Adafruit_BME280 &p_bme);
     float lireTemperature()  ;
     float lireHumidite()  ;
     float lirePression() ;
     float lireAltitude() ;
     
public:
    Adafruit_BME280 &m_bme;
};