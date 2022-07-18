/*
  cccvfsm.cpp
  Конечный автомат заряда в режиме CC/CV.
  Такой алгоритм обеспечивает достаточно быстрый и «бережный» режим заряда аккумулятора. Для исключения
  долговременного пребывания аккумулятора в конце процесса заряда устройство переходит в режим поддержания 
  (компенсации тока саморазряда) напряжения на аккумуляторе. Такой алгоритм называется трехступенчатым. 
  График такого алгоритма заряда представлен на рисунке  http://www.balsat.ru/statia2.php
  27.05.2019 
  23.03.2021 - под новый дисплей
  03.07.2022 - подключение  к драйверу силовой платы
*/

#include "modes/cccvfsm.h"
#include "nvs.h"
#include "mtools.h"
#include "mcmd.h"
#include "board/mboard.h"
#include "board/msupervisor.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>
#include <string>

namespace Cccv
{
  float maxV, minV, maxI, minI;
  

  // Состояние "Старт", инициализация выбранного режима работы (Заряд CCCV).
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
    //Отключить на всякий пожарный
    Tools->powerStop();                                 // 0x21  Команда драйверу

    // Параметры заряда из энергонезависимой памяти, Занесенные в нее при предыдущих включениях, как и
    // выбранные ранее номинальные параметры батареи (напряжение, емкость).
    // При первом включении, как правило заводском, номиналы батареи задаются в mdispather.h. 
    maxV = Tools->readNvsFloat("cccv", "maxV", Tools->getVoltageNom() * MConst::voltageMaxFactor);
//Serial.println(); Serial.print("maxV="); Serial.println(maxV);
    minV = Tools->readNvsFloat("cccv", "minV", Tools->getVoltageNom() * MConst::voltageMinFactor);
    maxI = Tools->readNvsFloat("cccv", "maxI", Tools->getCapacity()   * MConst::currentMaxFactor);
    minI = Tools->readNvsFloat("cccv", "minI", Tools->getCapacity()   * MConst::currentMinFactor);

