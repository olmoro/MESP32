/*


*/

#include "modes/optionsfsm.h"
#include "nvs.h"
#include "mtools.h"
#include "mcmd.h"
#include "board/mboard.h"
#include "board/mkeyboard.h"
#include "display/mdisplay.h"
#include <Arduino.h>
#include <string>


namespace OptionFsm
{
  // static int mode = QULON;
  // char sName[ 15 ] = { 0 };   // Ограничение ESP32


  // Состояние "Старт", инициализация выбранного режима работы.
  MStart::MStart(MTools * Tools) : MState(Tools)
  {
    #ifdef DEBUG_OPTIONS
      Serial.println("Options: Start");
    #endif
      // Индикация
    Display->showMode( (char*) " OPTIONS SELECTED " );
    Display->showHelp( (char*) "  P-NEXT  C-STOP  " );
    Board->ledsOn();
  }
  MState * MStart::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK: Board->buzzerOn();
      return new MStop(Tools);

      case MKeyboard::P_CLICK: Board->buzzerOn();
          // Продолжение выбора объекта настройки
      return new MSetPostpone(Tools);

      // case MKeyboard::B_CLICK: Board->buzzerOn();
      //   Tools->setToQueue(MCmd::cmd_read_offset_u);       // Взять актуальное смещение
      // return new MSetVoltageOffset(Tools);
      
      default:;
    }
    return this;
  };


MSetPostpone::MSetPostpone(MTools * Tools) : MState(Tools) 
{
    // Подсказка
    Display->showMode((char*) "  U/D-SET POSTPONE   ");
    Display->showHelp((char*) "  P-DEFINE   C-EXIT  ");
}
MState * MSetPostpone::fsm()
{
    switch (Keyboard->getKey())
    {
    case MKeyboard::C_LONG_CLICK: Board->buzzerOn();
    return new MExit(Tools);

    case MKeyboard::P_CLICK: Board->buzzerOn();
    return new MUpDnAdcOffset(Tools);

    case MKeyboard::UP_CLICK:
    case MKeyboard::UP_AUTO_CLICK: Board->buzzerOn();
      Tools->postpone = Tools->upiVal( Tools->postpone, MOptConsts::ppone_l, MOptConsts::ppone_h, 1 );
    break;

    case MKeyboard::DN_CLICK:
    case MKeyboard::DN_AUTO_CLICK: Board->buzzerOn();
      Tools->postpone = Tools->dniVal( Tools->postpone, MOptConsts::ppone_l, MOptConsts::ppone_h, 1 );
    break;

    case MKeyboard::B_CLICK :
      Tools->saveInt( MNvs::nQulon, MNvs::kQulonPostpone, Tools->postpone );   // Выбор заносится в энергонезависимую память
        // #ifdef DEBUG_OPTIONS
        //     Serial.println(Tools->postpone);
        // #endif
    return new MUpDnAdcOffset(Tools);
    
    default:;
    }

    Display->showDuration( Tools->postpone, MDisplay::HOUR );
    return this;
};

  MUpDnAdcOffset::MUpDnAdcOffset(MTools * Tools) : MState(Tools)
  {
    // Значение восстановлено из nvs при инициализации прибора

    Display->showMode((char*) " ADC OFFSET UP/DN ");
    Display->showHelp((char*) " B-Y P-NEXY C-STOP");

        Board->ledsGreen();
  }
  MState * MUpDnAdcOffset::fsm()
  {
    switch(Keyboard->getKey())
    {
    case MKeyboard::C_CLICK: Board->buzzerOn();                         return new MStop(Tools);
    case MKeyboard::P_CLICK: Board->buzzerOn();                         return new MSetVoltageOffset(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();                                              // Смещение по 1 за шаг
      Tools->offsetAdc = Tools->upiVal(Tools->offsetAdc, MOptConsts::offset_adc_l, MOptConsts::offset_adc_h, 1);
      Tools->setAdcOffset();                      // 0x21  Команда драйверу
    break;
    
    case MKeyboard::DN_CLICK: Board->buzzerOn();                                              // Смещение по 1 за шаг
      Tools->offsetAdc = Tools->dniVal(Tools->offsetAdc, MOptConsts::offset_adc_l, MOptConsts::offset_adc_h, 1);
      Tools->setAdcOffset();                      // 0x21  Команда драйверу
    break;
    
    case MKeyboard::B_CLICK: Board->buzzerOn();                                               // Сохранить
      Tools->saveInt(MNvs::nQulon, MNvs::kOffsetAdc, Tools->offsetAdc); return new MSetVoltageOffset(Tools);
    default:;  
    }
    // Изменение смещения отображается на текущем значении (спорно)
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);                      return this;
  };



  MSetVoltageOffset::MSetVoltageOffset(MTools * Tools) : MState(Tools)
  {
    // Значение восстановлено из nvs при инициализации прибора
    // Индикация помощи
    Display->showMode((char*) "  V OFFSET UP/DN  ");
    Display->showHelp((char*) " B-Y P-NEXT C-STOP");
        Board->ledsGreen();
  }
  MState * MSetVoltageOffset::fsm()
  {
    switch(Keyboard->getKey())
    {
    case MKeyboard::C_CLICK: Board->buzzerOn();                         return new MStop(Tools);
    case MKeyboard::P_CLICK: Board->buzzerOn();                         return new MSetVoltageFactor(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();                                      // Смещение по 1 мВ за шаг
      Tools->offsetV = Tools->upiVal(Tools->offsetV, MOptConsts::offset_v_l, MOptConsts::offset_v_h, 1);
      Tools->setShiftU();                      // 0x36  Команда драйверу
    break;
    
    case MKeyboard::DN_CLICK: Board->buzzerOn();                                      // Смещение по 1 мВ за шаг
      Tools->offsetV = Tools->dniVal(Tools->offsetV, MOptConsts::offset_v_l, MOptConsts::offset_v_h, 1);
      Tools->setShiftU();                      // 0x36  Команда драйверу
    break;
    
    case MKeyboard::B_CLICK: Board->buzzerOn();                                       // Сохранить
      Tools->saveInt(MNvs::nQulon, MNvs::kOffsetV, Tools->offsetV);     return new MSetVoltageFactor(Tools);

    default:;
    }
    // Изменение смещения отображается на текущем значении (спорно)
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);                      return this;
  };

  MSetVoltageFactor::MSetVoltageFactor(MTools * Tools) : MState(Tools)
  {
      // Значение восстановлено из nvs при инициализации прибора
      // Индикация помощи
      Display->showMode((char*) "  V FACTOR UP/DN  ");
      Display->showHelp((char*) " B-YES P-NO C-EX  ");
  }
  MState * MSetVoltageFactor::fsm()
  {
    switch(Keyboard->getKey())
    {
    case MKeyboard::C_CLICK: Board->buzzerOn();                        return new MStop(Tools);
    case MKeyboard::P_CLICK: Board->buzzerOn();                        return new MSetVoltageSmooth(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      Tools->upiVal(Tools->factorV, MOptConsts::factor_v_l, MOptConsts::factor_v_h, 1);
      Tools->setFactorU();                      // 0x31  Команда драйверу
    break;
    
    case MKeyboard::DN_CLICK : Board->buzzerOn();
      Tools->dniVal(Tools->factorV, MOptConsts::factor_v_l, MOptConsts::factor_v_h, 1);
      Tools->setFactorU();                      // 0x31  Команда драйверу
    break;
    
    case MKeyboard::B_CLICK : Board->buzzerOn();
      Tools->saveInt( MNvs::nQulon, MNvs::kFactorV, Tools->factorV);  return new MSetVoltageSmooth(Tools);
    default :;
    }
    // Изменение смещения отображается на текущем значении (спорно)
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);                      return this;
  };


  MSetVoltageSmooth::MSetVoltageSmooth(MTools * Tools) : MState(Tools)
  {
      // Значение восстановлено из nvs при инициализации прибора
      // Индикация помощи
      Display->showMode((char*) "  V SMOOTH UP/DN  ");
      Display->showHelp((char*) " B-YES P-NO C-EX  ");
  }
  MState * MSetVoltageSmooth::fsm()
  {
    switch(Keyboard->getKey())
    {
    case MKeyboard::C_CLICK: Board->buzzerOn();                        return new MStop(Tools);
    case MKeyboard::P_CLICK: Board->buzzerOn();                        return new          MSetVoltageSmooth(Tools);
    case MKeyboard::UP_CLICK: Board->buzzerOn();
      Tools->upiVal(Tools->smoothV, MOptConsts::smooth_v_l, MOptConsts::smooth_v_h, 1);
      Tools->setSmoothU();                      // 0x34  Команда драйверу
    break;
    
    case MKeyboard::DN_CLICK : Board->buzzerOn();
      Tools->dniVal(Tools->smoothV, MOptConsts::smooth_v_l, MOptConsts::smooth_v_h, 1);
      Tools->setSmoothU();                      // 0x34  Команда драйверу
    break;
    
    case MKeyboard::B_CLICK : Board->buzzerOn();
      Tools->saveInt( MNvs::nQulon, MNvs::kSmoothV, Tools->smoothV);    return new MSetVoltageSmooth(Tools);
    default:;
    }
    // Изменение смещения отображается на текущем значении (спорно)
    Display->showVolt(Tools->getRealVoltage(), 3);
    Display->showAmp (Tools->getRealCurrent(), 3);                      return this;
  };

