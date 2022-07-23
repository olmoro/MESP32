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
    //===================================================================================== MStart

    // Состояние "Старт", инициализация выбранного режима работы (PID).
    /*...*/
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
    Display->showMode((char*)"  PIDTEST START   ");                         // Что регулируется
    Display->showHelp((char*)"            C-GO  ");                         // Активные кнопки
    Board->ledsOn();                                // Подтверждение входа белым свечением светодиода
  }
  MState * MStart::fsm()
  {
    switch ( Keyboard->getKey() )    //Здесь так можно
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                      return new MStop(Tools);
        // Перейти к выбору регулятора
      case MKeyboard::C_CLICK: Board->buzzerOn();                           return new MMode(Tools);
      default:;
    }
    // Индикация текущих значений, указывается число знаков после запятой
    Display->showVolt(Tools->getRealVoltage(), 1);
    Display->showAmp (Tools->getRealCurrent(), 1);                          return this;
  };

//===================================================================================== MMode

    // Состояние: "Выбор ПИД-регулятора".
    /* 0 - отключено
       1 - по напряжению
       2 - по току заряда
       3 - по току разряда
    */
  MMode::MMode(MTools * Tools) : MState(Tools)
  {
    mode = Tools->readNvsInt("pidtest", "mode", fixed);
    #ifdef TESTPID
      Serial.print("\nmode="); Serial.print(mode);
    #endif
      // Индикация
    Display->showMode((char*)"     MODE         ");                         // Что регулируется
    Display->showHelp((char*)" +/-       B-SAVE ");                         // Активные кнопки
    Board->ledsGreen();                                                     // Подтверждение
  }
  MState * MMode::fsm()
  {
    switch (Keyboard->getKey())
    {
      // Отказ от продолжения ввода параметров - стоп
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                        return new MStop(Tools);
    //   // Вернуться
    // case MKeyboard::P_CLICK: Board->buzzerOn();                       return new MShiftV(Tools);
      // Сохранить и перейти к следующему состоянию    
    case MKeyboard::B_CLICK: Board->buzzerOn();
      Tools->saveInt("pidtest", "mode", mode);
      switch (mode)
      {
      case 1:                                                               return new MSetPointV(Tools);  // к pidU
      case 2:                                                               return new MStop(Tools);  // к pidI
      case 3:                                                               return new MStop(Tools);  // к pidD
      default:                                                              return new MStop(Tools);  // отключить
      }

    case MKeyboard::UP_CLICK: Board->buzzerOn();
      mode = Tools->updnInt(mode, below, above, +1); 
      #ifdef TESTPID
        Serial.print("mode="); Serial.print(mode);
      #endif           
      //Tools->txSetSmoothU(mode);                                    // 0x34 Команда драйверу
    break;
    case MKeyboard::DN_CLICK: Board->buzzerOn();
      mode = Tools->updnInt(mode, below, above, -1); 
      #ifdef TESTPID
        Serial.print("mode="); Serial.print(mode);
      #endif
      //Tools->txSetSmoothU(mode);                                    // 0x34 Команда драйверу
    default:;
    }

      // В строке помощи показать результат +/-
      switch (mode)
      {
      case 1:  Display->showMode((char*)"     MODE V       ");  break;
      case 2:  Display->showMode((char*)"     MODE I       ");  break;
      case 3:  Display->showMode((char*)"     MODE D       ");  break;
      default: Display->showMode((char*)"     MODE OFF     ");  break;
      }

      Display->showHelp((char*)" +/-       B-SAVE ");                       // Активные кнопки


      Display->showVolt(Tools->getRealVoltage(), 1);
      Display->showAmp (Tools->getRealCurrent(), 1);                        return this;  
  };  //MMode

//===================================================================================== MSetPointV
    // Состояние: "Включение ПИД-регулятора напряжения".
    /* 1 - регулятор напряжения, задается порог регулирования.
        Начальный уровень - maxV режима CCCV.
    */
  MSetPointV::MSetPointV(MTools * Tools) : MState(Tools)
  {
    setpoint = Tools->readNvsFloat("cccv", "maxV", fixed);
      // Индикация 
    Display->showMode((char*)" SETPOINT_V   +/- ");
    Display->showHelp((char*)" B-SAVE      C-GO ");
    
  }
  MState * MSetPointV::fsm()
  {
    switch ( Keyboard->getKey() )
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                      return new MStop(Tools);

      //   // Отказ от дальнейшего ввода параметров - исполнение
      // case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MPostpone(Tools);

        // Перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();
        Tools->txPowerGo(setpoint, fixedI, 1);                          // 0x20  Команда драйверу
                                                                            //return new MSetKpV(Tools);
                                                                            return new MMode(Tools);

        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK: Board->buzzerOn();
        setpoint = Tools->updnFloat(setpoint, below, above, 0.1f);          break;     
      case MKeyboard::UP_LONG_CLICK: Board->buzzerOn();
        setpoint = Tools->updnFloat(setpoint, below, above, 1.0f);          break;

        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn();
        setpoint = Tools->updnFloat(setpoint, below, above, -0.1f);         break;
      case MKeyboard::DN_LONG_CLICK: Board->buzzerOn();
        setpoint = Tools->updnFloat(setpoint, below, above, -1.0f);         break;
        
      default:;
    }
      // Показать и продолжить
    Display->showVolt(setpoint, 1);
    Display->showAmp(Tools->getRealCurrent(), 3);                       return this;
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