    // Индикация
    Display->showMode( (char*)"       CC/CV      " );  // В каком режиме
    Display->showHelp( (char*)"    P-ADJ   C-GO  " );  // Активные кнопки
    Display->barOff();
    Board->ledsOn();              // Подтверждение входа в настройки заряда белым свечением светодиода
  }
  MState * MStart::fsm()
  {
    switch ( Keyboard->getKey() )    //Здесь так можно
    {
      case MKeyboard::C_CLICK: Board->buzzerOn(); 
        // Старт без уточнения параметров (здесь – для батарей типа AGM), 
        maxV = Tools->getVoltageNom() * MConst::voltageMaxFactor;
//Serial.println(); Serial.print("maxV="); Serial.println(maxV);
        minV = Tools->getVoltageNom() * MConst::voltageMinFactor;
        maxI = Tools->getCapacity()   * MConst::currentMaxFactor; 
        minI = Tools->getCapacity()   * MConst::currentMinFactor;
      return new MPostpone(Tools);

      case MKeyboard::P_CLICK: Board->buzzerOn();
      return new MSetCurrentMax(Tools);     // Выбрано уточнение настроек заряда, начать с установки maxI
      default:;
    }
    // Индикация текущих значений, указывается число знаков после запятой
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 2);
    return this;
  };

  // Состояние "Коррекция максимального тока заряда"."
  MSetCurrentMax::MSetCurrentMax(MTools * Tools) : MState(Tools)
  {
    // Индикация
    Display->showMode((char*)"    CURRENT MAX   ");  // Регулируемый параметр
    Display->showHelp((char*)"  U/D B-SAVE C-GO ");  //
  }
  MState * MSetCurrentMax::fsm()
  {
    switch (Keyboard->getKey())
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MPostpone(Tools);
        // Сохранить и перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();       
        Tools->saveFloat("cccv", "maxI", maxI);                         return new MSetVoltageMax(Tools);
        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK:  Board->buzzerOn(); 
      case MKeyboard::UP_AUTO_CLICK:  
        maxI = Tools->upfVal( maxI, MConst::i_l, MConst::i_h, 0.1f );   break;
        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn();
      case MKeyboard::DN_AUTO_CLICK:  
        maxI = Tools->dnfVal(maxI, MConst::i_l, MConst::i_h, 0.1f );    break;
      default:;
    }
    // Если не закончили ввод, то индикация введенного и остаться в том же состоянии
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp(maxI, 1);                                          return this;
  };

  // Состояние: "Коррекция максимального напряжения"
  MSetVoltageMax::MSetVoltageMax(MTools * Tools) : MState(Tools)
  {
    // Индикация 
    Display->showMode((char*)" U/D-SET VOLT MAX ");
    Display->showHelp((char*)"  B-SAVE C-START  ");
  }
  MState * MSetVoltageMax::fsm()
  {
    switch ( Keyboard->getKey() )
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MPostpone(Tools);
        // Сохранить и перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();
        Tools->saveFloat("cccv", "maxV", maxV);                         return new MSetCurrentMin(Tools);
        // Увеличить значение на указанную величину
      case MKeyboard::UP_CLICK: Board->buzzerOn();
      case MKeyboard::UP_AUTO_CLICK:  
        maxV = Tools->upfVal(maxV, MConst::v_l, MConst::v_h, 0.1f);     break;
        // Уменьшить значение на указанную величину
      case MKeyboard::DN_CLICK: Board->buzzerOn(); 
      case MKeyboard::DN_AUTO_CLICK: 
        maxV = Tools->dnfVal(maxV, MConst::v_l, MConst::v_h, 0.1f);     break;
      default:;
    }
      // Показать и продолжить
    Display->showVolt(maxV, 1);
    Display->showAmp(Tools->getRealCurrent(), 3);                       return this;
  };

  // Состояние: "Коррекция минимального тока заряда"
  MSetCurrentMin::MSetCurrentMin(MTools * Tools) : MState(Tools)
  {
    // Индикация помощи
    Display->showMode((char*)"    CURRENT MIN   ");
    Display->showHelp((char*)"  U/D B-SAVE C-GO ");
  }   
  MState * MSetCurrentMin::fsm()
  {
    switch (Keyboard->getKey())
    {
        // Отказ от продолжения ввода параметров - стоп
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MPostpone(Tools);
        // Сохранить и перейти к следующему параметру
      case MKeyboard::B_CLICK: Board->buzzerOn();
        Tools->saveFloat("cccv", "minI", minI);                         return new MSetVoltageMin(Tools);
      case MKeyboard::UP_CLICK: Board->buzzerOn();
      case MKeyboard::UP_AUTO_CLICK:
        minI = Tools->upfVal(minI, MConst::i_l, MConst::i_h, 0.1f);     break;
      case MKeyboard::DN_CLICK: Board->buzzerOn();
      case MKeyboard::DN_AUTO_CLICK:
        minI = Tools->dnfVal(minI, MConst::i_l, MConst::i_h, 0.1f);     break;
      default:;
    }
    // Показать и продолжить
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp(minI, 1);                                          return this;
  };

  // Состояние: "Коррекция минимального напряжения окончания заряда"
  MSetVoltageMin::MSetVoltageMin(MTools * Tools) : MState(Tools)
  {
    // Индикация помощи
    Display->showMode((char*)"   VOLTAGE MIN    ");
    Display->showHelp((char*)"  U/D B-SAVE C-GO ");
  }   
  MState * MSetVoltageMin::fsm()
  {
    switch (Keyboard->getKey())
    {
        // Отказ от продолжения ввода параметров - стоп    
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
        // Отказ от дальнейшего ввода параметров - исполнение
      case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MPostpone(Tools);
        // Сохранить и перейти к исполнению
      case MKeyboard::B_CLICK: Board->buzzerOn();
        Tools->saveFloat( "cccv", "minV", minV );                       return new MPostpone(Tools);
      case MKeyboard::UP_CLICK: Board->buzzerOn();
      case MKeyboard::UP_AUTO_CLICK:
        minV = Tools->upfVal( minV, MConst::v_l, MConst::v_h, 0.1f );   break;
      case MKeyboard::DN_CLICK: Board->buzzerOn();
      case MKeyboard::DN_AUTO_CLICK:
        minV = Tools->dnfVal(minV, MConst::v_l, MConst::v_h, 0.1f);     break;
      default:;
    }
    // Индикация ввода
    Display->showVolt(minV, 1);
    Display->showAmp(Tools->getRealCurrent(), 3);                       return this;
  };

  // Состояние: "Задержка включения (отложенный старт)", время ожидания старта задается в OPTIONS.
  MPostpone::MPostpone(MTools * Tools) : MState(Tools)
  {
      // Параметр задержки начала заряда из энергонезависимой памяти, при первом включении - заводское
    Tools->postpone = Tools->readNvsInt(MNvs::nQulon, MNvs::kQulonPostpone, 0);          
      // Индикация помощи
    Display->showMode((char*)"  DELAYED START   ");
    Display->showHelp((char*)"     C-START      ");
      // Инициализация счетчика времени до старта
    Tools->setTimeCounter( Tools->postpone * 36000 );                // Отложенный старт ( * 0.1s в этой версии)
  }     
  MState * MPostpone::fsm()
  {
      // Старт по времени
    if(Tools->postponeCalculation())                                    return new MSetPidCoeffU(Tools);

    switch (Keyboard->getKey())
    {
        // Досрочное прекращение заряда оператором
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                  return new MStop(Tools);
        // Старт оператором
      case MKeyboard::C_CLICK: Board->buzzerOn();                       return new MSetPidCoeffU(Tools);
      default:;
    }
    // Индикация в период ожидания старта (обратный отсчет)
    Display->showDuration( Tools->getChargeTimeCounter(), MDisplay::SEC );
    //Board->blinkWhite();                // Исполняется  некорректно - пока отменено
    return this;
  };

  // *********************************** PID *******************************************
  // Восстановление пользовательских kp, ki, и kd для управления напряжением.
  MSetPidCoeffU::MSetPidCoeffU(MTools * Tools) : MState(Tools) {}
  MState * MSetPidCoeffU::fsm()
  {
    //Взять сохраненные из ЭНОЗУ.
    kpU = Tools->readNvsFloat("cccv", "kpU", 0.06);
    kiU = Tools->readNvsFloat("cccv", "kiU", 0.02);
    kdU = Tools->readNvsFloat("cccv", "kdU", 0.00);
    Tools->txSetPidCoeffU(kpU, kiU, kdU);                                               // 0x41  Команда драйверу
    return new MSetPidCoeffI(Tools);                                                  // Перейти к следующему параметру
  };

    // Восстановление пользовательских kp, ki, и kd для управления током заряда.
  MSetPidCoeffI::MSetPidCoeffI(MTools * Tools) : MState(Tools) {}
  MState * MSetPidCoeffI::fsm()
  {
    //Взять сохраненные из ЭНОЗУ.
    kpI = Tools->readNvsFloat("cccv", "kpI", 0.02);
    kiI = Tools->readNvsFloat("cccv", "kiI", 0.10);
    kdI = Tools->readNvsFloat("cccv", "kdI", 0.00);
    Tools->txSetPidCoeffI(kpI, kiI, kdI);                                               // 0x41  Команда драйверу
    return new MSetPidCoeffD(Tools);                                                  // Перейти к следующему параметру
  };

  // Восстановление пользовательских kp, ki, и kd для управления током разряда.
  MSetPidCoeffD::MSetPidCoeffD(MTools * Tools) : MState(Tools) {}
  MState * MSetPidCoeffD::fsm()
  {
    //Взять сохраненные из ЭНОЗУ.
    kpD = Tools->readNvsFloat("cccv", "kpD", 0.02);    // Уточнить
    kiD = Tools->readNvsFloat("cccv", "kiD", 0.10);
    kdD = Tools->readNvsFloat("cccv", "kdD", 0.00);
    Tools->txSetPidCoeffD(kpD, kiD, kdD);                                               // 0x41  Команда драйверу
    return new MUpCurrent(Tools);                                                     // Перейти к следующему состоянию
  };

  // ************************* Состояния собственно заряда CC/CV *************************
  /*  Начальный этап заряда - ток поднимается не выше заданного уровня, при достижении 
    заданного максимального напряжения переход к его удержанию. 
    Здесь и далее подсчитывается время и отданный заряд, а также сохраняется возможность
    прекращения заряда оператором. */

  // Состояние: "Подъем и удержание максимального тока"
  MUpCurrent::MUpCurrent(MTools * Tools) : MState(Tools)
  {   
      // Индикация входа в режим ConstCurrent
    Display->showMode((char*)"  CONST CURRENT   ");
    Display->showHelp((char*)"      C-STOP      ");
    Board->ledsGreen();
      // Обнуляются счетчики времени и отданного заряда
    Tools->clrTimeCounter();
    Tools->clrAhCharge();

    /* Включение преобразователя и коммутатора драйвером силовой платы.
     Параметры PID-регулятора заданы в настройках прибора (DEVICE).
     Здесь задаются сетпойнты по напряжению и току. Подъем тока
     производится ПИД-регулятором.
    */ 
    Tools->powerGo(Tools->voltageMax * 1.05f, Tools->currentMax, 1);         // 0x20  Команда драйверу
  }     
  MUpCurrent::MState * MUpCurrent::fsm()
  {
    Tools->chargeCalculations();                     // Подсчет отданных ампер-часов.

    switch ( Keyboard->getKey() )
    {
        // Досрочное прекращение заряда оператором
      case MKeyboard::C_CLICK:
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                      return new MStop(Tools);
      default:;
    }

    // Проверка напряжения и переход на поддержание напряжения.
    if(Tools->getRealVoltage() >= maxV)                                     return new MKeepVmax(Tools);
    
      // Индикация фазы подъема тока не выше заданного
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);    // Может быть избыточно 3 знака
    Display->initBar(TFT_GREEN);
    Display->showDuration(Tools->getChargeTimeCounter(), MDisplay::SEC);
    Display->showAh(Tools->getAhCharge());                                  return this;
  };  //MUpCurrent

  /*  Вторая фаза заряда - достигнуто заданное максимальное напряжение.
    Настройки регулятора не меняются, по факту состояние необходимо только для 
    изменения индикации.
    При падении тока ниже заданного уровня - переход к третьей фазе. */

    // Состояние: "Удержание максимального напряжения"
  MKeepVmax::MKeepVmax(MTools * Tools) : MState(Tools)
  {
    // Индикация помощи
    Display->showMode((char*)"  CONST VOLTAGE   ");
    Display->showHelp((char*)"     C-STOP       ");
    Board->ledsYellow();
    //Tools->powerGo(maxV, maxI, 1);         // 0x20  Команда драйверу
  }       
  MState * MKeepVmax::fsm()
  {
    Tools->chargeCalculations();                   // Подсчет отданных ампер-часов.

    //if (Keyboard->getKey(MKeyboard::C_CLICK)) { Board->buzzerOn();  return new MStop(Tools); }                // Окончание процесса оператором.
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK:        // Досрочное прекращение заряда оператором
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();                    return new MStop(Tools);
      default:;
    }

    if( Tools->getRealCurrent() <= Tools->getCurrentMin() )
    return new MKeepVmin(Tools);        // Ожидание спада тока ниже C/20 ампер.

    // Коррекция
