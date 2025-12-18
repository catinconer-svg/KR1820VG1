#include "KR1820VG1.h"
#include <ctype.h>

// Конструктор
KR1820VG1::KR1820VG1(uint8_t dataPin, uint8_t clkPin, uint8_t cs1Pin, uint8_t cs2Pin) {
  _dataPin = dataPin;
  _clkPin = clkPin;
  _cs1Pin = cs1Pin;
  _cs2Pin = cs2Pin;
  _cursorPos = 0;
  _displayControl = true;
  _blinkControl = false;
  _brightness = 15;
  
  for (uint8_t i = 0; i < 8; i++) {
    _displayBuffer[i] = ' ';
  }
}

// Функция преобразования символа в сегментный код
uint8_t KR1820VG1::_charToSegments(uint8_t ch) {
  // Вспомогательная функция для проверки символа
  auto checkChar = [](uint8_t c) -> uint8_t {
    switch (c) {
      // Цифры 0-9
      case '0': return 0b11000000;
      case '1': return 0b11111001;
      case '2': return 0b10100100;
      case '3': return 0b10110000;
      case '4': return 0b10011001;
      case '5': return 0b10010010;
      case '6': return 0b10000010;
      case '7': return 0b11111000;
      case '8': return 0b10000000;
      case '9': return 0b10010000;
      
      // Заглавные латинские буквы (также используются для строчных)
      case 'A': return 0b10001000;
      case 'B': return 0b10000011;
      case 'C': return 0b11000110;
      case 'E': return 0b10000110;
      case 'F': return 0b10001110;
      case 'G': return 0b11000010;
      case 'H': return 0b10001001;
      case 'I': return 0b11111001;
      case 'J': return 0b11100001;
      case 'L': return 0b11000111;
      case 'N': return 0b11001000;
      case 'O': return 0b11000000;
      case 'P': return 0b10001100;
      case 'S': return 0b10010010;
      case 'U': return 0b11000001;
      case 'Y': return 0b10010001;
      case 'Z': return 0b10100100;
      
      // Строчные латинские буквы, которые отличаются от заглавных
      case 'a': return 0b10001000;    // как 'A'
      case 'b': return 0b10000011;    // как 'B'
      case 'c': return 0b11000110;    // как 'C'
      case 'd': return 0b10100001;    // специальная форма
      case 'e': return 0b10000110;    // как 'E'
      case 'f': return 0b10001110;    // как 'F'
      case 'g': return 0b11000010;    // специальная форма
      case 'h': return 0b10001011;    // специальная форма
      case 'i': return 0b11111011;    // специальная форма (с точкой)
      case 'j': return 0b11100001;    // как 'J'
      case 'l': return 0b11000111;    // как 'L'
      case 'n': return 0b10101011;    // специальная форма
      case 'ñ': return 0b10101011;    // как 'n'
      case 'o': return 0b10100011;    // специальная форма
      case 'q': return 0b10011001;    // специальная форма
      case 'r': return 0b10101111;    // специальная форма
      case 't': return 0b10000111;    // специальная форма
      case 'u': return 0b11100011;    // специальная форма
      case 'y': return 0b10010001;    // как 'Y'
      case 'z': return 0b10100100;    // как 'Z'
      
      // Специальные символы
      case '-': return 0b10111111;
      case '_': return 0b11110111;
      case ' ': return 0b11111111;
      case '.': return 0b01111111;
      case ':': return 0b11101110;
      
      default: return 0xFF; // Не найдено
    }
  };
  
  // Сначала пробуем точное совпадение
  uint8_t result = checkChar(ch);
  if (result != 0xFF) return result;
  
  // Если не найдено, пробуем другой регистр
  uint8_t altChar = 0;
  
  if (isalpha(ch)) {
    if (isupper(ch)) {
      // Преобразуем заглавную в строчную
      altChar = tolower(ch);
    } else {
      // Преобразуем строчную в заглавную
      altChar = toupper(ch);
    }
    
    // Проверяем альтернативный символ
    result = checkChar(altChar);
    if (result != 0xFF) return result;
  }
  
  // Если ничего не найдено, возвращаем пустой символ
  return 0b11111111;
}

// Инициализация дисплея
void KR1820VG1::begin() {
  pinMode(_dataPin, OUTPUT);
  pinMode(_clkPin, OUTPUT);
  pinMode(_cs1Pin, OUTPUT);
  pinMode(_cs2Pin, OUTPUT);
  
  digitalWrite(_dataPin, HIGH);
  digitalWrite(_clkPin, HIGH);
  digitalWrite(_cs1Pin, HIGH);
  digitalWrite(_cs2Pin, HIGH);
  
  _initDisplay();
  delay(100);
  clear();
}

