/*
  Файл: pidfsm.cpp 23.07.2022

  Инструмент разработчика, облегчающий настройку ПИД-регуляторов.

*/

#include "modes/pidfsm.h"
#include "mtools.h"
#include "board/mboard.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>

namespace MPid
{
  float spV;
  float spI;
  float spD;

  //short mode;

  float kpV = 0.02;
  float kiV = 0.20;
  float kdV = 0.00;

  float kpI = 0.02;
  float kiI = 0.20;
  float kdI = 0.00;

  float kpD = 0.02;
  float kiD = 0.20;
  float kdD = 0.00; 
    //===================================================================================== MStart

    // Состояние "Старт", инициализация выбранного режима работы (PID).
    /*...*/
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
    m = Tools->readNvsShort("pidtest", "mode", fixed);
    cnt = 7;
    Tools->txPowerStop();                               // 0x21  Команда драйверу перейти в безопасный режим
    Display->showMode((char*)"  PIDTEST START   ");     // Что регулируется
    Display->showHelp((char*)" CH/DECHAR.  +/-  ");     // Активные кнопки
    Board->ledsOn();                                    // Подтверждение входа белым свечением светодиода
  }
  MState * MStart::fsm()
  {
    switch (Keyboard->getKey())    //Здесь так можно
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();    return new MStop(Tools);
      // Сохранить и начать
    case MKeyboard::C_CLICK: Board->buzzerOn();
      // Tools->saveInt("pidtest", "mode", m);
      if(m == 0)                                        return new MSetPointV(Tools);
      else                                              return new MSetPointD(Tools);

    case MKeyboard::B_CLICK: Board->buzzerOn();
      if(--cnt <= 0)                                                      return new MClearCccvKeys(Tools);
      break;    



    case MKeyboard::UP_CLICK: Board->buzzerOn();
      m = Tools->updnInt(m, below, above, +1); 
      #ifdef TESTPID
        Serial.print("\nm="); Serial.print(m);
      #endif
      break;

    case MKeyboard::DN_CLICK: Board->buzzerOn();
      m = Tools->updnInt(m, below, above, -1); 
      #ifdef TESTPID
        Serial.print("\nm="); Serial.print(m);
      #endif
      break;

      default:;
    }
    if(m == 0) Display->showMode((char*)"   CHARGE  C-GO   ");    // Тестировать ПИД-регулятор заряда
    else       Display->showMode((char*)" DECHARGE  C-GO   ");    // Тестировать ПИД-регулятор разряда              

      // Индикация текущих значений, указывается число знаков после запятой
    Display->showVolt(Tools->getRealVoltage(), 2);
    Display->showAmp (Tools->getRealCurrent(), 2);                return this;
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
    case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MSetPointV(Tools);
    case MKeyboard::C_CLICK: Board->buzzerOn();
      done = Tools->clearAllKeys("pidtest");
      vTaskDelay(2 / portTICK_PERIOD_MS);
      #ifdef TEST_KEYS_CLEAR
        Serial.print("\nAll keys \"pidtest\": ");
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


//===================================================================================== MSetPointV
    // Состояние: "Ввод порога ПИД-регулятора напряжения".
  MSetPointV::MSetPointV(MTools * Tools) : MState(Tools)
  {
    spV = Tools->readNvsFloat("pidtest", "spV", fixedV);
    spI = Tools->readNvsFloat("pidtest", "spI", fixedI);
      // Индикация 
    Display->showMode((char*)" SET SP_V    +/-  ");
    Display->showHelp((char*)" B-SAVE     C-GO  ");
    Board->ledsYellow();                                // Подтверждение выбора желтым свечением светодиода
  }
  MState * MSetPointV::fsm()
  {
    switch (Keyboard->getKey())
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();            return new MStop(Tools);

        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                 return new MGo(Tools);

        // Запомнить и перейти к вводу spI
      case MKeyboard::B_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "spV", spV);                  
        return new MSetPointI(Tools);

        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK: Board->buzzerOn();
        spV = Tools->updnFloat(spV, below, above, 0.1f);          break;     
      case MKeyboard::UP_LONG_CLICK: Board->buzzerOn();
        spV = Tools->updnFloat(spV, below, above, 1.0f);          break;

        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn();
        spV = Tools->updnFloat(spV, below, above, -0.1f);         break;
      case MKeyboard::DN_LONG_CLICK: Board->buzzerOn();
        spV = Tools->updnFloat(spV, below, above, -1.0f);         break;
        
      default:;
    }
      // Показать и продолжить 
    Display->showVolt(spV, 1);
    Display->showAmp(spI, 1);                                     return this;
  };

