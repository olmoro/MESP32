/*
  Файл: devicefsm.cpp 17.07.2022
  Конечный автомат заводских регулировок - арсенал разработчика (ручной режим):
  - коррекция приборного смещения и коэффициента преобразования по напряжению;
  - коррекция приборного смещения и коэффициента преобразования по току;
  - коррекция коэффициента фильтрации измерений;
  - 
    Перед коррекцией прибор должен быть прогрет в течение нескольких минут, желательно
  под нагрузкой или в режиме разряда. 
    Коррекцию производить, подключив к клеммам "+" и "-"  внешний источник с регулируемым
  напряжением порядка 12 вольт по четырёхточечной схеме и эталонный измеритель напряжения. 
    Прибор, кстати, отобразит ток, потребляемый высокоомным входным делителем порядка 
  40 килоом, что свиделельствует об исправности входных цепей измерителей.
    Цель коррекции - минимальные отклонения во всем диапазоне от -2 до +17 вольт. 
  Процесс коррекции сдвига чередовать с коррекцией коэффициента пересчета. Переход
  между этими состояниями производится кнопкой "P".
    При коррекции сдвигов следует иметь ввиду, что одно нажатие на "+" или "-" добавляется 
  не к милливольтам  или миллиамперам, а данным АЦП до их преобразования в физические величины.
*/

#include "modes/devicefsm.h"
#include "mtools.h"
#include "board/mboard.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>

namespace MDevice
{
    //===================================================================================== MStart