// ============ Пользовательские настройки измерителя тока ============
  MSetCurrentOffset::MSetCurrentOffset(MTools * Tools) : MState(Tools)
  {
      // Значение восстановлено из nvs при инициализации прибора
      // Индикация помощи
      Display->showMode((char*) "  I OFFSET UP/DN  ");
      Display->showHelp((char*) " B-YES P-NO C-EX  ");
  }
  MState * MSetCurrentOffset::fsm()
  {


            return new MStop(Tools);
  };










  // Завершение режима - до нажатия кнопки "С" удерживается индикация 
  // о ...
  // Состояние: "Завершение "
  MStop::MStop(MTools * Tools) : MState(Tools)
  {
    Tools->shutdownCharge();

    Display->showMode( (char*) "     OPTIONS:     " );
    Display->showHelp( (char*) "      C-STOP      " );
    Board->ledsRed();
  }    
  MState * MStop::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK:  Board->buzzerOn();  
      return new MExit(Tools);

      case MKeyboard::P_CLICK:  Board->buzzerOn();  
      return new MStart(Tools);                       // Вернуться в начало

      default:;
    }
    return this;
  };


  // Выход из режима - до нажатия кнопки "С" удерживается индикация 
  MExit::MExit(MTools * Tools) : MState(Tools)
  {
    // Индикация помощи
    Display->showMode( (char*) "     OPTIONS:     " );
    Display->showHelp( (char*) " C-RET TO SELECT  " );
  }      
  MState * MExit::fsm()
  {
    switch ( Keyboard->getKey() )
    {
      case MKeyboard::C_CLICK: Board->buzzerOn();
          Board->ledsOff();

        // Надо бы восстанавливать средствами диспетчера...
        Display->showMode( (char*) "     OPTIONS:     " );
        Display->showHelp( (char*) "   U/D B-SELECT   " );
      return nullptr;                             // Возврат к выбору режима
      default:;
    }
    return this;
  };

};