//===================================================================================== MSetPointI
    // Состояние: "Ввод порога ПИД-регулятора тока".
  MSetPointI::MSetPointI(MTools * Tools) : MState(Tools)
  {
    spV = Tools->readNvsFloat("pidtest", "spV", fixedV);
    spI = Tools->readNvsFloat("pidtest", "spI", fixedI);
      // Индикация 
    Display->showMode((char*)" SET SP_I    +/-  ");
    Display->showHelp((char*)" B-SAVE     C-GO  ");
    Board->ledsYellow();                                // Подтверждение выбора желтым свечением светодиода
  }
  MState * MSetPointI::fsm()
  {
    switch (Keyboard->getKey())
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();            return new MStop(Tools);

        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                 return new MGo(Tools);

        // Сохранить и перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "spI", spI);                  
        return new MSelectKpV(Tools);

        // Сохранить и вернуться на шаг
      case MKeyboard::P_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "spI", spI);                  
        return new MSetPointV(Tools);

        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK: Board->buzzerOn();
        spI = Tools->updnFloat(spI, below, above, 0.1f);          break;     
      case MKeyboard::UP_LONG_CLICK: Board->buzzerOn();
        spI = Tools->updnFloat(spI, below, above, 1.0f);          break;

        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn();
        spI = Tools->updnFloat(spI, below, above, -0.1f);         break;
      case MKeyboard::DN_LONG_CLICK: Board->buzzerOn();
        spI = Tools->updnFloat(spI, below, above, -1.0f);         break;
        
      default:;
    }
      // Показать и продолжить
    Display->showVolt(spV, 1);
    Display->showAmp(spI, 1);                           return this;
  };

//===================================================================================== MSelectKpV

  MSelectKpV::MSelectKpV(MTools * Tools) : MState(Tools)
  {

    kpV = Tools->readNvsFloat("pidtest", "kpU", fixed);

      // Индикация 
    Display->showMode((char*)" KP_V        +/-  ");
    Display->showHelp((char*)" B-SAVE     C-GO  ");
    Board->ledsYellow();                                // Подтверждение выбора желтым свечением светодиода
  }
  MState * MSelectKpV::fsm()
  {
    switch (Keyboard->getKey())
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();            return new MStop(Tools);

        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                 return new MGo(Tools);

        // Сохранить и перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kpV", kpV);                  
        return new MSelectKiV(Tools);

        // Сохранить и и вернуться на шаг
      case MKeyboard::P_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kpV", kpV);                  
        return new MSetPointI(Tools);

        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK: Board->buzzerOn();
        kpV = Tools->updnFloat(kpV, below, above, 0.1f);          break;     
      case MKeyboard::UP_LONG_CLICK: Board->buzzerOn();
        kpV = Tools->updnFloat(kpV, below, above, 1.0f);          break;

        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn();
        kpV = Tools->updnFloat(kpV, below, above, -0.1f);         break;
      case MKeyboard::DN_LONG_CLICK: Board->buzzerOn();
        kpV = Tools->updnFloat(kpV, below, above, -1.0f);         break;
        
      default:;
    }

  //  Display->showPar(kpV, 2);   ПОКА НЕ РЕАЛИЗОВАНО
    Display->showVolt(Tools->getRealVoltage(), 2);
    Display->showAmp(Tools->getRealCurrent(), 2);                 return this;
  };

//===================================================================================== MSelectKiV

  MSelectKiV::MSelectKiV(MTools * Tools) : MState(Tools)
  {
    kiV = Tools->readNvsFloat("pidtest", "kiV", fixed);
      // Индикация 
    Display->showMode((char*)" KI_V        +/-  ");
    Display->showHelp((char*)" B-SAVE     C-GO  ");
    Board->ledsYellow();                                // Подтверждение выбора желтым свечением светодиода
  }
  MState * MSelectKiV::fsm()
  {
    switch (Keyboard->getKey())
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();            return new MStop(Tools);

        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                 return new MGo(Tools);

        // Сохранить и перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kiV", kiV);                  
        return new MSelectKdV(Tools);

        // Сохранить и вернуться на шаг
      case MKeyboard::P_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kiV", kiV);                  
        return new MSelectKpV(Tools);

        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK: Board->buzzerOn();
        kiV = Tools->updnFloat(kiV, below, above, 0.1f);          break;     
      case MKeyboard::UP_LONG_CLICK: Board->buzzerOn();
        kiV = Tools->updnFloat(kiV, below, above, 1.0f);          break;

        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn();
        kiV = Tools->updnFloat(kiV, below, above, -0.1f);         break;
      case MKeyboard::DN_LONG_CLICK: Board->buzzerOn();
        kiV = Tools->updnFloat(kiV, below, above, -1.0f);         break;
        
      default:;
    }

  //  Display->showPar(kpV, 2);
    Display->showVolt(Tools->getRealVoltage(), 2);
    Display->showAmp(Tools->getRealCurrent(), 2);                 return this;

  };

