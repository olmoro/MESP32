/*
  cccvfsm.cpp
  Конечный автомат заряда в режиме CC/CV.
  Такой алгоритм обеспечивает достаточно быстрый и «бережный» режим заряда аккумулятора. Для исключения
  долговременного пребывания аккумулятора в конце процесса заряда устройство переходит в режим поддержания 
  (компенсации тока саморазряда) напряжения на аккумуляторе. Такой алгоритм называется трехступенчатым. 
  График такого алгоритма заряда представлен на рисунке  http://www.balsat.ru/statia2.php
  27.05.2019 
  23.03.2021 - под новый дисплей
  03.05.2021 - подключение  к драйверу силовой платы
  06.05.2022 - обработка ошибки протокола при приеме ответа драйвера
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

namespace CcCvFsm
{

  // Состояние "Старт", инициализация выбранного режима работы (Заряд CCCV).
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
    //Tools->setToQueue(MCmd::cmd_power_stop);      // Отключить на всякий пожарный

    // Параметры заряда из энергонезависимой памяти, Занесенные в нее при предыдущих включениях, как и
    // выбранные ранее номинальные параметры батареи (напряжение, емкость).
    // При первом включении, как правило заводском, номиналы батареи задаются в mdispather.h. 
    Tools->setVoltageMax( Tools->readNvsFloat( MNvs::nCcCv, MNvs::kCcCvVmax, MChConsts::voltageMaxFactor * Tools->getVoltageNom()) );
    Tools->setVoltageMin( Tools->readNvsFloat( MNvs::nCcCv, MNvs::kCcCvVmin, MChConsts::voltageMinFactor * Tools->getVoltageNom()) );
    Tools->setCurrentMax( Tools->readNvsFloat( MNvs::nCcCv, MNvs::kCcCvImax, MChConsts::currentMaxFactor * Tools->getCapacity()) );
    Tools->setCurrentMin( Tools->readNvsFloat( MNvs::nCcCv, MNvs::kCcCvImin, MChConsts::currentMinFactor * Tools->getCapacity()) );

    // Индикация
    Display->showMode( (char*) "       CC/CV      " );  // В каком режиме
    Display->showHelp( (char*) "    P-ADJ   C-GO  " );  // Активные кнопки
    Display->barOff();
    Board->ledsOn();              // Подтверждение входа в настройки заряда белым свечением светодиода
  }
  MState * MStart::fsm()
  {
    switch ( Keyboard->getKey() )    //Здесь так можно
    {
      case MKeyboard::C_CLICK: Board->buzzerOn(); 
        // Старт без уточнения параметров (здесь – для батарей типа AGM), 
        // максимальный ток и напряжение окончания - паспортные, исходя из параметров АКБ 
        // Выбор АКБ производится в "Настройках".
        Tools->setVoltageMax( MChConsts::voltageMaxFactor * Tools->getVoltageNom() );
        Tools->setVoltageMin( MChConsts::voltageMinFactor * Tools->getVoltageNom() );
        Tools->setCurrentMax( MChConsts::currentMaxFactor * Tools->getCapacity() );
        Tools->setCurrentMin( MChConsts::currentMinFactor * Tools->getCapacity() );

      return new MPostpone(Tools);

      case MKeyboard::P_CLICK: Board->buzzerOn();
      return new MSetCurrentMax(Tools);     // Выбрано уточнение настроек заряда,
                                            // начать с установки максимального тока
      default:;
    }
    Display->showVolt( Tools->getRealVoltage(), 2 );
    Display->showAmp( Tools->getRealCurrent(), 1 );
    return this;
  };

  // Состояние "Коррекция максимального тока заряда"."
  MSetCurrentMax::MSetCurrentMax(MTools * Tools) : MState(Tools)
  {
    // Индикация подсказки
    Display->showMode( (char*) "    CURRENT MAX   " );  // Регулируемый параметр
    Display->showHelp( (char*) "  U/D B-SAVE C-GO " );  //
  }
  MState * MSetCurrentMax::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      // Что нажато и как 
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();  // Отказ от продолжения ввода параметров - стоп
      return new MStop(Tools);

      case MKeyboard::C_CLICK: Board->buzzerOn();       // Отказ от дальнейшего ввода параметров - исполнение
      return new MPostpone(Tools);

      case MKeyboard::B_CLICK: Board->buzzerOn();       // Сохранить и перейти к следующему параметру
        Tools->saveFloat( MNvs::nCcCv, MNvs::kCcCvImax, Tools->getCurrentMax() ); 
      return new MSetVoltageMax(Tools);

      case MKeyboard::UP_CLICK:  
      case MKeyboard::UP_AUTO_CLICK: Board->buzzerOn();  
        Tools->currentMax = Tools->upfVal( Tools->currentMax, MChConsts::i_l, MChConsts::i_h, 0.1f );
      break;

      case MKeyboard::DN_CLICK:  
      case MKeyboard::DN_AUTO_CLICK: Board->buzzerOn();  
        Tools->currentMax = Tools->dnfVal( Tools->currentMax, MChConsts::i_l, MChConsts::i_h, 0.1f );
      break;

      default:;
    }
    // Если не закончили ввод, то индикация введенного
    Display->showVolt( Tools->getRealVoltage(), 2 );
    Display->showAmp( Tools->getCurrentMax(), 1 );
    return this;        // и остаемся в том же состоянии
  };

  // Состояние: "Коррекция максимального напряжения"
  MSetVoltageMax::MSetVoltageMax(MTools * Tools) : MState(Tools)
  {
    // Индикация помощи
    Display->showMode( (char*) " U/D-SET VOLT MAX " );
    Display->showHelp( (char*) "  B-SAVE C-START  " );
  }
  MState * MSetVoltageMax::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();  // Отказ от продолжения ввода параметров - стоп
      return new MStop(Tools);

      case MKeyboard::C_CLICK: Board->buzzerOn();       // Отказ от дальнейшего ввода параметров - исполнение
      return new MPostpone(Tools);

      case MKeyboard::B_CLICK: Board->buzzerOn();       // Сохранить и перейти к следующему параметру
        Tools->saveFloat( MNvs::nCcCv, MNvs::kCcCvVmax, Tools->getVoltageMax() ); 
      return new MSetCurrentMin(Tools);

      case MKeyboard::UP_CLICK:  
      case MKeyboard::UP_AUTO_CLICK: Board->buzzerOn();  
        Tools->voltageMax = Tools->upfVal( Tools->voltageMax, MChConsts::v_l, MChConsts::v_h, 0.1f );
      break;

      case MKeyboard::DN_CLICK:  
      case MKeyboard::DN_AUTO_CLICK: Board->buzzerOn();  
        Tools->voltageMax = Tools->dnfVal( Tools->voltageMax, MChConsts::v_l, MChConsts::v_h, 0.1f );
      break;

      default:;
    }
    // Индикация ввода
    Display->showVolt( Tools->getVoltageMax(), 1 );
    Display->showAmp( Tools->getRealCurrent(), 1 );
    return this;
  };

  // Состояние: "Коррекция минимального тока заряда"
  MSetCurrentMin::MSetCurrentMin(MTools * Tools) : MState(Tools)
  {
    // Индикация помощи
    Display->showMode( (char*) "    CURRENT MIN   " );
    Display->showHelp( (char*) "  U/D B-SAVE C-GO " );
  }   
  MState * MSetCurrentMin::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();     // Отказ от продолжения ввода параметров - стоп
      return new MStop(Tools);

      case MKeyboard::C_CLICK: Board->buzzerOn();          // Отказ от дальнейшего ввода параметров - исполнение
      return new MPostpone(Tools);

      case MKeyboard::B_CLICK: Board->buzzerOn();          // Сохранить и перейти к следующему параметру
        Tools->saveFloat( MNvs::nCcCv, MNvs::kCcCvImin, Tools->getCurrentMin() ); 
      return new MSetVoltageMin(Tools);

      case MKeyboard::UP_CLICK:
      case MKeyboard::UP_AUTO_CLICK: Board->buzzerOn();
        Tools->currentMin = Tools->upfVal( Tools->currentMin, MChConsts::i_l, MChConsts::i_h, 0.1f );
      break;

      case MKeyboard::DN_CLICK:
      case MKeyboard::DN_AUTO_CLICK: Board->buzzerOn();
        //Tools->decCurrentMin( 0.1f, false );
        Tools->currentMin = Tools->dnfVal( Tools->currentMin, MChConsts::i_l, MChConsts::i_h, 0.1f );
      break;

      default:;
    }
    // Индикация ввода
    Display->showVolt( Tools->getRealVoltage(), 2 );
    Display->showAmp( Tools->getCurrentMin(), 1 );
    return this;
  };

  // Состояние: "Коррекция минимального напряжения окончания заряда"
  MSetVoltageMin::MSetVoltageMin(MTools * Tools) : MState(Tools)
  {
    // Индикация помощи
    Display->showMode( (char*) "   VOLTAGE MIN    " );
    Display->showHelp( (char*) "  U/D B-SAVE C-GO " );
  }   
  MState * MSetVoltageMin::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();    // Отказ от продолжения ввода параметров - стоп
      return new MStop(Tools);

      case MKeyboard::C_CLICK: Board->buzzerOn();         // Отказ от дальнейшего ввода параметров - исполнение
      return new MPostpone(Tools);

      case MKeyboard::B_CLICK: Board->buzzerOn();         // Сохранить и перейти к исполнению
        Tools->saveFloat( MNvs::nCcCv, MNvs::kCcCvVmin, Tools->getVoltageMin() ); 
      return new MPostpone(Tools);

      case MKeyboard::UP_CLICK:
      case MKeyboard::UP_AUTO_CLICK: Board->buzzerOn();
        Tools->voltageMin = Tools->upfVal( Tools->voltageMin, MChConsts::v_l, MChConsts::v_h, 0.1f );
      break;

      case MKeyboard::DN_CLICK:
      case MKeyboard::DN_AUTO_CLICK: Board->buzzerOn();
        Tools->voltageMin = Tools->dnfVal( Tools->voltageMin, MChConsts::v_l, MChConsts::v_h, 0.1f );
      break;

      default:;
    }
    // Индикация ввода
    Display->showVolt( Tools->getVoltageMin(), 1 );
    Display->showAmp( Tools->getRealCurrent(), 1 );
    return this;
  };

  // Состояние: "Задержка включения (отложенный старт)"
  // Время ожидания старта задается в настройках.
  MPostpone::MPostpone(MTools * Tools) : MState(Tools)
  {
    // Параметр задержки начала заряда из энергонезависимой памяти, при первом включении - заводское
    Tools->postpone = Tools->readNvsInt( MNvs::nQulon, MNvs::kQulonPostpone, 0 );
    //Tools->setToQueue( MCmd::cmd_pid_up_current);      // Настройка для фазы плавного подъема тока
    //Tools->setPidCoefficients(0.1f, 0.5f, 0.01f, 250.0f); //float kp, float ki, float kd, float hz
    Tools->setPidCoefficients(2.5f, 12.5f, 0.25f); //float kp, float ki, float kd
            
    // Индикация помощи
    Display->showMode( (char*) "  DELAYED START   " );
    Display->showHelp( (char*) "     C-START      " );

    // Инициализация счетчика времени до старта
    Tools->setTimeCounter( Tools->postpone * 36000 );                // Отложенный старт ( * 0.1s )
  }     
  MState * MPostpone::fsm()
  {
    if( Tools->postponeCalculation() )
    {
      /* Как вариант для плавного увеличения роста применить соответствующие
         настройки коэффициентовПИД-регулятора
        Tools->setPid( 0.1, 0.5, 0,01 );    // KP, KI, KD 
        Tools->setToQueue(cmd_pid_write_coefficients);
      */
      return new MUpCurrent(Tools);    // Старт по времени
    }

    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();  // Досрочное прекращение заряда оператором
      return new MStop(Tools);

      case MKeyboard::C_CLICK: Board->buzzerOn();

      return new MUpCurrent(Tools);
      default:;
    }
    // Индикация в период ожидания старта (обратный отсчет)
    Display->showDuration( Tools->getChargeTimeCounter(), MDisplay::SEC );
    Board->blinkWhite();

    return this;
  };

    // **************************************************************************************

  // Начальный этап заряда - ток поднимается не выше заданного уровня,
  // при достижении заданного максимального напряжения переход к его удержанию.
  // Здесь и далее подсчитывается время и отданный заряд, а также
  // сохраняется возможность прекращения заряда оператором.
  // Состояние: "Подъем и удержание максимального тока"
  MUpCurrent::MUpCurrent(MTools * Tools) : MState(Tools)
  {   
    // Индикация входа в режим ConstCurrent
    Display->showMode( (char*) "  CONST CURRENT   " );
    Display->showHelp( (char*) "      C-STOP      " );
    Board->ledsGreen();
    // Обнуляются счетчики времени и отданного заряда
    Tools->clrTimeCounter();
    Tools->clrAhCharge();

    // Включение преобразователя и коммутатора драйвером силовой платы
    // Параметры PID-регулятора заданы в настройках прибора 
    Tools->setpointU = Tools->voltageMax * 1.05f;   // Voltage limit
    Tools->setpointI = Tools->currentMax;
    Tools->pidMode   = 0x0001;              // TEST
//    Tools->setToQueue(MCmd::cmd_power_go);    
  }     
  MUpCurrent::MState * MUpCurrent::fsm()
  {
    Tools->chargeCalculations();                     // Подсчет отданных ампер-часов.

    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK:                      // Досрочное прекращение заряда оператором
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();
      return new MStop(Tools);

      default:;
    }

    // Проверка напряжения и переход на поддержание напряжения.
    if( Tools->getRealVoltage() >= Tools->getVoltageMax() )
    {
      //Tools->setToQueue(MCmd::cmd_.......); //
      return new MKeepVmax(Tools);
    }

    // Индикация фазы подъема тока не выше заданного
    Display->showVolt( Tools->getRealVoltage(), 2 );
    Display->showAmp( Tools->getRealCurrent(), 1 );

    Display->initBar( TFT_GREEN );
    Display->showDuration( Tools->getChargeTimeCounter(), MDisplay::SEC );
    Display->showAh( Tools->getAhCharge() );
    
    return this;
  };  //MUpCurrent

  // Вторая фаза заряда - достигнуто заданное максимальное напряжение.
  // При падении тока ниже заданного уровня - переход к третьей фазе.
  // Состояние: "Удержание максимального напряжения"
  MKeepVmax::MKeepVmax(MTools * Tools) : MState(Tools)
  {
    // Индикация помощи
    Display->showMode( (char*) "  CONST VOLTAGE   " );
    Display->showHelp( (char*) "     C-STOP       " );

      // Порог регулирования по напряжению
//        Tools->setSetPoint( Tools->getVoltageMax() );
    Tools->setpointU = Tools->voltageMax;   // Voltage limit
    Tools->setpointI = Tools->currentMax;
  }       
  MState * MKeepVmax::fsm()
  {
    Tools->chargeCalculations();                   // Подсчет отданных ампер-часов.

    //if (Keyboard->getKey(MKeyboard::C_CLICK)) { Board->buzzerOn();  return new MStop(Tools); }                // Окончание процесса оператором.
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK:        // Досрочное прекращение заряда оператором
      case MKeyboard::C_LONG_CLICK: Board->buzzerOn();
      return new MStop(Tools);

      default:;
    }

    if( Tools->getRealCurrent() <= Tools->getCurrentMin() )
    return new MKeepVmin(Tools);        // Ожидание спада тока ниже C/20 ампер.

    // Коррекция