//        if( Tools->getRealCurrent() > Tools->getCurrentMax() ) { Tools->adjustIntegral( -0.250f ); } // -0.025A

//        Tools->runPid( Tools->getRealVoltage() ); // Поддержание максимального напряжения.
    
                                              
    // Индикация фазы удержания максимального напряжения
    // Реальные ток и напряжения - без изменения, можно не задавать?
    
    //Display->initBar( TFT_YELLOW );
    Display->showDuration( Tools->getChargeTimeCounter(), MDisplay::SEC );
    Display->showAh( Tools->getAhCharge() );

    return this;
  };

  // Третья фаза заряда - достигнуто снижение тока заряда ниже заданного предела.
  // Проверки различных причин завершения заряда.
  MKeepVmin::MKeepVmin(MTools * Tools) : MState(Tools)
  {
    // Индикация помощи
    Display->showMode( (char*) " KEEP MIN VOLTAGE " );
    Display->showHelp( (char*) "      C-STOP      " );
    Board->ledsYellow();
      // Порог регулирования по минимальному напряжению
    Tools->powerGo(minV, Tools->currentMax, 1);         // 0x20  Команда драйверу
  }     
  MState * MKeepVmin::fsm()
  {
    Tools->chargeCalculations();        // Подсчет отданных ампер-часов.

    // Окончание процесса оператором.
    //if (Keyboard->getKey(MKeyboard::C_CLICK))     { return new MStop(Tools); }       
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK:        // Досрочное прекращение заряда оператором
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();
      return new MStop(Tools);

      default:;
    }
    // Здесь возможны проверки других условий окончания заряда
    // if( ( ... >= ... ) && ( ... <= ... ) )  { return new MStop(Tools); }

    // Максимальное время заряда, задается в "Настройках"
    if( Tools->getChargeTimeCounter() >= ( Tools->charge_time_out_limit * 36000 ) ) 
    { return new MStop(Tools); }

    // Необходимая коррекция против выброса тока
