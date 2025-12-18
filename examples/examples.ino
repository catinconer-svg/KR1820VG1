/*
  Пример часов с использованием библиотеки KR1820VG1
  Вывод времени в формате HH-MM-SS
*/

#include <KR1820VG1.h>

// Инициализация дисплея (DATA, CLK, CS1, CS2)
KR1820VG1 lcd(5, 6, 7, 8);

unsigned long lastUpdate = 0;
uint8_t hours = 12;
uint8_t minutes = 34;
uint8_t seconds = 56;

void setup() {
  lcd.begin();
  lcd.display();
  lcd.clear();
}

void loop() {
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours = (hours + 1) % 24;
      }
    }
    
    // Способ 1: используем специальный метод
    lcd.clear();
    lcd.printTime(hours, minutes, seconds);
    
    // Способ 2: выводим как обычные символы
    // lcd.clear();
    // lcd.print(hours / 10);
    // lcd.print(hours % 10);
    // lcd.print('-');
    // lcd.print(minutes / 10);
    // lcd.print(minutes % 10);
    // lcd.print('-');
    // lcd.print(seconds / 10);
    // lcd.print(seconds % 10);
  }
}