    // Состояние "Старт", инициализация выбранного режима работы (DEVICE).
    /*...*/
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
      //Отключить на всякий пожарный
    Tools->txPowerStop();                                               // 0x21 Команда драйверу
    cnt = 7;
      // Индикация
    Display->showMode((char*)"   DEVICE START   ");                     // Что регулируется
    Display->showHelp((char*)" P-ADJ_I  C-ADJ_V ");                     // Активные кнопки
    Board->ledsOn();                                // Подтверждение входа белым свечением светодиода
  }
  MState * MStart::fsm()
  {
    switch ( Keyboard->getKey() )    //Здесь так можно
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
        // Коррекция смещения по напряжению
      case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MShiftV(Tools);
        // Коррекция смещения по току
      case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MShiftI(Tools);
      case MKeyboard::B_CLICK: Board->buzzerOn();
        if(--cnt <= 0)                                                  return new MClearCccvKeys(Tools);
        break;
      default:;
    }
    // Индикация текущих значений, указывается число знаков после запятой
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);                      return this;
  };

    //================================================================================ MClearCccvKeys

  MClearCccvKeys::MClearCccvKeys(MTools * Tools) : MState(Tools)
  {
    Display->showMode((char*)"      CLEAR?      ");   // В каком режиме
    Display->showHelp((char*)"  P-NO     C-YES  ");   // Активные кнопки
    Board->ledsBlue();
    cnt = 50;                                         // 5с 
  }
  MState * MClearCccvKeys::fsm()
  {
    switch  (Keyboard->getKey())
    {
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MShiftV(Tools);
    case MKeyboard::C_CLICK: Board->buzzerOn();
      done = Tools->clearAllKeys("device");
      vTaskDelay(2 / portTICK_PERIOD_MS);
      #ifdef TEST_KEYS_CLEAR
        Serial.print("\nAll keys \"device\": ");
        (done) ? Serial.println("cleared") : Serial.println("err");
      #endif
      break;
    default:                                                          break;
    }
    if(--cnt <= 0)                                                    return new MStart(Tools);
    Display->showMode((char*)"     CLEARING     ");   // В каком режиме
    Display->showHelp((char*)"  ___C-CLEAR___   ");   // Активные кнопки - нет
    return this;
  };


  //===================================================================================== MShiftV

    // Состояние: "Коррекция приборного смещения (сдвига) по напряжению".
  /*  
    Перед коррекцией прибор должен быть прогрет в течение нескольких минут, желательно
    под нагрузкой или в режиме разряда. 
      Коррекцию производить, подключив к клеммам "+" и "-"  внешний источник с регулируемым
    напряжением порядка 12 вольт по четырёхточечной схеме и эталонный измеритель напряжения. 
      Прибор, кстати, отобразит ток, потребляемый высокоомным входным делителем порядка 
    40 килоом, что свиделельствует об исправности входных цепей измерителей.
      Цель коррекции - минимальные отклонения во всем диапазоне от -2 до +17 вольт. 
    Процесс коррекции сдвига чередовать с коррекцией коэффициента пересчета. Переход
    между этими состояниями производится кнопкой "P". */
  MShiftV::MShiftV(MTools * Tools) : MState(Tools)
  {
    shift = Tools->readNvsShort("device", "offsetV", fixed);
    #ifdef TESTDEVICE
      Serial.print("\nshiftV=0x"); Serial.print(shift, HEX);
    #endif
      // Индикация
    Display->showMode((char*)"    SHIFT_V +/-   ");                   // Что регулируется
    Display->showHelp((char*)" P-FACT_V  B-SAVE ");                   // Активные кнопки
    Board->ledsGreen();                                               // Подтверждение
  }
  MState * MShiftV::fsm()
  {
    switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MFactorV(Tools);
      // Сохранить и перейти к следующему состоянию
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->writeNvsShort("device", "offsetV", shift);                
    #ifdef TESTDEVICE
      Serial.print("\nshiftVw=0x"); Serial.print(shift, HEX);
      Serial.print("\nshiftVr=0x"); Serial.print(Tools->readNvsShort("device", "offsetV", fixed), HEX);
    #endif      
      
            return new MFactorV(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      shift = Tools->updnInt(shift, below, above, +1); 
      #ifdef TESTDEVICE
        Serial.print("\nshiftV=0x"); Serial.print(shift, HEX);
      #endif           
      Tools->txSetShiftU(shift);                                      // 0x36 Команда драйверу
    break;
    case MKeyboard::DN_CLICK: Board->buzzerOn();
      shift = Tools->updnInt(shift, below, above, -1); 
      #ifdef TESTDEVICE
        Serial.print("\nshiftV=0x"); Serial.print(shift, HEX);
      #endif
      Tools->txSetShiftU(shift);                                      // 0x36 Команда драйверу

    default:;
    }
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);                    return this;
  };  //MShiftV

  //===================================================================================== MFactorV

    // Состояние: "Коррекция коэффициента преобразования в милливольты".
    /*...*/
  MFactorV::MFactorV(MTools * Tools) : MState(Tools)
  {
    factor = Tools->readNvsShort("device", "factorV", fixed);
    #ifdef TESTDEVICE
      Serial.print("\nfactorV=0x"); Serial.print(factor, HEX);
    #endif
      // Индикация
    Display->showMode((char*)"   FACTOR_V +/-   ");                   // Что регулируется
    Display->showHelp((char*)" P-SHIFT_V B-SAVE ");                   // Активные кнопки
    //Board->ledsGreen();                                             // Подтверждение
  }
  MState * MFactorV::fsm()
  {
      switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
      // Вернуться
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MShiftV(Tools);
      // Сохранить и перейти к следующему состоянию    
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->writeNvsShort("device", "factorV", factor);                     return new MSmoothV(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      factor = Tools->updnInt(factor, below, above, +1); 
      #ifdef TESTDEVICE
        Serial.print("\nfactorV=0x"); Serial.print(factor, HEX);
      #endif           
      Tools->txSetShiftU(factor);                                     // 0x39 Команда драйверу
    break;
    case MKeyboard::DN_CLICK: Board->buzzerOn();
      factor = Tools->updnInt(factor, below, above, -1); 
      #ifdef TESTDEVICE
        Serial.print("\nfactor=0x"); Serial.print(factor, HEX);
      #endif
      Tools->txSetFactorU(factor);                                    // 0x39 Команда драйверу
    default:;
    }
      Display->showVolt(Tools->getRealVoltage(), 3);
      Display->showAmp (Tools->getRealCurrent(), 3);                  return this;  
  };  //MFactorV

  //===================================================================================== MSmoothV

    // Состояние: "Коррекция коэффициента фильтрации по току".
    /*...*/
  MSmoothV::MSmoothV(MTools * Tools) : MState(Tools)
  {
    smooth = Tools->readNvsShort("device", "smoothV", fixed);
    #ifdef TESTDEVICE
      Serial.print("\nsmoothV=0x"); Serial.print(smooth, HEX);
    #endif
      // Индикация
    Display->showMode((char*)"   SMOOTH_V +/-   ");                   // Что регулируется
    Display->showHelp((char*)" P-SHIFT_I B-SAVE ");                   // Активные кнопки
    //Board->ledsGreen();                                             // Подтверждение
  }
  MState * MSmoothV::fsm()
  {
      switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
      // Вернуться
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MShiftV(Tools);
      // Сохранить и перейти к следующему состоянию    
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->writeNvsShort("device", "smoothV", smooth);                     return new MShiftI(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      smooth = Tools->updnInt(smooth, below, above, +1); 
      #ifdef TESTDEVICE
        Serial.print("smoothV=0x"); Serial.print(smooth, HEX);
      #endif           
      Tools->txSetSmoothU(smooth);                                    // 0x34 Команда драйверу
    break;
    case MKeyboard::DN_CLICK: Board->buzzerOn();
      smooth = Tools->updnInt(smooth, below, above, -1); 
      #ifdef TESTDEVICE
        Serial.print("smoothV=0x"); Serial.print(smooth, HEX);
      #endif
      Tools->txSetSmoothU(smooth);                                    // 0x34 Команда драйверу
    default:;
    }
      Display->showVolt(Tools->getRealVoltage(), 3);
      Display->showAmp (Tools->getRealCurrent(), 3);                  return this;  
  };  //MSmoothV

  //===================================================================================== MShiftI
  
    // Состояние: "Коррекция приборного смещения (сдвига) по току".
    /*...*/
  MShiftI::MShiftI(MTools * Tools) : MState(Tools)
  {
    shift = Tools->readNvsShort("device", "offsetI", fixed);
    #ifdef TESTDEVICE
      Serial.print("\nshiftI=0x"); Serial.print(shift, HEX);
    #endif
    Display->showMode((char*)"    SHIFT_I +/-   ");                   // Что регулируется
    Display->showHelp((char*)" P-FACT_I  B-SAVE ");                   // Активные кнопки
  }
  MState * MShiftI::fsm()
  {
    switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MFactorI(Tools);
      // Сохранить и перейти к следующему состоянию
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->writeNvsShort("device", "offsetI", shift);                      return new MFactorI(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      shift = Tools->updnInt(shift, below, above, +1); 
      #ifdef TESTDEVICE
        Serial.print("\nshiftI=0x"); Serial.print(shift, HEX);
      #endif           
      Tools->txSetShiftI(shift);                                      // 0x3E Команда драйверу
    break;
    case MKeyboard::DN_CLICK: Board->buzzerOn();
      shift = Tools->updnInt(shift, below, above, -1); 
      #ifdef TESTDEVICE
        Serial.print("\nshiftI=0x"); Serial.print(shift, HEX);
      #endif
      Tools->txSetShiftI(shift);                                      // 0x3E Команда драйверу

    default:;
    }
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);                    return this;
  };

  //===================================================================================== MFactorI

    // Состояние: "Коррекция коэффициента преобразования в миллиамперы".
    /*...*/
  MFactorI::MFactorI(MTools * Tools) : MState(Tools)
  {
    factor = Tools->readNvsShort("device", "factorI", fixed);
    #ifdef TESTDEVICE
      Serial.print("\nfactorI="); Serial.print(factor, HEX);
    #endif
      // Индикация
    Display->showMode((char*)"   FACTOR_I +/-   ");                   // Что регулируется
    Display->showHelp((char*)" P-SHIFT_I B-SAVE ");                   // Активные кнопки
    Board->ledsGreen();                                               // Подтверждение
  }
  MState * MFactorI::fsm()
  {
      switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
      // Вернуться
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MShiftI(Tools);
      // Сохранить и перейти к следующему состоянию    
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->writeNvsShort("device", "factorI", factor);                     return new MSmoothI(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      factor = Tools->updnInt(factor, below, above, +1); 
      #ifdef TESTDEVICE
        Serial.print("\nfactorI = 0x"); Serial.print(factor, HEX);
      #endif           
      Tools->txSetFactorI(factor);                                    // 0x39 Команда драйверу
    break;
    case MKeyboard::DN_CLICK: Board->buzzerOn();
      factor = Tools->updnInt(factor, below, above, -1); 
      #ifdef TESTDEVICE
        Serial.print("\nfactorI = 0x"); Serial.print(factor, HEX);
      #endif
      Tools->txSetFactorI(factor);                                    // 0x39 Команда драйверу
    default:;
    }
      Display->showVolt(Tools->getRealVoltage(), 3);
      Display->showAmp (Tools->getRealCurrent(), 3);                  return this;  
  };  //MFactorI

  //===================================================================================== MSmoothI

    // Состояние: "Коррекция коэффициента фильтрации по току".
    /*...*/
  MSmoothI::MSmoothI(MTools * Tools) : MState(Tools)
  {
    smooth = Tools->readNvsShort("device", "smoothI", fixed);
    #ifdef TESTDEVICE
      Serial.print("\nsmoothI=0x"); Serial.print(smooth, HEX);
    #endif
      // Индикация
    Display->showMode((char*)"   SMOOTH_I +/-   ");                   // Что регулируется
    Display->showHelp((char*)" P-SHIFT_I B-SAVE ");                   // Активные кнопки
    Board->ledsGreen();                                               // Подтверждение
  }
  MState * MSmoothI::fsm()
  {
    switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
      // Вернуться
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MShiftI(Tools);
      // Сохранить и перейти к следующему состоянию    
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->writeNvsShort("device", "smoothI", smooth);                     return new MExit(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      smooth = Tools->updnInt(smooth, below, above, +1); 
      #ifdef TESTDEVICE
        Serial.print("\nsmoothI=0x"); Serial.print(smooth, HEX);
      #endif           
      Tools->txSetSmoothI(smooth);                                    // 0x3C Команда драйверу
    break;
    case MKeyboard::DN_CLICK: Board->buzzerOn();
      smooth = Tools->updnInt(smooth, below, above, -1); 
      #ifdef TESTDEVICE
        Serial.print("\nsmoothI=0x"); Serial.print(smooth, HEX);
      #endif
      Tools->txSetSmoothI(smooth);                                    // 0x3C Команда драйверу
    default:;
    }
      Display->showVolt(Tools->getRealVoltage(), 3);
      Display->showAmp (Tools->getRealCurrent(), 3);                  return this;  
  };  //MSmoothI

