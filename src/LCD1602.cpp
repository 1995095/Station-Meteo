#include "LCD1602.h"

#include <LiquidCrystal_I2C.h>

LCD1602::LCD1602(LiquidCrystal_I2C &p_lcd) : m_lcd(p_lcd)
{

   // On pourrait passer l'adresse par paramètres
   this->m_lcd.init();
   this->m_lcd.backlight();
}

void LCD1602::definirPositionCurseur(int p_colonne, int p_ligne) const
{
   this->m_lcd.setCursor(p_colonne, p_ligne);
}

void LCD1602::afficher(const String &p_texte) const
{
   this->m_lcd.print(p_texte);
}

void LCD1602::effacer() const { this->m_lcd.clear(); }
