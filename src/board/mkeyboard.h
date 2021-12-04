#ifndef _MKEYBOARD_H_
#define _MKEYBOARD_H_

#include <Arduino.h>  // only to include String - is there a better way?
#include "stdint.h"

class SButton;
class MButton;

class MKeyboard
{
  public:
    MKeyboard();
    ~MKeyboard();

    void calcKeys(short ivalue);

    enum KEY_PRESS
    {
      KEY_NO = 0,
      P_CLICK,   P_AUTO_CLICK,  P_LONG_CLICK,
      UP_CLICK, UP_AUTO_CLICK, UP_LONG_CLICK,
      DN_CLICK, DN_AUTO_CLICK, DN_LONG_CLICK,
      C_CLICK,   C_AUTO_CLICK,  C_LONG_CLICK,
      B_CLICK,   B_AUTO_CLICK,  B_LONG_CLICK
    };

    // Expected button weight
    static constexpr short adc_no   =  1023;            // Нет нажатия
    static constexpr short adc_p    = (1023 * 5) / 6;   // Кнопка "Р"  - режим
    static constexpr short adc_up   = (1023 * 4) / 6;   // Кнопка "UP" - вверх
    static constexpr short adc_dn   = (1023 * 3) / 6;   // Кнопка "DN" - вниз
    static constexpr short adc_c    = (1023 * 2) / 6;   // Кнопка "С"  - Старт/Стоп
    static constexpr short adc_b    =  1023 / 6;        // Кнопка "В"  - выбор

    bool getKey(uint8_t key);
    KEY_PRESS getKey();

  private:
    uint8_t keyBuff = 0;     // Код нажатой кнопки
    uint16_t KeyInput;

    static constexpr int numButtons = 5;
    MButton* Buttons[numButtons];

};

class MButton
{
  public:
    MButton(SButton * Button, MKeyboard::KEY_PRESS Click, MKeyboard::KEY_PRESS LongClick, MKeyboard::KEY_PRESS AutoClick, const String & DebugName);
    ~MButton();

    void check(uint8_t & keyBuff);

  private:
    SButton * Button = nullptr;

    MKeyboard::KEY_PRESS Click;
    MKeyboard::KEY_PRESS LongClick;
    MKeyboard::KEY_PRESS AutoClick;
    String               DebugName;
};

#endif // !_MKEYBOARD_H_
