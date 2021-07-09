#pragma once
#include <Arduino.h>

#include <LiquidCrystal_I2C.h>

class LCD1602 {
 public:
  LCD1602 (LiquidCrystal_I2C &p_lcd);
  void definirPositionCurseur(int p_colonne, int p_ligne) const;
  void afficher(const String& p_texte) const;
  void effacer() const;

 private:
  LiquidCrystal_I2C &m_lcd;
};
