#ifndef KR1820VG1_H
#define KR1820VG1_H

#include <Arduino.h>
#include <Print.h>

class KR1820VG1 : public Print {
  public:
    KR1820VG1(uint8_t dataPin, uint8_t clkPin, uint8_t cs1Pin, uint8_t cs2Pin);
    
    void begin();
    void clear();
    void home();
    void display();
    void noDisplay();
    void blink();
    void noBlink();
    void setCursor(uint8_t pos);
    uint8_t getCursor();
    
    virtual size_t write(uint8_t value);
    using Print::write;
    
    void printTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
    void printDigits(uint8_t digits[8]);
    void setBrightness(uint8_t level);
    void createChar(uint8_t num, uint8_t data);
    
    void command(uint8_t value);
    
  private:
    uint8_t _dataPin, _clkPin, _cs1Pin, _cs2Pin;
    uint8_t _cursorPos;
    bool _displayControl;
    bool _blinkControl;
    uint8_t _brightness;
    
    uint8_t _displayBuffer[8];
    
    uint8_t _charToSegments(uint8_t ch);
    void _strob();
    void _writeBits(uint8_t data);
    void _writeZeros(uint8_t count);
    void _initDisplay();
    void _updateDisplay();
    void _sendToDisplay(uint8_t chip, uint8_t data[4]);
};

#endif