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
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
    // Индикация
    //Display->initBar(TFT_GREEN);
    Board->ledsBlue();              // Подтверждение входа синим свечением светодиода
  }
  MState * MStart::fsm()
  {
//Serial.println();  Serial.println("       BOOT       ");
    return new MSetAdcOffset(Tools);
  };

  // Состояние "..."
  MSetAdcOffset::MSetAdcOffset(MTools * Tools) : MState(Tools) {}
  MState * MSetAdcOffset::fsm()
  {
    Tools->offsetAdc = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetAdc, 0x0000);  // Смещение ЦАП
    //Tools->setAdcOffset();                    // 0x52  Команда драйверу
  Serial.print("cmd="); Serial.println(0x52, HEX);
    return new MSetsetFactorU(Tools);
  };

  // Состояние "..."
  MSetsetFactorU::MSetsetFactorU(MTools * Tools) : MState(Tools)
  {
    Display->showHelp((char*) "  ...FACTOR V...  ");
  }
  MState * MSetsetFactorU::fsm()
  {
    Tools->factorV = Tools->readNvsInt(MNvs::nQulon, MNvs::kFactorV, 0x2DA0);  // Множитель преобразования

//    Tools->setFactorU();                      // 0x32  Команда драйверу
  Serial.print("cmd="); Serial.println(0x32, HEX);
    return new MSetSmoothU(Tools);
  };

  // Состояние "..."
  MSetSmoothU::MSetSmoothU(MTools * Tools) : MState(Tools)
  {
    Display->showHelp((char*) "  ...SMOOTH V...  ");
  }
  MState * MSetSmoothU::fsm()
  {
    Tools->smoothV = Tools->readNvsInt(MNvs::nQulon, MNvs::kSmoothV, 0x0003);  // Коэффициент фильтрации
//    Tools->setSmoothU();                      // 0x34  Команда драйверу
  Serial.print("cmd="); Serial.println(0x34, HEX);
    return new MSetOffsetU(Tools);
  };

  // Состояние "..."
  MSetOffsetU::MSetOffsetU(MTools * Tools) : MState(Tools)
  {
    Display->showHelp((char*) "  ...OFFSET V...  ");
  }
  MState * MSetOffsetU::fsm()
  {
    Tools->offsetV = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetV, 0x0000);  // Смещение в милливольтах
//    Tools->setOffsetU();                      // 0x36  Команда драйверу
  Serial.print("cmd="); Serial.println(0x36, HEX);
    return new MSetFactorI(Tools);
  };

  // Состояние "..."
  MSetFactorI::MSetFactorI(MTools * Tools) : MState(Tools)
  {
    Display->showHelp((char*) "  ...FACTOR I...  ");
  }
  MState * MSetFactorI::fsm()
  {
    Tools->factorA = Tools->readNvsInt(MNvs::nQulon, MNvs::kFactorA, 0x030C);  // Множитель преобразования

//    Tools->setFactorI();                      // 0x39  Команда драйверу
  Serial.print("cmd="); Serial.println(0x39, HEX);
    return new MSetSmoothI(Tools);
  };

  // Состояние "..."
  MSetSmoothI::MSetSmoothI(MTools * Tools) : MState(Tools)
  {
    Display->showHelp((char*) "  ...SMOOTH I...  ");
  }
  MState * MSetSmoothI::fsm()
  {
    Tools->smoothA = Tools->readNvsInt(MNvs::nQulon, MNvs::kSmoothA, 0x0003);  // Коэффициент фильтрации
//    Tools->setSmoothI();                      // 0x3C  Команда драйверу
  Serial.print("cmd="); Serial.println(0x3C, HEX);
    return new MSetOffsetI(Tools);
  };

  // Состояние "..."
  MSetOffsetI::MSetOffsetI(MTools * Tools) : MState(Tools)
  {
    Display->showHelp((char*) "  ...OFFSET I...  ");
  }
  MState * MSetOffsetI::fsm()
  {
    Tools->offsetA = Tools->readNvsInt(MNvs::nQulon, MNvs::kOffsetA, 0x0000);  // Смещение в миллиамперах
//    Tools->setOffsetI();                      // 0x3E  Команда драйверу
  Serial.print("cmd="); Serial.println(0x3E, HEX);
    return new MExit(Tools);
  };

        // Команды работы с ПИД-регулятором
  //     case MCmd::cmd_pid_configure:             doPidConfigure();           break;  // 0x40   + 0B->01


// Состояние "..."
  MSetPidConfigure::MSetPidConfigure(MTools * Tools) : MState(Tools)
  {
    // Индикация подсказки
    Display->showMode((char*) "       BOOT       ");
    Display->showHelp((char*) " ...PID CONFIG... ");
  }
  MState * MSetPidConfigure::fsm()
  {
    // Tools->voltageMax  = Tools->readNvsFloat(MNvs::nQulon, MNvs::kCcCvVmax, 14.5f); // Заданное максимальное напряжение заряда, В
    // Tools->set...();                      // 0x..  Команда драйверу  
    // return new MSetPidCoefficients;
    return new MExit(Tools);
  };

  //  MSetPidCoefficients::MSetPidCoefficients(MTools * Tools) : MState(Tools)  // 0x41   + 07->01
  //  MSetPidOutputRange::MSetPidOutputRange(MTools * Tools) : MState(Tools)    // 0x42   + 05->01
  //  MSetPidReconfigure::MSetPidReconfigure(MTools * Tools) : MState(Tools)    // 0x43   + 0B->01
  //  MSetPidClear::MSetPidClear(MTools * Tools) : MState(Tools)                // 0x44   + 01->01
  //  MSetPidTest::MSetPidTest(MTools * Tools) : MState(Tools)                  // 0x46   + 03->01
  //  MGetPidConfigure::MGetPidConfigure(MTools * Tools) : MState(Tools)        // 0x48   + 00->0C




  // Процесс выхода из режима .
  // Состояние: "Индикация итогов и выход из режима заряда в меню диспетчера" 
  MExit::MExit(MTools * Tools) : MState(Tools)
  {
    Display->showHelp((char*) "    ...DONE...    " );
    Display->barOff();
  }    
  MState * MExit::fsm()
  {
//static constexpr const char* kQulonMode      = "mode";     // 
    Board->ledsOff();
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);
    //modeSelection   = Tools->readNvsInt ( MNvs::nQulon, MNvs::kQulonMode, CCCVCHARGE );   // Индекс массива

    return nullptr;                             // Возврат к выбору режима
  }
};  //MExit

// !Конечный автомат синхронизации данных между контроллерами.
