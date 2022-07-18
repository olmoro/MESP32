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
  //  if(--cnt <= 0)  {return new MTxAdcOffset(Tools);}         // Безусловный выход через 3 секунды
  //  else            {return this;}
//     // Флаг блокировки обмена с драйвером на время его рестарта
// bool MTools::getBlocking() {return blocking;}
// void MTools::setBlocking(bool bl) {blocking = bl;}
    Tools->setBlocking(true);                                                       // Блокировать обмен
//Serial.println(); Serial.print("Заблокировано"); Serial.println(millis());    
    
    vTaskDelay(3000/portTICK_PERIOD_MS);                                            // Не беспокоим драйвер 3 секунды после рестарта 
    
    Tools->setBlocking(false);                                                      // Разблокировать обмен
//Serial.println(); Serial.print("Разблокировано"); Serial.println(millis());
    return new MTxAdcOffset(Tools);                                                // Перейти к следующему параметру
  };

  // Восстановление пользовательской (или заводской) настройки смещения АЦП.
  MTxAdcOffset::MTxAdcOffset(MTools * Tools) : MState(Tools) {}
  MState * MTxAdcOffset::fsm()
  {
    Tools->offsetAdc = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetAdc, 0x0000);   // Взять сохраненное из ЭНОЗУ.
    Tools->txSetAdcOffset(Tools->offsetAdc);                                        // 0x52  Команда драйверу в буфер
    return new MTxsetFactorU(Tools);                                               // Перейти к следующему параметру
  };

  // Восстановление пользовательского (или заводского) коэфициента преобразования в милливольты.
  MTxsetFactorU::MTxsetFactorU(MTools * Tools) : MState(Tools) {}
  MState * MTxsetFactorU::fsm()
  {
    Tools->factorV = Tools->readNvsInt(MNvs::nQulon, MNvs::kFactorV, 0x2DA0);       // Взять сохраненное из ЭНОЗУ.
    Tools->txSetFactorU(Tools->factorV);                                                            // 0x31  Команда драйверу
    return new MTxSmoothU(Tools);                                                  // Перейти к следующему параметру
  };

  // Восстановление пользовательского (или заводского) коэффициента фильтрации по напряжению.
  MTxSmoothU::MTxSmoothU(MTools * Tools) : MState(Tools) {}
  MState * MTxSmoothU::fsm()
  {
    Tools->smoothV = Tools->readNvsInt(MNvs::nQulon, MNvs::kSmoothV, 0x0003);       // Взять сохраненное из ЭНОЗУ.
    Tools->txSetSmoothU(Tools->smoothV);                                                            // 0x34  Команда драйверу
    return new MTxShiftU(Tools);                                                   // Перейти к следующему параметру
  };

  // Восстановление пользовательской (или заводской) настройки сдвига по напряжению.
  MTxShiftU::MTxShiftU(MTools * Tools) : MState(Tools) {}
  MState * MTxShiftU::fsm()
  {
    Tools->shiftV = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetV, 0x0000);       // Взять сохраненное из ЭНОЗУ.
    Tools->txSetShiftU(Tools->shiftV);                                                             // 0x36  Команда драйверу
    return new MTxFactorI(Tools);                                                  // Перейти к следующему параметру
  };

  // Восстановление пользовательского (или заводского) коэфициента преобразования в миллиамперы.
  MTxFactorI::MTxFactorI(MTools * Tools) : MState(Tools) {}
  MState * MTxFactorI::fsm()
  {
    Tools->factorI = Tools->readNvsInt(MNvs::nQulon, MNvs::kFactorA, 0x030C);       // Взять сохраненное из ЭНОЗУ.
    Tools->txSetFactorI(Tools->factorI);                                                            // 0x39  Команда драйверу
    return new MTxSmoothI(Tools);                                                  // Перейти к следующему параметру
  };

  // Восстановление пользовательского (или заводского) коэффициента фильтрации по току.
  MTxSmoothI::MTxSmoothI(MTools * Tools) : MState(Tools) {}
  MState * MTxSmoothI::fsm()
  {
    //Tools->smoothI = Tools->readNvsInt(MNvs::nQulon, MNvs::kSmoothA, 0x0003);       // Взять сохраненное из ЭНОЗУ.
    Tools->smoothI = 0x0003;
    Tools->txSetSmoothI(Tools->smoothI);                                                            // 0x3C  Команда драйверу
    return new MTxShiftI(Tools);                                                   // Перейти к следующему параметру
  };

  // Восстановление пользовательской (или заводской) настройки сдвига по току.
  MTxShiftI::MTxShiftI(MTools * Tools) : MState(Tools) {}
  MState * MTxShiftI::fsm()
  {
    Tools->shiftI = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetA, 0x0000);       // Взять сохраненное из ЭНОЗУ.
    Tools->txSetShiftI(Tools->shiftI);                                                             // 0x3E  Команда драйверу
    return new MExit(Tools);      //return new MTxPidConfigure(Tools); // Перейти к следующему параметру (Временно закончить)
  };

      // Команды работы с ПИД-регулятором
//     case MCmd::cmd_pid_configure:             doPidConfigure();           break;  // 0x40   + 0B->01
// // Состояние "Восстановление пользовательской (или заводской) настройки ПИД-регулятора."
//   MTxPidConfigure::MTxPidConfigure(MTools * Tools) : MState(Tools) {}
//   MState * MTxPidConfigure::fsm()
//   {
//     // Tools->voltageMax  = Tools->readNvsFloat(MNvs::nQulon, MNvs::kCcCvVmax, 14.5f); // Заданное максимальное напряжение заряда, В
//     // Tools->set...();                      // 0x..  Команда драйверу  
//     // return new MTxPidCoefficients;
//     return new MExit(Tools);
//   };

//   //  MTxPidCoefficients::MTxPidCoefficients(MTools * Tools) : MState(Tools)  // 0x41   + 07->01
//   //  MTxPidOutputRange::MTxPidOutputRange(MTools * Tools) : MState(Tools)    // 0x42   + 05->01
//   //  MTxPidReconfigure::MTxPidReconfigure(MTools * Tools) : MState(Tools)    // 0x43   + 0B->01
//   //  MTxPidClear::MTxPidClear(MTools * Tools) : MState(Tools)                // 0x44   + 01->01
//   //  MTxPidTest::MTxPidTest(MTools * Tools) : MState(Tools)                  // 0x46   + 03->01
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