// Низкоуровневая инициализация
void KR1820VG1::_initDisplay() {
  digitalWrite(_cs2Pin, HIGH);
  digitalWrite(_cs1Pin, HIGH);
  _writeZeros(36);
  
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(_dataPin, LOW);
    _strob();
  }
  
  digitalWrite(_cs2Pin, LOW);
  digitalWrite(_cs1Pin, LOW);
  digitalWrite(_cs2Pin, HIGH);
  _writeZeros(40);
  digitalWrite(_cs2Pin, LOW);
}

// Запись нулей
void KR1820VG1::_writeZeros(uint8_t count) {
  for (uint8_t i = 0; i < count; i++) {
    digitalWrite(_dataPin, HIGH);
    _strob();
  }
}

// Строб-сигнал
void KR1820VG1::_strob() {
  digitalWrite(_clkPin, LOW);
  digitalWrite(_clkPin, HIGH);
}

// Запись битов
void KR1820VG1::_writeBits(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++, data <<= 1) {
    digitalWrite(_dataPin, (data & 0x80) ? HIGH : LOW);
    _strob();
  }
}

// Отправка данных на конкретный чип
void KR1820VG1::_sendToDisplay(uint8_t chip, uint8_t data[4]) {
  uint8_t csPin = (chip == 0) ? _cs1Pin : _cs2Pin;
  digitalWrite(csPin, HIGH);
  
  for (uint8_t i = 0; i < 4; i++) {
    _writeBits(data[i]);
  }
  
  // Управляющий байт
  _writeBits((chip == 0) ? 0b00001001 : 0b00001111);
  digitalWrite(csPin, LOW);
}

// Обновление дисплея из буфера
void KR1820VG1::_updateDisplay() {
  if (!_displayControl) return;
  
  uint8_t leftData[4], rightData[4];
  
  // Преобразуем буфер в коды сегментов
  for (uint8_t i = 0; i < 4; i++) {
    leftData[i] = _charToSegments(_displayBuffer[i]);
    rightData[i] = _charToSegments(_displayBuffer[i + 4]);
  }
  
  // Отправка на дисплей
  _sendToDisplay(0, leftData);   // CS1 - левая часть (разряды 5-8)
  _sendToDisplay(1, rightData);  // CS2 - правая часть (разряды 1-4)
}

// Очистка дисплея
void KR1820VG1::clear() {
  for (uint8_t i = 0; i < 8; i++) {
    _displayBuffer[i] = ' ';
  }
  _cursorPos = 0;
  _updateDisplay();
}

// Курсор в начало
void KR1820VG1::home() {
  _cursorPos = 0;
}

// Включение дисплея
void KR1820VG1::display() {
  _displayControl = true;
  _updateDisplay();
}

// Выключение дисплея
void KR1820VG1::noDisplay() {
  _displayControl = false;
}

// Включение мигания
void KR1820VG1::blink() {
  _blinkControl = true;
}

// Выключение мигания
void KR1820VG1::noBlink() {
  _blinkControl = false;
}

// Установка позиции курсора
void KR1820VG1::setCursor(uint8_t pos) {
  if (pos < 8) {
    _cursorPos = pos;
  }
}

// Получение позиции курсора
uint8_t KR1820VG1::getCursor() {
  return _cursorPos;
}

// Вывод символа
size_t KR1820VG1::write(uint8_t value) {
  if (_cursorPos >= 8) return 0;
  
  _displayBuffer[_cursorPos] = value;
  _cursorPos++;
  
  _updateDisplay();
  return 1;
}

// Вывод времени в формате HH-MM-SS
void KR1820VG1::printTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  char buffer[9];
  sprintf(buffer, "%02d-%02d-%02d", hours, minutes, seconds);
  
  for (uint8_t i = 0; i < 8 && buffer[i]; i++) {
    write(buffer[i]);
  }
}

// Вывод произвольных цифр
void KR1820VG1::printDigits(uint8_t digits[8]) {
  clear();
  setCursor(0);
  
  for (uint8_t i = 0; i < 8; i++) {
    if (digits[i] <= 9) {
      write('0' + digits[i]);
    } else if (digits[i] == 10) {
      write(' ');
    } else if (digits[i] == 11) {
      write('-');
    }
  }
}

// Установка яркости
void KR1820VG1::setBrightness(uint8_t level) {
  _brightness = (level > 15) ? 15 : level;
}

// Создание пользовательского символа
void KR1820VG1::createChar(uint8_t num, uint8_t data) {
  // Заглушка - можно реализовать позже
}

// Команда (для совместимости)
void KR1820VG1::command(uint8_t value) {
  switch (value) {
    case 0x01: clear(); break;
    case 0x02: home(); break;
    case 0x0C: display(); break;
    case 0x08: noDisplay(); break;
    case 0x0F: blink(); break;
    case 0x0E: noBlink(); break;
  }
}