//===================================================================================== MSelectKdV

  MSelectKdV::MSelectKdV(MTools * Tools) : MState(Tools)
  {
    kdV = Tools->readNvsFloat("pidtest", "kdU", fixed);

      // Индикация 
    Display->showMode((char*)" KD_V        +/-  ");
    Display->showHelp((char*)" B-SAVE     C-GO  ");
        Board->ledsYellow();                                // Подтверждение выбора желтым свечением светодиода

  }
  MState * MSelectKdV::fsm()
  {
    switch ( Keyboard->getKey() )
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();            return new MStop(Tools);

        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                 return new MGo(Tools);

        // Сохранить и перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kdV", kdV);                  
        return new MSelectKpI(Tools);

        // Сохранить и вернуться на шаг
      case MKeyboard::P_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kdV", kdV);                  
        return new MSelectKiV(Tools);

        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK: Board->buzzerOn();
        kdV = Tools->updnFloat(kdV, below, above, 0.1f);          break;     
      case MKeyboard::UP_LONG_CLICK: Board->buzzerOn();
        kdV = Tools->updnFloat(kdV, below, above, 1.0f);          break;

        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn();
        kdV = Tools->updnFloat(kdV, below, above, -0.1f);         break;
      case MKeyboard::DN_LONG_CLICK: Board->buzzerOn();
        kdV = Tools->updnFloat(kdV, below, above, -1.0f);         break;
        
      default:;
    }

  //  Display->showPar(kpV, 2);
    Display->showVolt(Tools->getRealVoltage(), 2);
    Display->showAmp(Tools->getRealCurrent(), 2);                 return this;

  };


//===================================================================================== MSelectKpI

  MSelectKpI::MSelectKpI(MTools * Tools) : MState(Tools)
  {
    kpV = Tools->readNvsFloat("pidtest", "kpI", fixed);

      // Индикация 
    Display->showMode((char*)" KP_I        +/-  ");
    Display->showHelp((char*)" B-SAVE     C-GO  ");
    Board->ledsYellow();                                // Подтверждение выбора желтым свечением светодиода
  }
  MState * MSelectKpI::fsm()
  {
    switch ( Keyboard->getKey() )
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();            return new MStop(Tools);

        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                 return new MGo(Tools);

        // Сохранить и перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kpI", kpI);                  
        return new MSelectKiI(Tools);

        // Сохранить и вернуться на шаг
      case MKeyboard::P_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kpI", kpI);                  
        return new MSelectKdV(Tools);

        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK: Board->buzzerOn();
        kpI = Tools->updnFloat(kdI, below, above, 0.1f);          break;     
      case MKeyboard::UP_LONG_CLICK: Board->buzzerOn();
        kdI = Tools->updnFloat(kdI, below, above, 1.0f);          break;

        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn();
        kdI = Tools->updnFloat(kdI, below, above, -0.1f);         break;
      case MKeyboard::DN_LONG_CLICK: Board->buzzerOn();
        kdI = Tools->updnFloat(kdI, below, above, -1.0f);         break;
        
      default:;
    }

  //  Display->showPar(kpI, 2);
    Display->showVolt(Tools->getRealVoltage(), 2);
    Display->showAmp(Tools->getRealCurrent(), 2);                 return this;
  };

