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
  // Старт и инициализация выбранного режима работы.
  MStart::MStart(MTools * Tools) : MState(Tools) 
  {
    Board->ledsBlue();                              // Подтверждение входа синим свечением светодиода как и любую загрузку
    Tools->Keyboard->getKey(MKeyboard::UP_CLICK);
    //cnt = duration;
  }
  MState * MStart::fsm()
  {
  //  if(--cnt <= 0)  {return new MSetAdcOffset(Tools);}         // Безусловный выход через 3 секунды
  //  else            {return this;}
//     // Флаг блокировки обмена с драйвером на время его рестарта
// bool MTools::getBlocking() {return blocking;}
// void MTools::setBlocking(bool bl) {blocking = bl;}
    Tools->setBlocking(true);                                                       // Блокировать обмен
Serial.println(); Serial.print("Заблокировано"); Serial.println(millis());    
    vTaskDelay(2500/portTICK_PERIOD_MS);                                            // Не беспокоим драйвер 3 секунды после рестарта 
    Tools->setBlocking(false);                                                      // Разблокировать обмен
Serial.println(); Serial.print("Разблокировано"); Serial.println(millis());
    return new MSetAdcOffset(Tools);                                                // Перейти к следующему параметру
  };

  // Восстановление пользовательской (или заводской) настройки смещения АЦП.
  MSetAdcOffset::MSetAdcOffset(MTools * Tools) : MState(Tools) {}
  MState * MSetAdcOffset::fsm()
  {
    Tools->offsetAdc = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetAdc, 0x0000);   // Взять сохраненное из ЭНОЗУ.
    Tools->setAdcOffset(Tools->offsetAdc);                                          // 0x52  Команда драйверу в буфер
    return new MSetsetFactorU(Tools);                                               // Перейти к следующему параметру
  };

  // Восстановление пользовательского (или заводского) коэфициента преобразования в милливольты.
  MSetsetFactorU::MSetsetFactorU(MTools * Tools) : MState(Tools) {}
  MState * MSetsetFactorU::fsm()
  {
    Tools->factorV = Tools->readNvsInt(MNvs::nQulon, MNvs::kFactorV, 0x2DA0);       // Взять сохраненное из ЭНОЗУ.
    Tools->setFactorU();                                                            // 0x31  Команда драйверу
    return new MSetSmoothU(Tools);                                                  // Перейти к следующему параметру
  };

  // Восстановление пользовательского (или заводского) коэффициента фильтрации по напряжению.
  MSetSmoothU::MSetSmoothU(MTools * Tools) : MState(Tools) {}
  MState * MSetSmoothU::fsm()
  {
    Tools->smoothV = Tools->readNvsInt(MNvs::nQulon, MNvs::kSmoothV, 0x0003);       // Взять сохраненное из ЭНОЗУ.
    Tools->setSmoothU();                                                            // 0x34  Команда драйверу
    return new MSetShiftU(Tools);                                                   // Перейти к следующему параметру
  };

  // Восстановление пользовательской (или заводской) настройки сдвига по напряжению.
  MSetShiftU::MSetShiftU(MTools * Tools) : MState(Tools) {}
  MState * MSetShiftU::fsm()
  {
    Tools->offsetV = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetV, 0x0000);       // Взять сохраненное из ЭНОЗУ.
    Tools->setShiftU();                                                             // 0x36  Команда драйверу
    return new MSetFactorI(Tools);                                                  // Перейти к следующему параметру
  };

  // Восстановление пользовательского (или заводского) коэфициента преобразования в миллиамперы.
  MSetFactorI::MSetFactorI(MTools * Tools) : MState(Tools) {}
  MState * MSetFactorI::fsm()
  {
    Tools->factorA = Tools->readNvsInt(MNvs::nQulon, MNvs::kFactorA, 0x030C);       // Взять сохраненное из ЭНОЗУ.
    Tools->setFactorI();                                                            // 0x39  Команда драйверу
    return new MSetSmoothI(Tools);                                                  // Перейти к следующему параметру
  };

  // Восстановление пользовательского (или заводского) коэффициента фильтрации по току.
  MSetSmoothI::MSetSmoothI(MTools * Tools) : MState(Tools) {}
  MState * MSetSmoothI::fsm()
  {
    Tools->smoothA = Tools->readNvsInt(MNvs::nQulon, MNvs::kSmoothA, 0x0003);       // Взять сохраненное из ЭНОЗУ.
    Tools->setSmoothI();                                                            // 0x3C  Команда драйверу
    return new MSetShiftI(Tools);                                                   // Перейти к следующему параметру
  };

  // Восстановление пользовательской (или заводской) настройки сдвига по току.
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
    Board->ledsOff();
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);
    //modeSelection = Tools->readNvsInt ( MNvs::nQulon, MNvs::kQulonMode, CCCVCHARGE );   // Индекс массива
    //modeSelection = Tools->readNvsInt ( MNvs::nQulon, MNvs::kQulonMode, 0 );   // Индекс массива

    return nullptr;                                                             // Возврат к выбору режима
  }
};  //MExit

// !Конечный автомат синхронизации данных между контроллерами.