//        if( Tools->getRealCurrent() > Tools->getCurrentMax() ) { Tools->adjustIntegral( -0.250f ); } // -0.025A

//        Tools->runPid( Tools->getRealVoltage() ); // Поддержание максимального напряжения.
    
                                              
    // Индикация фазы удержания максимального напряжения
    // Реальные ток и напряжения - без изменения, можно не задавать?
    
    Display->initBar( TFT_YELLOW );
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

      // Порог регулирования по напряжению
//        Tools->setSetPoint( Tools->getVoltageMin() );
    //Tools->setpointU = Tools->voltageMax * 1.05f;   // Voltage limit уточнить
    //Tools->setpointI = Tools->currentMax;   
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
    // Tools->shutdownCharge();
    // Tools->setToQueue(MCmd::cmd_power_stop);  // Отключить 

    Display->showMode( (char*) "     POWER OFF    " );
    Display->showHelp( (char*) "      C-EXIT      " );
    Display->barStop();
  }    
  MState * MStop::fsm()
  {
    // Команда драйверу отключить преобразователь (0x21) 
    if(Tools->powerStop())             // 0x21
    {
      // Ответ драйвера о выполнении команды получен
      Board->ledsRed();
      Display->showHelp( (char*) "      C-EXIT      " );
      return new MExit(Tools);
    }
    else
    {
      // Драйвер не ответил или ответил ошибкой протокола
      Display->showHelp( (char*) "      ERROR 1     " );
      return new MExit(Tools);
    }


    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK:  Board->buzzerOn();  
      return new MExit(Tools);
      default:;
    
//Display->barOff();
    }
    return this;
  };  //MStop

  // Процесс выхода из режима заряда - до нажатия кнопки "С" удерживается индикация о продолжительности и отданном заряде.
  // Состояние: "Индикация итогов и выход из режима заряда в меню диспетчера" 
  MExit::MExit(MTools * Tools) : MState(Tools)
  {
    Tools->shutdownCharge();
    Display->showMode( (char*) "  CC/CV MODE OFF  " );
    Display->showHelp( (char*) " C-TO SELECT MODE " );  // To select the mode

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