//===================================================================================== MSelectKiI

  MSelectKiI::MSelectKiI(MTools * Tools) : MState(Tools)
  {
    kiI = Tools->readNvsFloat("pidtest", "kiI", fixed);

      // Индикация 
    Display->showMode((char*)" KI_I        +/-  ");
    Display->showHelp((char*)" B-SAVE     C-GO  ");
    Board->ledsYellow();                                // Подтверждение выбора желтым свечением светодиода
  }
  MState * MSelectKiI::fsm()
  {
    switch ( Keyboard->getKey() )
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();            return new MStop(Tools);

        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                 return new MGo(Tools);

        // Сохранить и перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kiI", kiI);                  
        return new MSelectKdI(Tools);

        // Сохранить и вернуться на шаг
      case MKeyboard::P_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kiI", kiI);                  
        return new MSelectKpI(Tools);

        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK: Board->buzzerOn();
        kiI = Tools->updnFloat(kiI, below, above, 0.1f);          break;     
      case MKeyboard::UP_LONG_CLICK: Board->buzzerOn();
        kiI = Tools->updnFloat(kiI, below, above, 1.0f);          break;

        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn();
        kiI = Tools->updnFloat(kiI, below, above, -0.1f);         break;
      case MKeyboard::DN_LONG_CLICK: Board->buzzerOn();
        kiI = Tools->updnFloat(kiI, below, above, -1.0f);         break;
        
      default:;
    }

  //  Display->showPar(kiI, 2);
    Display->showVolt(Tools->getRealVoltage(), 2);
    Display->showAmp(Tools->getRealCurrent(), 2);                 return this;

  };

//===================================================================================== MSelectKdI

  MSelectKdI::MSelectKdI(MTools * Tools) : MState(Tools)
  {
    kdI = Tools->readNvsFloat("pidtest", "kdI", fixed);
      // Индикация 
    Display->showMode((char*)" KD_I        +/-  ");
    Display->showHelp((char*)" B-SAVE     C-GO  ");
    Board->ledsYellow();                                // Подтверждение выбора желтым свечением светодиода
  }
  MState * MSelectKdI::fsm()
  {
    switch ( Keyboard->getKey() )
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();            return new MStop(Tools);

        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                 return new MGo(Tools);

        // Сохранить и перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kdI", kdI);                  
        return new MSelectKpD(Tools);

        // Сохранить и вернуться на шаг
      case MKeyboard::P_CLICK: Board->buzzerOn();
        // Tools->saveFloat("pidtest", "kdI", kdI);                  
        return new MSelectKiI(Tools);

        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK: Board->buzzerOn();
        kdI = Tools->updnFloat(kdI, below, above, 0.1f);          break;     
      case MKeyboard::UP_LONG_CLICK: Board->buzzerOn();
        kdI = Tools->updnFloat(kdI, below, above, 1.0f);          break;

        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn();
        kdI = Tools->updnFloat(kdI, below, above, -0.1f);         break;
      case MKeyboard::DN_LONG_CLICK: Board->buzzerOn();
        kdI = Tools->updnFloat(kdI, below, above, -1.0f);         break;
        
      default:;
    }

  //  Display->showPar(kdI, 2);
    Display->showVolt(Tools->getRealVoltage(), 2);
    Display->showAmp(Tools->getRealCurrent(), 2);                 return this;

  };




  // разряд

  MSetPointD::MSetPointD(MTools * Tools) : MState(Tools)
  {
    Display->showMode((char*)"     DECHARGE     ");                         // Что регулируется

  }
  MState * MSetPointD::fsm()
  {
    switch ( Keyboard->getKey() )
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                      return new MStop(Tools);

      //   // Отказ от дальнейшего ввода параметров - исполнение
      // case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MGo(Tools);

        
      default:;
    }    
    return this;
  };







//===================================================================================== MSelectKpD

  MSelectKpD::MSelectKpD(MTools * Tools) : MState(Tools)
  {
  }
  MState * MSelectKpD::fsm()
  {


    
    return new MStop(Tools);
  };


//===================================================================================== MGo

  MGo::MGo(MTools * Tools) : MState(Tools)
  {

    Board->ledsGreen();                                // Подтверждение пуска зеленым свечением светодиода

  }
  MState * MGo::fsm()
  {

  //      Tools->txPowerGo(setpoint, fixedI, 1);                          // 0x20  Команда драйверу


    return new MStop(Tools);
  };


//===================================================================================== MStop

    // Состояние: "СТОП"
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
    Display->showMode((char*)" PIDTEST MODE OFF ");
    Display->showHelp((char*)" P-AGAIN   C-EXIT ");  // To select the mode
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
      Display->showMode((char*) "     TEMPLATE     ");
      Display->showHelp((char*) "      EXAMPLE     ");                return nullptr;
    default:;
    }
    return this;
  };

};
