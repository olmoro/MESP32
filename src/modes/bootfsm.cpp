/*
  syncingfsm.cpp
  Конечный автомат синхронизации данных между контроллерами.
            Как это работает.
    Процесс синхронизации (BOOT) запускается диспетчером при инициализации прибора. 
  Пользовательские параметры (OPTIONS) и параметры разработчика (DEVICE) восстанавливаются из
  энергонезависимой памяти ESP32 и одновременнно передаются на драйвер SAMD21, где заменяют 
  соответствующие дефолтные значения. Во время синхронизации на дисплей выводится информация 
  о ходе синхронизации. По окончании процесса синхронизации прибор готов к работе в выбранном режиме.
  07.07.2022 - 
*/

#include "modes/bootfsm.h"
  #include "mdispatcher.h"
#include "nvs.h"
#include "mtools.h"
#include "mcmd.h"
#include "board/mboard.h"
#include "board/msupervisor.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>
#include <string>

namespace Bootfsm
{
  // Состояние "Старт", инициализация выбранного режима работы.
  // Подтверждение входа синим свечением светодиода
  MStart::MStart(MTools * Tools) : MState(Tools) {Board->ledsRed();}
  MState * MStart::fsm() {return new MSetAdcOffset(Tools);};

  // Состояние "Восстановление пользовательской (или заводской) настройки смещения АЦП".
  MSetAdcOffset::MSetAdcOffset(MTools * Tools) : MState(Tools) {}
  MState * MSetAdcOffset::fsm()
  {
    Tools->offsetAdc = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetAdc, 0x0000);   // Взять сохраненное из ЭНОЗУ.
    Tools->setAdcOffset();                                                          // 0x52  Команда драйверу в буфер
    return new MSetsetFactorU(Tools);                                               // Перейти к следующему параметру
  };

  // Состояние "Восстановление пользовательского (или заводского) коэфициента преобразования в милливольты."
  MSetsetFactorU::MSetsetFactorU(MTools * Tools) : MState(Tools) {}
  MState * MSetsetFactorU::fsm()
  {
    Tools->factorV = Tools->readNvsInt(MNvs::nQulon, MNvs::kFactorV, 0x2DA0);       // Взять сохраненное из ЭНОЗУ.
    Tools->setFactorU();                                                            // 0x32  Команда драйверу
    return new MSetSmoothU(Tools);                                                  // Перейти к следующему параметру
  };

  // Состояние "Восстановление пользовательского (или заводского) коэффициента фильтрации по напряжению."
  MSetSmoothU::MSetSmoothU(MTools * Tools) : MState(Tools) {}
  MState * MSetSmoothU::fsm()
  {
    Tools->smoothV = Tools->readNvsInt(MNvs::nQulon, MNvs::kSmoothV, 0x0003);       // Взять сохраненное из ЭНОЗУ.
    Tools->setSmoothU();                                                            // 0x34  Команда драйверу
    return new MSetShiftU(Tools);                                                   // Перейти к следующему параметру
  };

  // Состояние "Восстановление пользовательской (или заводской) настройки сдвига по напряжению." shift
  MSetShiftU::MSetShiftU(MTools * Tools) : MState(Tools) {}
  MState * MSetShiftU::fsm()
  {
    Tools->offsetV = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetV, 0x0000);       // Взять сохраненное из ЭНОЗУ.
    Tools->setShiftU();                                                             // 0x36  Команда драйверу
    return new MSetFactorI(Tools);                                                  // Перейти к следующему параметру
  };

  // Состояние "Восстановление пользовательского (или заводского) коэфициента преобразования в миллиамперы."
  MSetFactorI::MSetFactorI(MTools * Tools) : MState(Tools) {}
  MState * MSetFactorI::fsm()
  {
    Tools->factorA = Tools->readNvsInt(MNvs::nQulon, MNvs::kFactorA, 0x030C);       // Взять сохраненное из ЭНОЗУ.
    Tools->setFactorI();                                                            // 0x39  Команда драйверу
    return new MSetSmoothI(Tools);                                                  // Перейти к следующему параметру
  };

  // Состояние "Восстановление пользовательского (или заводского) коэффициента фильтрации по току."
  MSetSmoothI::MSetSmoothI(MTools * Tools) : MState(Tools) {}
  MState * MSetSmoothI::fsm()
  {
    Tools->smoothA = Tools->readNvsInt(MNvs::nQulon, MNvs::kSmoothA, 0x0003);       // Взять сохраненное из ЭНОЗУ.
    Tools->setSmoothI();                                                            // 0x3C  Команда драйверу
    return new MSetShiftI(Tools);                                                  // Перейти к следующему параметру
  };

  // Состояние "Восстановление пользовательской (или заводской) настройки сдвига по току."
  MSetShiftI::MSetShiftI(MTools * Tools) : MState(Tools) {}
  MState * MSetShiftI::fsm()
  {
    Tools->offsetA = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetA, 0x0000);       // Взять сохраненное из ЭНОЗУ.
    Tools->setShiftI();                                                             // 0x3E  Команда драйверу
    return new MExit(Tools);      //return new MSetPidConfigure(Tools); // Перейти к следующему параметру (Временно закончить)
  };

      // Команды работы с ПИД-регулятором
//     case MCmd::cmd_pid_configure:             doPidConfigure();           break;  // 0x40   + 0B->01
// // Состояние "Восстановление пользовательской (или заводской) настройки ПИД-регулятора."
//   MSetPidConfigure::MSetPidConfigure(MTools * Tools) : MState(Tools) {}
//   MState * MSetPidConfigure::fsm()
//   {
//     // Tools->voltageMax  = Tools->readNvsFloat(MNvs::nQulon, MNvs::kCcCvVmax, 14.5f); // Заданное максимальное напряжение заряда, В
//     // Tools->set...();                      // 0x..  Команда драйверу  
//     // return new MSetPidCoefficients;
//     return new MExit(Tools);
//   };

//   //  MSetPidCoefficients::MSetPidCoefficients(MTools * Tools) : MState(Tools)  // 0x41   + 07->01
//   //  MSetPidOutputRange::MSetPidOutputRange(MTools * Tools) : MState(Tools)    // 0x42   + 05->01
//   //  MSetPidReconfigure::MSetPidReconfigure(MTools * Tools) : MState(Tools)    // 0x43   + 0B->01
//   //  MSetPidClear::MSetPidClear(MTools * Tools) : MState(Tools)                // 0x44   + 01->01
//   //  MSetPidTest::MSetPidTest(MTools * Tools) : MState(Tools)                  // 0x46   + 03->01
//   //  MGetPidConfigure::MGetPidConfigure(MTools * Tools) : MState(Tools)        // 0x48   + 00->0C




  // Процесс выхода из режима "BOOT".
  // Состояние: "Индикация итогов и выход из режима заряда в меню диспетчера" 
  MExit::MExit(MTools * Tools) : MState(Tools) {Display->showHelp((char*) "   ...READY...    " );}    
  MState * MExit::fsm()
  {
//static constexpr const char* kQulonMode      = "mode";     // 
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);
    //modeSelection   = Tools->readNvsInt ( MNvs::nQulon, MNvs::kQulonMode, CCCVCHARGE );   // Индекс массива

    return nullptr;                                                             // Возврат к выбору режима
  }
};  //MExit

// !Конечный автомат синхронизации данных между контроллерами.
