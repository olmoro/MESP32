/*
  Файл: devicefsm.cpp 17.07.2022
  Конечный автомат заводских регулировок - арсенал разработчика (ручной режим):
  - коррекция смещения АЦП в ручном и автоматическом режимах;
  - коррекция приборного смещения и коэффициента преобразования по напряжению;
  - коррекция приборного смещения и коэффициента преобразования по току;
  - коррекция коэффициента фильтрации измерений;
  - 
*/

#include "modes/devicefsm.h"
#include "mtools.h"
  //#include "mcmd.h"
  #include "driver/mcommands.h"
#include "board/mboard.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
  //#include "board/msupervisor.h"
#include <Arduino.h>
  //#include <string>

namespace MDevice
{

  short shift, factor, smooth, par;

    // Состояние "Старт", инициализация выбранного режима работы (DEVICE).
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
      //Отключить на всякий пожарный
    Tools->txPowerStop();                                 // 0x21  Команда драйверу

    // При первом включении, как правило заводском, задается нулевое смещение 
    par = Tools->readNvsInt("qulon", "offsetAdc", MConst::adc_offset);
    #ifdef TESTDEVICE
      Serial.println(); Serial.print("offsetAdc=0x"); Serial.println(par, HEX);
    #endif
    // Индикация
    Display->showMode((char*)"   DEVICE START   ");  // В каком режиме
    Display->showHelp((char*)"    P-ADJ   C-GO  ");  // Активные кнопки
    Display->barOff();
    Board->ledsOn();              // Подтверждение входа в настройки заряда белым свечением светодиода
  }
  MState * MStart::fsm()
  {
    switch ( Keyboard->getKey() )    //Здесь так можно
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
        // Старт автоматической коррекции
      case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MAuto(Tools);
        // Ручная коррекция смещения АЦП
      case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MManual(Tools);
      default:;
    }
    // Индикация текущих значений, указывается число знаков после запятой
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 2);                      return this;
  };

    // Выбран режим автоматической коррекции смещения АЦП
  MAuto::MAuto(MTools * Tools) : MState(Tools)
  {

      // Индикация
    Display->showMode((char*)"       AUTO       ");  // В каком режиме
    Display->showHelp((char*)"  IN DEVELOPMENT  ");  // Активные кнопки
    Display->initBar(TFT_GREEN);
    Board->ledsGreen();              // Подтверждение
  }
  MState * MAuto::fsm()
  {
    switch (Keyboard->getKey())    //Здесь так можно
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
    case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MStop(Tools);
    case MKeyboard::B_CLICK: Board->buzzerOn();                       return new MStop(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();                      return new MStop(Tools);
    case MKeyboard::DN_CLICK: Board->buzzerOn();                      return new MStop(Tools);

    default:;
    }
    return this;
  };

  /*  Выбран режим ручной коррекции смещения АЦП. 
    Перед коррекцией прибор должен быть прогрет в течение нескольких минут, желательно
    под нагрузкой или в режиме разряда. 
      Коррекцию производить, подключив к клеммам "+" и "-" низкоомный резистор или, 
    если не страшно, клеммы надо закоротить перемычкой. И то и другое - проводами 
    минимальной длины.
      Цель коррекции - нулевые или околонулевые показания тока и напряжения. - уточняется  */
  MManual::MManual(MTools * Tools) : MState(Tools)
  {
Tools->setTuningAdc(true);
      // Индикация
    Display->showMode((char*)"  MANUAL    UP/DN ");  // В каком режиме
    Display->showHelp((char*)"  P-NEXT   B-SAVE ");  // Активные кнопки
    Board->ledsGreen();              // Подтверждение
  }
  MState * MManual::fsm()
  {
    switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
      // Сохранить и перейти к следующему состоянию
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->saveInt("qulon", "offsetAdc", par );                     return new MShiftFactorU(Tools);
      // Перейти к следующему состоянию без сохранения
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MShiftFactorU(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      par = Tools->upiVal(par, MConst::adc_l, MConst::adc_h, 1U); 
      #ifdef TESTDEVICE
        Serial.println(); Serial.print("offsetAdc = 0x"); Serial.println(par, HEX);
      #endif           
      Tools->txSetAdcOffset(par);                                 // 0x52  Команда драйверу
    break;
    case MKeyboard::DN_CLICK: Board->buzzerOn();
      par = Tools->dniVal(par, MConst::adc_l, MConst::adc_h, 1U);
      #ifdef TESTDEVICE
        Serial.println(); Serial.print("offsetAdc = 0x"); Serial.println(par, HEX);
      #endif
      Tools->txSetAdcOffset(par);                                 // 0x52  Команда драйверу

    break;
    default:;
    }
      // Индикация ввода в "попугаях" АЦП
      Display->showVolt(Tools->getAdcV() / 100, 3);
      Display->showAmp (Tools->getAdcI() / 100, 3); 
    return this;
  };  //MManual


  /*  Выбран режим коррекции приборного смещения (сдвига) по напряжению. 
    Перед коррекцией прибор должен быть прогрет в течение нескольких минут, желательно
    под нагрузкой или в режиме разряда. 
      Коррекцию производить, подключив к клеммам "+" и "-"  внешний источник с регулируемым
    напряжением порядка 12 вольт по четырёхточечной схеме и эталонный измеритель напряжения. 
      Прибор, кстати, отобразит ток, потребляемый высокоомным входным делителем порядка 
    40 килоом, что свиделельствует об исправности входных цепей измерителей.
      Цель коррекции - минимальные отклонения во всем диапазоне от -2 до +17 вольт. 
    Процесс коррекции сдвига чередовать с коррекцией коэффициента пересчета. Переход
    между этими состояниями производится кнопкой "P". */


  /* Переходное состояние
  */
  MShiftFactorU::MShiftFactorU(MTools * Tools) : MState(Tools)
  {
Tools->setTuningAdc(false);
      // Индикация здесь и далее 
//    Display->showMode((char*)"  FACTOR    UP/DN ");  // В каком режиме
//    Display->showHelp((char*)"  P-NEXT   B-SAVE ");  // Активные кнопки
    Board->ledsGreen();              // Подтверждение
  }
  MState * MShiftFactorU::fsm()
  {
    // Из-за парной регулировки ... это состояние.
    shift  = Tools->readNvsInt("qulon", "offsetV", MConst::shift_u);
    #ifdef TESTDEVICE
      Serial.println(); Serial.print("ShiftV="); Serial.println(shift);
    #endif

    factor = Tools->readNvsInt("qulon", "factorV", MConst::smooth_u);
    #ifdef TESTDEVICE
      Serial.println(); Serial.print("FactorV="); Serial.println(factor, HEX);
    #endif

    return new MShiftU(Tools);
  };

  MShiftU::MShiftU(MTools * Tools) : MState(Tools)
  {

      // Индикация
    Display->showMode((char*)" SHIFT_U    UP/DN ");  // В каком режиме
    Display->showHelp((char*)" P-FACT_U  B-SAVE ");  // Активные кнопки
    //Board->ledsGreen();              // Подтверждение
  }
  MState * MShiftU::fsm()
  {
    switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MFactorU(Tools);
      // Сохранить и перейти к следующему состоянию
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->saveInt("qulon", "offsetV", shift );                     return new MFactorU(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      par = Tools->upiVal(par, MConst::shift_u_l, MConst::shift_u_h, 1U); 
      #ifdef TESTDEVICE
        Serial.println(); Serial.print("shift = 0x"); Serial.println(shift, HEX);
      #endif           
      Tools->txSetShiftU(shift);                                 // 0x36  Команда драйверу
    break;
    case MKeyboard::DN_CLICK: Board->buzzerOn();
      par = Tools->dniVal(par, MConst::shift_u_l, MConst::shift_u_h, 1U);
      #ifdef TESTDEVICE
        Serial.println(); Serial.print("shift = 0x"); Serial.println(shift, HEX);
      #endif
      Tools->txSetShiftU(shift);                                 // 0x36  Команда драйверу

    default:;
    }
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 2);                    return this;
  };  //MShiftU



  MFactorU::MFactorU(MTools * Tools) : MState(Tools)
  {
      // Индикация
    Display->showMode((char*)" FACTOR_U   UP/DN ");  // В каком режиме
    Display->showHelp((char*)" P-SHIFT_U B-SAVE ");  // Активные кнопки
    //Board->ledsGreen();              // Подтверждение
  }
  MState * MFactorU::fsm()
  {
      switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
      // Сохранить и перейти к следующему состоянию

      // Вернуться
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MShiftU(Tools);


    default:;
    }  
    return this;
  };  //MFactorU







  // Завершение режима DEVICE - до нажатия кнопки "С" удерживается индикация 
  MStop::MStop(MTools * Tools) : MState(Tools)
  {
    Display->showMode((char*) "       READY      ");
    Display->showHelp((char*) "      C-EXIT      ");
    Display->barStop();
    Board->ledsRed();
  }    
  MState * MStop::fsm()
  {
    switch ( Keyboard->getKey() )
    {
    case MKeyboard::C_CLICK:  Board->buzzerOn();        return new MExit(Tools);
    default:;
    }
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 2);      return this;
  };  //MStop

  // Процесс выхода из режима - до нажатия кнопки "С" удерживается индикация
  // о завершении.
  // Состояние: "Индикация итогов и выход из режима в меню диспетчера" 
  MExit::MExit(MTools * Tools) : MState(Tools)
  {
    //Tools->shutdownCharge();
    Display->showMode((char*)" DEVICE MODE OFF  ");
    Display->showHelp((char*)"  P-AGAIN  C-EXIT ");  // To select the mode
    Board->ledsOff();
    Display->barOff();
  }    
  MState * MExit::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      // Вернуться в начало
    case MKeyboard::P_CLICK:  Board->buzzerOn();        return new MStart(Tools);

    case MKeyboard::C_CLICK:  Board->buzzerOn(); 
        //Tools->activateExit(" ");    // Можно сделать лучше, гасит светодиоды
      // Надо бы восстанавливать средствами диспетчера...
      Display->showMode( (char*) "   ????????????   " );
      Display->showHelp( (char*) "     B-SELECT     " );    return nullptr;
    default:;
    }
    return this;
  };

};