// C:\Users\olmor\.platformio\packages\framework-arduinoespressif32@3.10006.210326\libraries\Preferences
  MClearAllKeys::MClearAllKeys(MTools * Tools) : MState(Tools)
  {
    
  }
  MState * MClearAllKeys::fsm()
  {
        switch (Keyboard->getKey())
    {
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MRemoveKey(Tools);
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->clearAllKeys("qulon");                      // delay???  
      // Tools->clearAllKeys("template");                      // delay???  
      // Tools->clearAllKeys("s-power");                      // delay???  
      // Tools->clearAllKeys("cccv");                      // delay???  
      // Tools->clearAllKeys("pidtest");                      // delay???  
      // Tools->clearAllKeys("e-charge");                      // delay???  
      // Tools->clearAllKeys("recovery");                      // delay???  
      // Tools->clearAllKeys("storage");                      // delay???  
      // Tools->clearAllKeys("service");                      // delay???  





                                                                      return new MExit(Tools);



    default:;
    }

    return new MRemoveKey(Tools);
  };

  MRemoveKey::MRemoveKey(MTools * Tools) : MState(Tools)
  {

  }
  MState * MRemoveKey::fsm()
  {

    return new MExit(Tools);
  };





  //===================================================================================== MStop

    // Состояние: ""
    /* Завершение режима DEVICE - до нажатия кнопки "С" удерживается индикация.*/
  MStop::MStop(MTools * Tools) : MState(Tools)
  {
    Display->showMode((char*) "       READY      ");
    Display->showHelp((char*) "      C-EXIT      ");
    Board->ledsRed();                                                 // Подтверждение
  }    
  MState * MStop::fsm()
  {
    switch ( Keyboard->getKey() )
    {
    case MKeyboard::C_CLICK:  Board->buzzerOn();                      return new MExit(Tools);
    default:;
    }
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);                    return this;
  };  //MStop

  //===================================================================================== MExit

    // Процесс выхода из режима - до нажатия кнопки "С" удерживается индикация о завершении.
    /* Состояние: "Индикация итогов и выход из режима в меню диспетчера". */ 
  MExit::MExit(MTools * Tools) : MState(Tools)
  {
    Display->showMode((char*)" DEVICE MODE OFF  ");
    Display->showHelp((char*)"  P-AGAIN  C-EXIT ");  // To select the mode
    Board->ledsOff();
  }    
  MState * MExit::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      // Вернуться в начало
    case MKeyboard::P_CLICK:  Board->buzzerOn();                      return new MStart(Tools);

    case MKeyboard::C_CLICK:  Board->buzzerOn(); 
      // Надо бы восстанавливать средствами диспетчера...
      Display->showMode((char*) "      DEVICE:     ");
      Display->showHelp((char*) "  CALIBRATION ETC ");                return nullptr;
    default:;
    }
    return this;
  };

};