//        if( Tools->getRealCurrent() > Tools->getCurrentMax() ) 
//        { Tools->adjustIntegral( -0.250f ); }        // -0.025A

//        Tools->runPid( Tools->getRealVoltage() );           // Регулировка по напряжению

    Display->initBar( TFT_MAGENTA );
    Display->showDuration( Tools->getChargeTimeCounter(), MDisplay::SEC );
    Display->showAh( Tools->getAhCharge() );

    return this;
  };  //MKeepVmin

  // Завершение режима заряда - до нажатия кнопки "С" удерживается индикация 
  // о продолжительности и отданном заряде.
  // Состояние: "Завершение заряда"
  MStop::MStop(MTools * Tools) : MState(Tools)
  {
    // Команда драйверу отключить преобразователь (0x21) 
    Tools->powerStop();            // 0x21

    Display->showMode( (char*) "     POWER OFF    " );
    Display->showHelp( (char*) "      C-EXIT      " );
    Display->barStop();
    Board->ledsRed();
  }    
  MState * MStop::fsm()
  {

    //Display->showHelp( (char*) "      C-EXIT      " );
    //return new MExit(Tools);

    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK:  Board->buzzerOn();  
      return new MExit(Tools);
      default:;
    }
    return this;
  };  //MStop

  // Процесс выхода из режима заряда - до нажатия кнопки "С" удерживается индикация о продолжительности и отданном заряде.
  // Состояние: "Индикация итогов и выход из режима заряда в меню диспетчера" 
  MExit::MExit(MTools * Tools) : MState(Tools)
  {
    //Tools->shutdownCharge();
    Display->showMode( (char*) "  CC/CV MODE OFF  " );
    Display->showHelp( (char*) " C-TO SELECT MODE " );  // To select the mode
    Board->ledsOff();
    Display->barOff();
  }    
  MState * MExit::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::P_CLICK:  Board->buzzerOn();  
      return new MStart(Tools);                       // Вернуться в начало

      case MKeyboard::C_CLICK:  Board->buzzerOn(); 
        Tools->activateExit(" ");    // Можно сделать лучше, гасит светодиоды
        // Надо бы восстанавливать средствами диспетчера...
        Display->showMode( (char*) "   CC/CV CHARGE   " );
        Display->showHelp( (char*) "     B-SELECT     " );
        return nullptr;                             // Возврат к выбору режима
      default:;
    }
    return this;
  };

};  //MExit

// !Конечный автомат режима простого заряда (CCCV).
